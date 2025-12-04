use defmt::{info, Format};
use embassy_stm32::mode::Async;
use embassy_stm32::ospi::{
    Ospi, Instance,
    TransferConfig, OspiError,
    OspiWidth, AddressSize, DummyCycles,
};
use embassy_stm32::peripherals::OCTOSPI1;
use embassy_sync::mutex::MutexGuard;
use embassy_sync::pubsub::WaitResult;
use embassy_time::Timer;
use heapless::Deque;
use postcard::{from_bytes, to_slice};
use serde::{Deserialize, Serialize};
use crate::{usb_write, SensorPacket, FLASH, SENSOR_PUBSUB};

/// Winbond commands
const CMD_ENABLE_RESET:    u8 = 0x66;
const CMD_RESET_DEVICE:    u8 = 0x99;
const CMD_RELEASE_PD_ID:   u8 = 0xAB;
const CMD_READ_JEDEC_ID:   u8 = 0x9F;
const CMD_READ_SR:         u8 = 0x05;
const CMD_WRITE_ENABLE:    u8 = 0x06;
const CMD_SECTOR_ERASE:    u8 = 0x20;
const CMD_PAGE_PROGRAM:    u8 = 0x02;
const CMD_CHIP_ERASE:      u8 = 0xC7;
const CMD_FAST_READ: u8 = 0x0B;
const CMD_READ: u8 = 0x03;

/// How many pages to keep in RAM before trigger
const BUFFER_PAGES: usize = 128;
/// Flash page and sector sizes
pub const PAGE_SIZE: usize = 256;
pub const SECTOR_SIZE: usize = 4096;
pub const HEADER_SLOTS: usize = SECTOR_SIZE / PAGE_SIZE; // 16
/// “G‐force” accel threshold
const ACCEL_THRESHOLD: f32 = 10.0;  // Gs

// Header definition at flash offset 0
#[derive(Serialize, Deserialize, Default, Format)]
pub struct Header {
    pub marker: u32,
    pub version: u32,
    pub pages:  u32,
}
pub const HEADER_MAGIC: u32 = 0xDEADBEEF;

#[embassy_executor::task]
pub async fn store_sensor_data() {
    // — 1) On boot, scan both header sectors, pick the freshest —
    let mut best_header = Header::default();
    let mut last_used_sector: u32;

    {
        let mut guard = FLASH.lock().await;
        let flash = guard.as_mut().unwrap();

        // Read sector 0
        let mut buf0 = [0u8; PAGE_SIZE];
        flash.read_data(0, &mut buf0).await.unwrap();
        let hdr0: Header =
            from_bytes(&buf0[..size_of::<Header>()]).unwrap_or_default();

        // Read sector 1
        let mut buf1 = [0u8; PAGE_SIZE];
        flash.read_data(SECTOR_SIZE as u32, &mut buf1).await.unwrap();
        let hdr1: Header =
            from_bytes(&buf1[..size_of::<Header>()]).unwrap_or_default();

        // Choose the one with the higher version
        if hdr0.marker == HEADER_MAGIC && hdr0.version >= hdr1.version {
            best_header      = hdr0;
            last_used_sector = 0;
        } else if hdr1.marker == HEADER_MAGIC {
            best_header      = hdr1;
            last_used_sector = 1;
        } else {
            // No valid header at all → start fresh
            flash.erase_sector(0).await.unwrap();
            best_header      = Header::default();
            last_used_sector = 1; // so we’ll write sector 0 first
        }
    }

    // — 2) Resume your data write pointer —
    let mut pages_written = best_header.pages as usize;
    let mut write_addr = SECTOR_SIZE as u32
        + (pages_written as u32) * (PAGE_SIZE as u32);
    info!(
        "Resuming at page {}, from sector {}",
        pages_written, last_used_sector
    );

    // — 3) Your ring/trigger loop —
    let mut ring: Deque<[u8; PAGE_SIZE], BUFFER_PAGES> = Deque::new();
    let mut triggered = false;
    let mut data_sub = SENSOR_PUBSUB.subscriber().unwrap();

    loop {
        // a) pull next SensorPacket …
        let packet = match data_sub.next_message().await {
            WaitResult::Message(pkt) => pkt,
            WaitResult::Lagged(e) => {
                info!("Gather Sensor Packet Lagged = {}", e);
                continue;
            }
        };

        // b) pack into a 256 B page
        let mut page = [0xFFu8; PAGE_SIZE];
        to_slice(&packet, &mut page).unwrap();

        if !triggered {
            // buffer until threshold
            if ring.len() == BUFFER_PAGES {
                ring.pop_front();
            }
            ring.push_back(page);

            if packet.lsm_accel.z.abs() > ACCEL_THRESHOLD {
                triggered = true;
                info!("ACCEL THRESHOLD REACHED: {} G", packet.lsm_accel.z);

                // flush ring → flash
                while let Some(p) = ring.pop_front() {
                    if write_addr % (SECTOR_SIZE as u32) == 0 {
                        let mut g = FLASH.lock().await;
                        g.as_mut().unwrap().erase_sector(write_addr).await.unwrap();
                    }
                    {
                        let mut g = FLASH.lock().await;
                        g.as_mut().unwrap().program_page(write_addr, &p).await.unwrap();
                    }
                    write_addr += PAGE_SIZE as u32;
                    pages_written += 1;
                }
                info!("Wrote {} pages after threshold", pages_written);

                // **journal** the header
                let new_hdr = Header {
                    marker:  HEADER_MAGIC,
                    version: best_header.version.wrapping_add(1),
                    pages:   pages_written as u32,
                };
                let mut hbuf = [0xFFu8; PAGE_SIZE];
                to_slice(&new_hdr, &mut hbuf[..size_of::<Header>()]).unwrap();

                // decide target sector (the one *not* used last time)
                let target_sector = if last_used_sector == 0 {
                    SECTOR_SIZE as u32
                } else {
                    0
                };
                {
                    let mut g = FLASH.lock().await;
                    let f = g.as_mut().unwrap();
                    // **erase** target, **then** program
                    f.erase_sector(target_sector).await.unwrap();
                    f.program_page(target_sector, &hbuf).await.unwrap();
                }
                info!(
                    "Wrote header to sector 0x{:X}: {:?}",
                    target_sector, new_hdr
                );

                // bump for next time
                last_used_sector = target_sector / (SECTOR_SIZE as u32);
                best_header      = new_hdr;
            }
        } else {
            // post-trigger: write each arriving packet
            if write_addr % (SECTOR_SIZE as u32) == 0 {
                let mut g = FLASH.lock().await;
                g.as_mut().unwrap().erase_sector(write_addr).await.unwrap();
            }
            {
                let mut g = FLASH.lock().await;
                g.as_mut().unwrap().program_page(write_addr, &page).await.unwrap();
            }
            write_addr += PAGE_SIZE as u32;
            pages_written += 1;
            info!(
                "Wrote page #{}, addr=0x{:X}",
                pages_written, write_addr
            );

            // journal the updated header
            let new_hdr = Header {
                marker:  HEADER_MAGIC,
                version: best_header.version.wrapping_add(1),
                pages:   pages_written as u32,
            };
            let mut hbuf = [0xFFu8; PAGE_SIZE];
            to_slice(&new_hdr, &mut hbuf[..size_of::<Header>()]).unwrap();

            let target_sector = if last_used_sector == 0 {
                SECTOR_SIZE as u32
            } else {
                0
            };
            {
                let mut g = FLASH.lock().await;
                let f = g.as_mut().unwrap();
                f.erase_sector(target_sector).await.unwrap();
                f.program_page(target_sector, &hbuf).await.unwrap();
            }
            info!(
                "Wrote header to sector 0x{:X}: {:?}",
                target_sector, new_hdr
            );

            last_used_sector = target_sector / (SECTOR_SIZE as u32);
            best_header      = new_hdr;
        }
    }
}



