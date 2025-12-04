use embassy_sync::pubsub::WaitResult;
use postcard::to_slice;
use crate::{SensorPacket, SENSOR_PUBSUB};
use defmt::*;
use embassy_stm32::can::{CanTx, Frame};
use embassy_time::Timer;

const BASE_ID:          u16   = 0x300;           // must match your RX BASE_ID
const MAX_PACKET_SIZE:  usize = 255;             // large enough for your 125 B
const TOTAL_BYTES:      usize = 125;             // your fixed postcard size

#[embassy_executor::task]
pub async fn can_tx_task(mut tx: CanTx<'static>) {
    let mut sub = SENSOR_PUBSUB.subscriber().unwrap();
    let mut buf = [0u8; MAX_PACKET_SIZE];

    loop {
        info!("Waiting for message to send over CAN");
        let packet = sub.next_message_pure().await; // get latest packet ignoring lag
        // serialize into buf, out is the exactly-used subslice
        let out: &mut [u8] = to_slice(&packet, &mut buf[..])
            .expect("fits in MAX_PACKET_SIZE");
        info!("Data Out to CAN: {}", out);
    
        let total_bytes = out.len();
        let segments    = (total_bytes + 7) / 8;
    
        for i in 0..segments {
            // pick your window
            let start = i * 8;
            let end   = core::cmp::min(start + 8, total_bytes);
            let chunk = &out[start..end];
    
            // zero-pad to 8 bytes
            let mut data = [0u8; 8];
            data[..chunk.len()].copy_from_slice(chunk);
    
            // standard ID = BASE_ID + segment index
            let id    = BASE_ID + (i as u16);
            let frame = Frame::new_standard(id, &data).unwrap();
            
            info!("Frame {} = {}", i, frame);
    
            if let Some(dropped_frame) = tx.write(&frame).await {
                warn!("dropped lower-priority frame {:?}", dropped_frame);
            }
            Timer::after_millis(1).await;
        }
        info!("Last CAN packet sent");
        Timer::after_millis(500).await;  // adjust based on how many packets a second to be sent
    }
}
