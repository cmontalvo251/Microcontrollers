#include "FrSkySportDecoder.h"
#include "FrSkySportSensorAss.h"

FrSkySportDecoder decoder;
FrSkySportSensorAss airspeed1(FrSkySportSensor::ID10); //Default is ID10 - is that pin 10? //No this ID is unique to the FrSky pitot sensor
//FrSkySportSensorAss airspeed2(FrSkySportSensor::ID6); //Can I just do ID5 for pin 5? 
//^^^Not sure if that will work but whatever.

//http://www.rcgroups.com/forums/showpost.php?p=29439177
//^^^2016 - If that website does not exist there is a pdf in the folder here


void setup()
{
  //Supposedly you can hook up multiple wires but I'm not sure how to code that
  //I'm assuming this means that the airspeed sensor is plugged into pin 5 and 6
  //Then the serial stuff is pin 4? Again How is this thing wired up? I'm so confused.
  //decoder.begin(FrSkySportSingleWireSerial::SOFT_SERIAL_PIN_4, &airspeed1,&airspeed2);
  decoder.begin(FrSkySportSingleWireSerial::SOFT_SERIAL_PIN_5,&airspeed1); //pin 5 is where you have to plug in the pitot probe
  //Problem is the documentation states that multiple sensors can get plugged into an S.Port device but they 
  //need a receiver. So I think to wire this up you actually need a FrSky receiver 
  //Then you can plug the airspeed sensor into the receiver and then plug the reciever into pin 5
  Serial.begin(9600);
}

void loop()
{
  // Call this on every loop
  decoder.decode();

  // Make sure that all the operations below are short or call them periodically otherwise you'll be losing telemetry data
  float speed1 = airspeed1.getSpeed();
  //float speed2 = airspeed2.getSpeed();
  
   /* DO YOUR STUFF HERE */
   Serial.println(speed1);
   //Serial.print(" ");
   //Serial.println(speed2);
}