pub struct FlashMemory<T: Instance> {
    ospi: Ospi<'static, T, Async>,
}

impl<T: Instance> FlashMemory<T> {
    /// Take ownership of your `Ospi::new_singlespi(...)` handle
    pub fn new(ospi: Ospi<'static, T, Async>) -> Self {
        Self { ospi }
    }

    /// Send a no-data command (reset, write-enable, chip-erase, etc.)
    async fn exec_cmd(&mut self, cmd: u8) -> Result<(), OspiError> {
        let dummy = [0u8];
        let mut cfg = TransferConfig::default();
        cfg.iwidth      = OspiWidth::SING;
        cfg.instruction = Some(u32::from(cmd));
        cfg.isize       = AddressSize::_8Bit;
        cfg.adwidth     = OspiWidth::NONE;
        cfg.dwidth      = OspiWidth::NONE;
        cfg.dummy       = DummyCycles::_0;
        self.ospi.write(&dummy, cfg).await
    }

    /// Release from deep-powerdown & reset
    pub async fn reset_device(&mut self) -> Result<(), OspiError> {
        self.exec_cmd(CMD_ENABLE_RESET).await?;
        self.exec_cmd(CMD_RESET_DEVICE).await?;
        self.wait_busy().await
    }

    pub async fn clear_header(&mut self) -> Result<(), OspiError> {
        // Sector 0 covers 0x0000–0x0FFF, which includes the entire header page
        self.erase_sector(0).await
    }

    /// Read status-register (0x05)
    pub async fn read_sr(&mut self) -> Result<u8, OspiError> {
        let mut buf = [0u8];
        let mut cfg = TransferConfig::default();
        cfg.iwidth      = OspiWidth::SING;
        cfg.instruction = Some(u32::from(CMD_READ_SR));
        cfg.isize       = AddressSize::_8Bit;
        cfg.adwidth     = OspiWidth::NONE;
        cfg.dwidth      = OspiWidth::SING;
        cfg.dummy       = DummyCycles::_0;
        self.ospi.read(&mut buf, cfg).await?;
        Ok(buf[0])
    }

