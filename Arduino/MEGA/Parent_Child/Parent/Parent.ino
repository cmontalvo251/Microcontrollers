
#define NUM_CHARS 6

char input_chars[NUM_CHARS];
boolean oktosend = true;

void setup() {
  Serial.begin(9600);
  Serial1.begin(115200); //sending to child
  clearDigits();
  Serial.println("Setup complete");
}

void clearDigits() {
  for (int ix = 0; ix < NUM_CHARS; ix++) {
    input_chars[ix] = '0'; 
  }
  Serial.println("Digits Cleared");
}

//Serial.println("Before Print Chars");

void printchars() {
  for (int id = 0; id < NUM_CHARS; id++) {
    Serial1.print(input_chars[id]); //last character is a newline.
  }
  Serial.println("Chars Printed");
}

//Serial.println("Before User Input");

void user_input() {  
  if (Serial.available() > 0) {
    Serial.println("Received_Data");
    clearDigits();

    Serial.println("Clearing Digits");

    int ctr = 0;
    delay(1000);
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
  user_input();
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
