import board
import busio
import adafruit_mcp4728

i2c = busio.I2C(board.SCL, board.SDA)
mcp4728 =  adafruit_mcp4728.MCP4728(i2c)

#pylint:disable=no-member
mcp4728.channel_a.vref = adafruit_mcp4728.Vref.VDD # sets the channel to scale between 0v and VDD
mcp4728.channel_b.vref = adafruit_mcp4728.Vref.VDD
mcp4728.channel_c.vref = adafruit_mcp4728.Vref.VDD
mcp4728.channel_d.vref = adafruit_mcp4728.Vref.INTERNAL # scales between 0v and 2.048v

mcp4728.channel_a.gain = 1
mcp4728.channel_b.gain = 1
mcp4728.channel_c.gain = 1

# changes the gain to 2 so the channel now scales between 0v and 4.098V (2 x Internal Vref)
mcp4728.channel_d.gain = 2


mcp4728.channel_a.value = 65535 # Voltage = VDD
mcp4728.channel_b.value = int(65535/2) # VDD/2
mcp4728.channel_c.value = int(65535/4) # VDD/4

mcp4728.channel_d.value = int(65535/4) # Vref.INTERNAL/4 * 2 => 2.048/4 * 2 = 2.048/2 =  ~ 1.024V

print("cha value:", mcp4728.channel_a.value)
print("chb value:", mcp4728.channel_b.value)
print("chc value:", mcp4728.channel_c.value)
print("chd value:", mcp4728.channel_d.value)
print()

print("cha gain:", mcp4728.channel_a.gain)
print("chb gain:", mcp4728.channel_b.gain)
print("chc gain:", mcp4728.channel_c.gain)
print("chd gain:", mcp4728.channel_d.gain)
print()
print("cha vref:", mcp4728.channel_a.vref)
print("chb vref:", mcp4728.channel_b.vref)
print("chc vref:", mcp4728.channel_c.vref)
print("chd vref:", mcp4728.channel_d.vref)


mcp4728.save_settings()
