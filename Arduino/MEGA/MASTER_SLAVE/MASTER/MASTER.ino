char input_chars[6]; //Data received from the serial port
unsigned long int time_now = 0;
unsigned long int time_last_send = 0;
boolean oktosend = false;

///Routines to send Floats to other arduino
void setup() {
  //Setup communication with computer
  Serial.begin(9600);
  //Setup communication with other arduino
  Serial1.begin(9600);
}

//Check For Inputs from the User
///////////////////////////////
int user_input() {
  if (Serial.available() > 0) {
    Serial.print("Serial Available!!!\n");
    for (int idx = 0;idx<7;idx++){
      input_chars[idx] = '0';
    }
    //int length_of_str = Serial.available()-1; //The minus 1 is here because of \n or newline
    //Serial.print("Length of String = ");
    //Serial.println(length_of_str);
    int ctr = 0;
    //Read the contents of the serial from computer
    while (Serial.available()) {
      input_chars[ctr] = Serial.read();
      //if (ctr < length_of_str) {
      //  Serial.print(input_chars[ctr]);
      //}
      ctr++;
      delay(10);
    }
    //Convert Number to float and then to integer
    String input_string = String(input_chars);
    Serial.print("Number Received = ");
    int outnumber = int(input_string.toFloat());
    Serial.println(outnumber);
    return outnumber;
  }
  else {
    return 0;
  }
}

void loop() {
  //Check and see if the other arduino has responded
  if (Serial1.available()) {
    while (Serial1.available()) {
      int d = Serial1.read();
    }
    Serial.println("Received Handshake from other Arduino");
    oktosend = true;
  }
  //Check for User Input
  int number = user_input();
  //If we get a number other than zero try and send to other arduino
  if (number) {
    //Make sure we haven't already sent something to the arduino
    if (oktosend) {
      Serial.println("Sending number to other arduino");
      for (int i = 0;i<6;i++) {
        Serial1.print(input_chars[i]);
      }
      Serial1.println("\r\n");
      Serial1.flush(); //wait for outgoing data to go out
      //Reset oktosend
      oktosend = false;
    }
  }
}
