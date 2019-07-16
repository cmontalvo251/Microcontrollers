void serialFloatPrint(float f) {
  byte * b = (byte *) &f;
  for(int i=0; i<4; i++) {
    
    byte b1 = (b[i] >> 4) & 0x0f;
    byte b2 = (b[i] & 0x0f);
    
    char c1 = (b1 < 10) ? ('0' + b1) : 'A' + b1 - 10;
    char c2 = (b2 < 10) ? ('0' + b2) : 'A' + b2 - 10;
    
    Serial.print(c1); //does this output a command between prints?
    Serial.print(c2); //otherwise line 75 on .pde won't work because their aren't any commas (Look up like 10 lines of code)
  }
}

void serialPrintFloatArr(float * arr, int length) {
  for(int i=0; i<length; i++) {
    serialFloatPrint(arr[i]);
    Serial.print(","); //The commas are here dude. 
  }
}

int numVars = 4;
String receive_message;
float serialoutarray[4];
float time_now;
float last_read = 0;
float deltaTime;
#define SEND_PERIOD 0.5

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
}

void loop()
{
  //Only read if you've got adequate data
  //No need to perform a serialEvent because we don't need multiple processes here. We
  //literally just need to perform some calculations on the Arduino and send it back.

  time_now = millis()/1000.0;
  deltaTime = time_now - last_read;
  
  if (Serial.available() and deltaTime > SEND_PERIOD) { 
    receive_message = Serial.readStringUntil('\n');
    last_read = time_now;
    float phi = 0.1*time_now;
    float theta = -0.2*time_now;
    float psi = 0.5*time_now;
    serialoutarray[0] = time_now;
    serialoutarray[1] = phi;
    serialoutarray[2] = theta;
    serialoutarray[3] = psi; 
    serialPrintFloatArr(serialoutarray,numVars);
    Serial.println("\r\n");
    Serial.flush(); //wait for outgoing data to go out
  }
}
