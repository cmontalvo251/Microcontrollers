float data[3];

void setup() {
 Serial.begin(9600); 
}


void loop () {
  //String
  String val = "1.25,2.34,5.43";
  //float fval = val.toFloat(); //Note this only works on 1.6.9
  parseMessage(val);
  
  for (int idx = 0;idx<3;idx++) {
    Serial.print(data[idx]*2.3);
    Serial.print(" ");
  }
  Serial.print("\n");
  
}

void parseMessage(String val) {
  String current_val;
  int ctr = 0;
  for (int idx = 0;idx<val.length();idx++){
    if (val[idx] == ',') {
      data[ctr] = current_val.toFloat();
      current_val = "";
      ctr++;
    }
    else {
      current_val+=val[idx];
    }
  }
  data[2] = current_val.toFloat(); //Need to get the last one
}

