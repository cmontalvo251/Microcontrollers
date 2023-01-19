///This code is used to test the driving code for ESCs taken from Kyrell_God

#define PWM_RESOLUTION_MAX 255 //4095 for 12bit and 255 for 8-bit
#define MOTORS_ESC_PWM_MIN 1000
#define MOTORS_ESC_PWM_MAX 2000
#define MOTORS_8BIT_LIMIT PWM_RESOLUTION_MAX
#define MOTORS_ESC_PWM_FREQ 400

float pwm0 = MOTORS_ESC_PWM_MIN;

#define MOTORS_PIN_PWM0 8 //Kyrell_God code uses pins 6-9 so double check that these are PWM on the Arduino UNO
//Right now my motors are connected to pins 8-11 so if I choose pin 8 I might be able to test this code on here.
//I'm a little hesitant to test this with an ESC with a prop on it so let's try a servo at work tomorrow
//Ok so I just looked this up from the Arduino Website
//PWM: 3, 5, 6, 9, 10, and 11. Provide 8-bit PWM output with the analogWrite() function.
//So there you go. You can use the analogWrite function to control ESCs. So what does that mean about the DUE?
//We should use AnalogWrite just like KyrellGod does. Otherwise it won't work right. We may need to move some solder around
//But hopefully we can make it work.

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(MOTORS_PIN_PWM0,OUTPUT);
}

void loop() {
  //Max Pulse Length
  float arduinoMaxPulseLength = 1000000.0f/MOTORS_ESC_PWM_FREQ;
  //Serial.print(" Mpulse = "); //Yea so this is 1000000/400 = 2500
  //Serial.print(arduinoMaxPulseLength);
  float tmp = map(MOTORS_ESC_PWM_MIN, 0.0f, arduinoMaxPulseLength, 0.0f, (float) MOTORS_8BIT_LIMIT);
  //This tmp maps 1000 - x = [0,2500 and 0,4095] so 1000*4095/2500 = 1638
  //Serial.print(" tmp = ");
  //Serial.print(tmp);
  tmp = ceil(tmp);
  //The ceiling of 1638 is then 1638.
  //Serial.print(" ceil(tmp) = ");
  //Serial.print(tmp);
  int lowerClippingValue = (int) tmp; ///this just sets some upper and lower values 
  //Serial.print(" Lower = ");
  //Serial.println(lowerClippingValue);

  //Set PWM Signal
  pwm0+=100;
  if (pwm0 > MOTORS_ESC_PWM_MAX) {
    pwm0 = MOTORS_ESC_PWM_MIN;
  }
  Serial.print("PWM = ");
  Serial.print(pwm0);
  //Constraing between 1000 and 2000
  float motor0 = constrain(pwm0, MOTORS_ESC_PWM_MIN, MOTORS_ESC_PWM_MAX);
  //Map to 8BIT_LIMIT
  motor0 = map(motor0, 0.0f, arduinoMaxPulseLength, 0.0f, (float) MOTORS_8BIT_LIMIT);
  Serial.print(" Motor0 = ");
  Serial.print(motor0);
  //Convert to Integers
  int motor0i = (int) motor0;
  Serial.print(" Mint = ");
  Serial.println(motor0i);
  //Constrain 1 more time based on 8BIT_LIMIT and lowerClippingValue
  motor0i = constrain(motor0i, lowerClippingValue, MOTORS_8BIT_LIMIT);
  //When you're ready send this to a motor
  //So I just scoured through the documentation for Servos and Servo.h hijacks ALL
  //the timers so that's definitely not what you want to do. According to the documentation
  //when you send Servo.writeMicroseconds all it does is change the servos.tick to the number of ticks
  //per clock cycle. Then there is an interrupt that handles the pulse. 
  //I'm not positive on this but I think if you send an analog signal to a servo between 0 to 255 
  //so long as the port is a PWM port the servo will move.
  //Gotta test this but let's see
  analogWrite(MOTORS_PIN_PWM0, motor0i);
  
}
