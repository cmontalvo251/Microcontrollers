1.) Download probe-rs toolchain using website installtion / curl
2.) Download rustup toolchain using same method as above
3.) Get VS code and rust extension
4.) Make sure you have dev board , USB-C and ST-LINK debugger wire
5.) Look for Embassy Rust and STM32u5 example 
https://github.com/embassy-rs/embassy/tree/main/examples/stm32u5

This dev board is the ce variant

6.) $ rustup target add thumbv8m.main-none-eabihf

7.) $ cargo build 

that compiles the program

8.) $ cargo run --release

will compiled and run the program on the board 

9.) Need to add the rules file in this directory to /etc/udev/rules.d

then run

sudo udevadm control --reload
sudo udevadm trigger

Note the zenith-example/src/main.rs example is just to blink the dev board not to program the actual
SALT Payload

The only difference for the SALT ZENITH is this is the VE instead of CE

That example is in embassy-salt-main-master

Oh and you need to remove the jumper pins so that power to the dev board cpu is removed and rthe power goes through the ST-LINK cable