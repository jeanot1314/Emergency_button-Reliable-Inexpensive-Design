
#include <ESP8266WiFi.h>

const char* ssid = "SSID"; // Add the name of the wifi network  ******************
const char* password = "PASSWORD"; // Add the password of your wifi network*******************

WiFiServer server(80);
WiFiClient client;

byte mac[] = { 0xFF, 0xFF, 0xFF, 0xFF ,0xFF, 0xFF }; // Write the MAC Address of your board, must be unique********************

IPAddress ip( 192, 168, 0, 3 ); // change network settings to yours *********************
IPAddress gateway( 192, 168, 0, 1 );//*********************
IPAddress subnet( 255, 255, 255, 0 );//***********************

char smtpcorp[] = "mail.smtp2go.com";
int port = 2525;

#define LED_PIN 0
#define BUTTON_PIN 2

extern "C" {
  #include "user_interface.h"
}

void setup()
{
Serial.begin(115200);
pinMode(BUTTON_PIN, INPUT);
pinMode(LED_PIN, OUTPUT);
digitalWrite(BUTTON_PIN, HIGH);
digitalWrite(LED_PIN, HIGH);

server.begin();

Serial.println();
Serial.println(F("Press button so send a EMAIL"));
delay(1000);

for(int i=0; i<3; i++){ // Bling the LED to show the program started
  digitalWrite(LED_PIN, LOW);
  delay(200);
  digitalWrite(LED_PIN, HIGH);
  delay(200);
}

}

void loop()
{
  digitalWrite(LED_PIN, LOW);
  delay(20);
  
  if(digitalRead(BUTTON_PIN) == LOW){ // If the button is pressed
    Serial.println("------  Starting to send an EMAIL  ------");
    for(int i=0; i<2; i++){
      digitalWrite(LED_PIN, LOW);
      delay(200);
      digitalWrite(LED_PIN, HIGH);
      delay(200);
    }
    if(sendEmail()) {
      Serial.println(F("Email sent"));
      for(int i=0; i<4; i++){
        digitalWrite(LED_PIN, HIGH);
        delay(200);
        digitalWrite(LED_PIN, LOW);
        delay(200);
      }
    }
    else {Serial.println(F("Email failed"));digitalWrite(LED_PIN, LOW);} 
    ESP.deepSleep(0, WAKE_RF_DEFAULT); 
    delay(10000); //Security, the sleep mode need some time to activate   
  }
}


byte sendEmail()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");   
  Serial.println(WiFi.localIP());
  byte thisByte = 0;
  byte respCode;
                                                               
  if(client.connect(smtpcorp,port) == 1) {
    Serial.println(F("connected smtp2go"));
  } else {
    Serial.println(F("connection failed smtp2go"));
    digitalWrite(LED_PIN, LOW);
    return 0;
  }
  
  if(!eRcv()) {Serial.println("before ehlo");digitalWrite(LED_PIN, LOW);return 0 ;}
  Serial.println(F("Sending ehlo"));
  client.println("EHLO 176.189.221.155");
  if(!eRcv()) {Serial.println("ehlo");digitalWrite(LED_PIN, LOW);return 0 ;}
  
  Serial.println(F("Sending auth login"));
  client.println("auth login");
  if(!eRcv()) {Serial.println("auth");digitalWrite(LED_PIN, LOW);return 0 ;}
  
  Serial.println("Sending User");
  // Change to your base64 encoded user
  client.println("yourEMAIL@gmail.com_base_64");//your smtp2go Email in base 64 ******************
  // You can go on http://www.motobit.com/util/base64-decoder-encoder.asp to translate
  if(!eRcv()) {Serial.println("user");digitalWrite(LED_PIN, LOW);return 0 ;}
  
  Serial.println(F("Sending Password"));
  client.println("Password_Base_64");// your smtp2go password in base 64 ****************************
  // You can go on http://www.motobit.com/util/base64-decoder-encoder.asp to translate
  if(!eRcv()) {Serial.println("ehlo");digitalWrite(LED_PIN, LOW);return 0;}
  
  Serial.println(F("Sending From"));
  client.println("MAIL From: <yourEMAIL@gmail.com>");  // Add your Email address here *********************
  if(!eRcv()) {Serial.println("email");digitalWrite(LED_PIN, LOW);return 0 ;}
  
  Serial.println(F("Sending To"));
  client.println("RCPT To: <yourEMAIL@gmail.com>"); // Add your Email address here **********************
  if(!eRcv()) {Serial.println("email");digitalWrite(LED_PIN, LOW);return 0 ;}
  
  Serial.println(F("Sending DATA"));
  client.println("DATA");
  if(!eRcv()) {Serial.println("email");digitalWrite(LED_PIN, LOW);return 0 ;}
  
  Serial.println(F("Sending email"));
  client.println("To: You <yourEMAIL@gmail.com>"); // Add your Email address here  *******************
  client.println("From: Mr Dupont <yourEMAIL@gmail.com>"); // Change the From : XYZ  *******************
  client.println("Subject: URGENT");
  client.println("The Button of Mr DUPONT has been activated"); // Change the content ********************
  client.println(".");
  if(!eRcv()) {Serial.println("aftersending");digitalWrite(LED_PIN, LOW);return 0 ;}
  
  Serial.println(F("Sending QUIT")); 
  client.println("QUIT");
  if(!eRcv()) {Serial.println("afterQuit");return 0 ;}
  
  client.stop();
  Serial.println(F("disconnected"));
  return 1;
}

byte eRcv(){
  byte respCode;
  byte thisByte;
  int loopCount = 0;
  while(!client.available()) {
    delay(1);
    loopCount++;
    if(loopCount > 10000) {// if nothing received for 10 seconds, timeout
      client.stop();
      Serial.println(F("10 secTimeout"));
      return 0;
    }
  }
  respCode = client.peek();
  while(client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }
  if(respCode >= '4')
  {
    efail();
    return 0;
  }
  return 1;
}

void efail()
{
  byte thisByte = 0;
  int loopCount = 0;
  client.println(F("QUIT"));
  while(!client.available()) {
    delay(1);
    loopCount++;
    if(loopCount > 10000) {// if nothing received for 10 seconds, timeout
      client.stop();
      Serial.println(F("efailTimeout"));
      return;
    }
  } 
  while(client.available())
  {
    thisByte = client.read();
    Serial.write(thisByte);
  }
  client.stop();
  Serial.println(F("disconnected"));
}


