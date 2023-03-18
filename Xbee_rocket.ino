#define HWSERIAL Serial4
String command;
int count = 0;
int led = 13;
bool done = 0;
String message = "";
String incomingString = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //while (!Serial) {
  //}
  HWSERIAL.begin(9600);
  pinMode(led, OUTPUT);
}

void loop() {
  /*if(Serial.available()){
    if (command.equals("CAMERA")) {
      delay(10000);
      HWSERIAL.println("COMMAND RECIEVED --> CAMERA ON\n");
      delay(500);
    }
    else if (command.equals("ROCKET")) {
      delay(10000);
      HWSERIAL.println("COMMAND RECIEVED --> ROCKET DOWNLINKING...\n");
      delay(500);
    }
  }*/
  if (HWSERIAL.available() > 0) {
    //String incomingString = HWSERIAL.readStringUntil("CNF");
    while (done == 0){
      char incomingByte = HWSERIAL.read();
      if (isAscii(incomingByte) == true) {
        message += incomingByte;
      }
      if (message.length() > 0) {
        if (message.substring(message.length() - 3) == "CNF"){
          message.trim();
          message.remove((message.length() - 3),3);
          incomingString = message;
          done = 1;
        }
      }
    }
  }
  if (done == 1) {
    if (message.substring(0,11) == "MelbARESroc") {
      digitalWrite(led, HIGH);
      HWSERIAL.println("MelbARESgrd,Command: \"" + incomingString.trim() + "\" has been receivedEND"); 
      message = "";
      HWSERIAL.flush();
      HWSERIAL.clear();
      done = 0;
      digitalWrite(led, LOW);  
    }
  }
  String countstr = String(count);
  HWSERIAL.println("MelbARESgrd," + countstr + ",-31.523123,115.123692,1000,412,69,96,123532END"); 
  digitalWrite(led, HIGH);
  delay(10);
  digitalWrite(led, LOW);
  if (count == 2147483646) {
    count = 0;
  }
  else {
    count++;
  }
  delay(500);
}
