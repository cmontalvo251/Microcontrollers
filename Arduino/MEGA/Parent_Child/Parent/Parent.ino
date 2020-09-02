
#define NUM_CHARS 4

char input_chars[NUM_CHARS];
boolean oktosend = true;

#define onlyTrigPin 9
#define onlyEchoPin 8

void SetupPin(int TrigPin, int EchoPin) {
  pinMode(TrigPin, OUTPUT);
  pinMode(EchoPin, INPUT);
}

long Distance(int TrigPin, int EchoPin) {
  long duration;
  digitalWrite(TrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin, LOW);
  duration = pulseIn(EchoPin, HIGH);
  long cm = (duration / 2.0) / 29.1;
  if (cm > 180) {
    cm = 180;
  }
  return cm;
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200); //sending to child
  clearDigits();
  SetupPin(onlyTrigPin, onlyEchoPin);
  Serial.println("Setup complete");
}

void clearDigits() {
  for (int ix = 0; ix < NUM_CHARS; ix++) {
    input_chars[ix] = '0';
  }
  //Serial.println("Digits Cleared");
}


void printchars() {
  for (int id = 0; id < NUM_CHARS; id++) {
    Serial1.print(input_chars[id]); //last character is a newline.
  }
  Serial.println("Chars Printed");
}

void user_input() {
  if (Serial.available() > 0) {
    Serial.println("Received_Data");
    clearDigits();

    Serial.println("Clearing Digits");

    int ctr = 0;
    delay(100);
    while (Serial.available()) {
      input_chars[ctr] = Serial.read();
      Serial.print(input_chars[ctr]);
      ctr++;
    }

    //Send this to child
    if (oktosend) {
      printchars();
      oktosend = false;
      Serial1.flush();
      delay(1000);
    }

    String input_string = String(input_chars);
    Serial.println("Number Received = ");
    int outnumber = int(input_string.toFloat());
    Serial.println(outnumber);
  }
}

void loop() {
  //Check for a user input
  //user_input();

  ///////////?READ PROX//////////////////
  long cm = Distance(onlyTrigPin, onlyEchoPin);
  Serial.print("Float = ");
  Serial.println(cm);
  String cmStr = String(cm);
  //Serial.print("String = ");
  //Serial.println(cmStr);
  int ctr = NUM_CHARS-cmStr.length()-1;
  clearDigits();
  for (int i = 0;i<cmStr.length();i++) {
    input_chars[ctr] = cmStr.charAt(i);
    ctr++;
  }
  input_chars[NUM_CHARS-1] = '\n';
  //////////////////////////////////////////

  //Send this to child
  if (oktosend) {
    printchars();
    oktosend = false;
    Serial1.flush();
    Serial.println("Sent data to Arduino");
    delay(100);
  }
  
  //for (int id = 0; id < NUM_CHARS; id++) {
  //  Serial.print(input_chars[id]); //last character is a newline.
  //}
  //Serial.print("\n");
  //Serial.println("Chars Printed");
  //delay(1000);
  //Serial.println("User Input Working"); //User Input Working
  //If ok to send. send to child
  if (!oktosend) {
    if (Serial1.available()) {
      Serial.println("Child has responded");
      char input = Serial1.read();
      if (input == '\n') {
        oktosend = true;
      }
    }
  }
}
