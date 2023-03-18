/*
RX is digital pin 2 (connect to TX of other device)
TX is digital pin 3 (connect to RX of other device)
*/

#include <SoftwareSerial.h>

int led = 13;
bool done = 0;
String message = "";
String incomingString = "";
SoftwareSerial mySerial(2, 3); // RX, TX 
char command[20]; // buffer to store command entered by user
char response[50]; // buffer to store response from XBee module
int i = 0; // index for command buffer
int whilecheck = 0;
bool done1 = 0;
bool GrdXbeeConfig = 0;
String message1 = "";

//setup Xbee and Arduino
void setup()
{
  Serial.begin(9600);
  while (!Serial) {
  }
  mySerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
  //Read data from rocket
  if (mySerial.available() > 0) {
    //incomingString = mySerial.readStringUntil('\n');
    while (done == 0){
      char incomingByte = mySerial.read();
      if (isAscii(incomingByte) == true) {
        message += incomingByte;
      }
      //validate rocket data format suffix
      if (message.length() > 0) {
        if (message.substring(message.length() - 3) == "END"){
          message.trim();
          message.remove((message.length() - 3),3);
          incomingString = message;
          done = 1;
        }
      }
    }
    //validate rocket data format prefix
    if (message.substring(0,11) == "MelbARESgrd") {
      Serial.println(incomingString);
      if (incomingString.length() > 0) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(5);
        digitalWrite(LED_BUILTIN, LOW);  
      }
      done = 0;
      message = "";
    }
  }

  //configure xbee settings by AT command
  if (Serial.available() > 0) {
    done = 0;
    char c = Serial.read();
    if (c == '\n') { // end of command, send it to XBee module
      String sendCommand = String(command);
      sendCommand.trim();
      if (sendCommand.substring(0,2) == "AT"){
        if (sendCommand.substring(sendCommand.length() - 3) == "grd"){
          mySerial.print("+++");
          done1 = 0;
          while (done1 == 0){
            char incomingByte = mySerial.read();
            if (isAscii(incomingByte) == true) {
              message1 += incomingByte;
            }
            if (message1.length() > 1) {
              if (message1.substring(message1.length() - 2) == "OK"){
                message1.trim();
                done1 = 1;
                Serial.println("Entered command mode: OK"); //If use message1 it will contain rocket data
                message1 = "";
                GrdXbeeConfig = 1;
              }
            }
          }
          //delay(1050);
          sendCommand = sendCommand.substring(0, (String(command).length() - 3));
          sendCommand.trim();
          mySerial.print(sendCommand + '\r');
          Serial.println("Sent command to ground XBee module: " + sendCommand);
          i = 0; // reset index for next command
          command[i] = '\0';
        }
        else if (sendCommand.substring(sendCommand.length() - 3) == "roc") {
          sendCommand = sendCommand.substring(0, (String(command).length() - 3));
          sendCommand.trim();
          mySerial.println("MelbARESroc," + sendCommand + "CNF"); 
          Serial.println("Sent command: " + sendCommand + " to ROCKET");
        }
        else if (sendCommand.substring(sendCommand.length() - 3) == "cam") {
          sendCommand = sendCommand.substring(0, (String(command).length() - 3));
          sendCommand.trim();
          mySerial.println("MelbAREScam," + sendCommand + "CNF"); 
          Serial.println("Sent command: " + sendCommand + " to CAMERA");
        }
        else {
          Serial.println("ERROR. Specify destination module (grd/roc/cam).");
        }
      } 
      else {
        Serial.println("ERROR. Not an AT command.");
      }
    }
    else {
      // add character to command buffer
      command[i++] = c;
      command[i] = '\0'; // terminate the string
    }
  }

  // check if there's any response from XBee module
  if ((mySerial.available() > 0) && (GrdXbeeConfig == 1)) {
    whilecheck = 0;
    while (done == 0){
      char incomingByte = mySerial.read();
      if (isAscii(incomingByte) == true) {
        message += incomingByte;
      }
      if (message.length() > 0) {
        if (message.substring(message.length() - 2) == "OK"){
          message.trim();
          done = 1;
          Serial.println("Response from XBee module: " + message);
          mySerial.print("ATCN" + '\r');
          delay(100);
          message = "";
        }
        else if (message.substring(message.length() - 5) == "ERROR"){
          message.trim();
          done = 1;
          Serial.println("Response from XBee module: " + message);
          mySerial.print("ATCN" + '\r');
          delay(100);
          message = "";
        }
        else if (whilecheck > 50){
        message.trim();
        done = 1;
        Serial.println("Response from XBee module: " + message);
        mySerial.print("ATCN" + '\r');
        delay(100);
        message = "";
        }
      }
      if (whilecheck > 55){
        done = 1;
        Serial.println("Wait... Self fixing loop error");
      }
      whilecheck++;
    }
    done = 0;
    GrdXbeeConfig = 0;
  }
}