    /// Block until WIP bit (b0 of SR) goes low
    pub async fn wait_busy(&mut self) -> Result<(), OspiError> {
        while self.read_sr().await? & 0x01 != 0 {
            // yield to other tasks
            embassy_futures::yield_now().await;
        }
        Ok(())
    }

    /// Read 3-byte JEDEC ID (0x9F)
    pub async fn read_jedec_id(&mut self) -> Result<[u8; 3], OspiError> {
        let mut id = [0u8; 3];
        let mut cfg = TransferConfig::default();
        cfg.iwidth      = OspiWidth::SING;
        cfg.instruction = Some(u32::from(CMD_READ_JEDEC_ID));
        cfg.isize       = AddressSize::_8Bit;
        cfg.adwidth     = OspiWidth::NONE;
        cfg.dwidth      = OspiWidth::SING;
        cfg.dummy       = DummyCycles::_0;
        self.ospi.read(&mut id, cfg).await?;
        Ok(id)
    }

    /// Send Write Enable (0x06)
    pub async fn enable_write(&mut self) -> Result<(), OspiError> {
        self.exec_cmd(CMD_WRITE_ENABLE).await
    }

    /// Erase one 4 KiB sector at `addr` (0x20)
    pub async fn erase_sector(&mut self, addr: u32) -> Result<(), OspiError> {
        self.enable_write().await?;
        let dummy = [0u8];
        let mut cfg = TransferConfig::default();
        cfg.iwidth      = OspiWidth::SING;
        cfg.instruction = Some(u32::from(CMD_SECTOR_ERASE));
        cfg.isize       = AddressSize::_8Bit;
        cfg.adwidth     = OspiWidth::SING;
        cfg.address     = Some(addr);
        cfg.adsize      = AddressSize::_24bit;
        cfg.dwidth      = OspiWidth::NONE;
        cfg.dummy       = DummyCycles::_0;
        self.ospi.write(&dummy, cfg).await?;
        self.wait_busy().await
    }

    /// Program up to 256 bytes at `addr` (0x02)
    pub async fn program_page(&mut self, addr: u32, data: &[u8]) -> Result<(), OspiError> {
        assert!(data.len() <= 256);
        self.enable_write().await?;
        let mut cfg = TransferConfig::default();
        cfg.iwidth      = OspiWidth::SING;
        cfg.instruction = Some(u32::from(CMD_PAGE_PROGRAM));
        cfg.isize       = AddressSize::_8Bit;
        cfg.adwidth     = OspiWidth::SING;
        cfg.address     = Some(addr);
        cfg.adsize      = AddressSize::_24bit;
        cfg.dwidth      = OspiWidth::SING;
        cfg.dummy       = DummyCycles::_0;
        self.ospi.write(data, cfg).await?;
        self.wait_busy().await
    }

    /// Asynchronously read `buf.len()` bytes from `addr` using Fast-Read (0x0B)
    pub async fn fast_read_data(&mut self, addr: u32, buf: &mut [u8]) -> Result<(), OspiError> {
        let mut cfg = TransferConfig::default();

        // Instruction phase: 0x0B
        cfg.iwidth      = OspiWidth::SING;
        cfg.instruction = Some(u32::from(CMD_FAST_READ));
        cfg.isize       = AddressSize::_8Bit;

        // Address phase: send 24-bit addr
        cfg.adwidth     = OspiWidth::SING;
        cfg.address     = Some(addr);
        cfg.adsize      = AddressSize::_24bit;
        cfg.addtr       = false;

        // Dummy phase: 8 cycles = 1 byte
        cfg.dummy       = DummyCycles::_8;

        // Data phase: read on single-SPI
        cfg.dwidth      = OspiWidth::SING;
        cfg.ddtr        = false;

        // kick off the async read
        self.ospi.read(buf, cfg).await
    }

    /// Asynchronously read `buf.len()` bytes from `addr` using the basic Read-Data (0x03) command
    pub async fn read_data(&mut self, addr: u32, buf: &mut [u8]) -> Result<(), OspiError> {
        let mut cfg = TransferConfig::default();

        // Instruction phase: 0x03
        cfg.iwidth      = OspiWidth::SING;
        cfg.instruction = Some(u32::from(CMD_READ));
        cfg.isize       = AddressSize::_8Bit;

        // Address phase: 24-bit address
        cfg.adwidth     = OspiWidth::SING;
        cfg.address     = Some(addr);
        cfg.adsize      = AddressSize::_24bit;
        cfg.addtr       = false;

        // No dummy cycles
        cfg.dummy       = DummyCycles::_0;

        // Data phase: single-SPI
        cfg.dwidth      = OspiWidth::SING;
        cfg.ddtr        = false;

        // Kick off the async read
        self.ospi.read(buf, cfg).await
    }


    /// Erase entire chip (0xC7)
    pub async fn erase_chip(&mut self) -> Result<(), OspiError> {
        self.enable_write().await?;
        self.exec_cmd(CMD_CHIP_ERASE).await?;
        self.wait_busy().await
    }
}
