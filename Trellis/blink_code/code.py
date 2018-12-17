import adafruit_trellis_express

trellis = adafruit_trellis_express.TrellisM4Express(rotation=90)

while True:
	trellis.pixels[(0,0)] = (255,0,0)
	time.sleep(2)
	trellis.pixels[(0,0)] = (0,0,0)
	time.sleep(2)
