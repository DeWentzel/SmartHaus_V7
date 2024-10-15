list of components
//1. Arduino Uno R4 WiFi
//2. A4988 Stepper Motor Driver
//3, NEMA 17 Stepper Motor
//4. 3x pushbuttons
//5. Temp Sensor
//6. SSD1306 OLED Display
//7. buzzer
//8. 2x RGB LEDS
//9. 80g Servo
//10. Relay
//11. 12V Fan
//12. 2x IR Sensors
//ESP32 S3 (Built into Arduino Uno R4 WiFi)


#include <SPI.h>
#include <WiFiS3.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT11.h>
#include <Servo.h>
#include <Wire.h> 



#define Garage_BT1 11           //Garage Open BT
#define Garage_BT2 2            //Garage Closing BT


#define R1 4
#define G1 5
#define B1 6
#define Step 8                  //Step Pin
#define Dir 7                   //Dir Pin 
#define Alarm 9                 //Buzzer
#define garage 10               //Servo
#define IR 1                    //Gate Motor Inner Sensor
#define IR1 3                   //Alarm Sensor
#define Fan 12                  //Fan Relay
#define BT1 13                  //Extra BT
#define R2 A0
#define G2 A1
#define B2 A2



Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire);   //Initiate I2C Communication via SCL & SDA Ports

        //Start Temp Sensor

bool systemArmed = false;

const int Pressed = HIGH; 

Servo Garage;          //connect Servo to 10

int pos = 0; 



const char* ssid = "Your network name";                      //WiFi SSID 
const char* password = "Your wifi password";           //WiFi Password    

WiFiServer server(80); // Create server on port 80



void setup() {

  Serial.begin(9600);                           // Start serial communication

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);    //Turn Display Voltage To 3.3V     



    display.display();                          //Startup Screen
  
    delay(1000);
    display.clearDisplay();                     //Clear Display




  
  pinMode(Garage_BT1, INPUT_PULLUP);            // Line 90-110 is Self Explanitory
  pinMode(Garage_BT2, INPUT_PULLUP);
  Garage.attach(garage);


  pinMode(R1, OUTPUT);
  pinMode(G1, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(G2, OUTPUT);
  pinMode(B2, OUTPUT);
  
  pinMode(IR, INPUT_PULLUP);

  pinMode(Step, OUTPUT);
  pinMode(Dir, OUTPUT);
  pinMode(Alarm, OUTPUT);
  pinMode(garage, OUTPUT);
  
  pinMode(Fan, OUTPUT);
  pinMode(BT1, INPUT_PULLUP);



  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);                  //Connect to WiFi
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  WifiStatus();            //Display IP Adress in Serial Monitor
  
  // Start the server
  server.begin();                              //Begin Web Server
}

void loop() {

  display.setTextSize(2);                //Text Size
  display.setTextColor(WHITE, BLACK);    //Display In white text
  display.setCursor(1, 1);             //Coordinats
  display.print("Home Auto V7");
  display.display();

  Exit();         //Open Gate from inside

  Enter();        //Open Gate from Outside

  garageOpen();   //Open Garage Door

  garageClose();

  WebControl();   //Handle WiFi Controll

 



}



void Enter(){
    if(digitalRead(BT1) == LOW){
    digitalWrite(Dir,HIGH);
    Serial.println("Enter");
    for(int x = 0; x < 200; x++) {
      digitalWrite(Step,HIGH);
      delayMicroseconds(500);
      digitalWrite(Step,LOW);
      delayMicroseconds(500);
    }
    delay(1000);
    digitalWrite(Dir,LOW);
    for(int x = 0; x < 200; x++) {
      digitalWrite(Step,HIGH);
      delayMicroseconds(500);
      digitalWrite(Step,LOW);
      delayMicroseconds(500);
    }
  }
}



void WebControl(){



  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) {
    Serial.println("New client connected");
    String request = "";
    while (client.connected() && !client.available()) {
      delay(1);
    }
    
    while (client.available()) {
      char c = client.read();
      request += c;
    }

  if (request.indexOf("/R1/on") != -1) {
      digitalWrite(R1, HIGH);
      Serial.println("R1");
    }
    if (request.indexOf("/R1/off") != -1) {
      digitalWrite(R1, LOW);
    }
    if (request.indexOf("/G1/on") != -1) {
      digitalWrite(G1, HIGH);
      Serial.println("G1");
    }
    if (request.indexOf("/G1/off") != -1) {
      digitalWrite(G1, LOW);
    }
    if (request.indexOf("/B1/on") != -1) {
      digitalWrite(B1, HIGH);
      Serial.println("B1");
    }
    if (request.indexOf("/B1/off") != -1) {
      digitalWrite(B1, LOW);
    }
    if (request.indexOf("/R2/on") != -1) {
      digitalWrite(R2, HIGH);
      Serial.println("R2");
    }
    if (request.indexOf("/R2/off") != -1) {
      digitalWrite(R2, LOW);
    }
    if (request.indexOf("/G2/on") != -1) {
      digitalWrite(G2, HIGH);
      Serial.println("G2");
    }
    if (request.indexOf("/G2/off") != -1) {
      digitalWrite(G2, LOW);
    }
    if (request.indexOf("/B2/on") != -1) {
      digitalWrite(B2, HIGH);
      Serial.println("B2");
    }
    if (request.indexOf("/B2/off") != -1) {
      digitalWrite(B2, LOW);
    }
   if (request.indexOf("/Fan/off") != -1) {
      digitalWrite(Fan, LOW);
   }
    if (request.indexOf("/Fan/on") != -1) {
      digitalWrite(Fan, HIGH);
      Serial.println("Fan");
    }
    if (request.indexOf("/Arm/on") != -1) {
           systemArmed = true;
  

  if (systemArmed && (digitalRead(IR) == LOW)) { 
     
      alarm();}
    }
    if (request.indexOf("/Panic/on") != -1) {
       alarm();
    }
   if (request.indexOf("/Disarm/on") != -1) {
    digitalWrite(Alarm, LOW);
       disarm();
       
    }
    



    client.println("HTTP/1.1 200 OK");
    client.println("Content-type: text/html");
    client.println();
    client.println("<html><body>");
    client.println("<h1>DW Huis</h1>");
    

    client.println("<br><br>");
    client.println("<p><a href=\"/R1/on\">Turn R1 ON</a></p>");
    client.println("<p><a href=\"/R1/off\">Turn R1 OFF</a></p>");
    client.println("<p><a href=\"/G1/on\">Turn G1 ON</a></p>");
    client.println("<p><a href=\"/G1/off\">Turn G1 OFF</a></p>");
    client.println("<p><a href=\"/B1/on\">Turn B1 ON</a></p>");
    client.println("<p><a href=\"/B1/off\">Turn B1 OFF</a></p>");
    client.println("<p><a href=\"/R2/on\">Turn R2 ON</a></p>");
    client.println("<p><a href=\"/R2/off\">Turn R2 OFF</a></p>");
    client.println("<p><a href=\"/G2/on\">Turn G2 ON</a></p>");
    client.println("<p><a href=\"/G2/off\">Turn G2 OFF</a></p>");
    client.println("<p><a href=\"/B2/on\">Turn B2 ON</a></p>");
    client.println("<p><a href=\"/B2/off\">Turn B2 OFF</a></p>");
    client.println("<p><a href=\"/Fan/on\">Turn Fan ON</a></p>");
    client.println("<p><a href=\"/Fan/off\">Turn Fan OFF</a></p>");
    client.println("<p><a href=\"/Arm/on\">Arm System</a></p>");
    client.println("<p><a href=\"/Panic/on\">Panic Mode</a></p>");
    client.println("<p><a href=\"/Disarm/on\">Deactivate Alarm</a></p>");
    client.println("</body></html>");

        delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
}


void garageOpen(){
    if(digitalRead(Garage_BT2) == LOW){

    Serial.println("Opening...");
  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    Garage.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
  }
  }
}


void garageClose(){
    if(digitalRead(Garage_BT1) == LOW){
      Serial.println("Closing...");
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    Garage.write(pos);              // tell servo to go to position in variable 'pos'
    delay(5);                       // waits 15 ms for the servo to reach the position
  }
  }
}


void Exit(){
    if(digitalRead(IR) == LOW){
    digitalWrite(Dir,HIGH);
    Serial.println("Exit");
    delay(500);
    for(int x = 0; x < 200; x++) {
      digitalWrite(Step,HIGH);
      delayMicroseconds(500);
      digitalWrite(Step,LOW);
      delayMicroseconds(500);
    }
    delay(1000);
    digitalWrite(Dir,LOW);
    for(int x = 0; x < 200; x++) {
      digitalWrite(Step,HIGH);
      delayMicroseconds(500);
      digitalWrite(Step,LOW);
      delayMicroseconds(500);
    }
  }
}


void alarm(){
      
      analogWrite(Alarm, 250);
      digitalWrite(R1,LOW);
      digitalWrite(B2,HIGH);
      delay(200);
      analogWrite(Alarm, LOW);
      digitalWrite(B2,LOW);
      digitalWrite(R1,HIGH);
      delay(200);
            analogWrite(Alarm, 250);
      digitalWrite(R1,LOW);
      digitalWrite(B2,HIGH);
      delay(200);
      analogWrite(Alarm, LOW);
      digitalWrite(B2,LOW);
      digitalWrite(R1,HIGH);
      delay(200);
            analogWrite(Alarm, 250);
      digitalWrite(R1,LOW);
      digitalWrite(B2,HIGH);
      delay(200);
      analogWrite(Alarm, LOW);
      digitalWrite(B2,LOW);
      digitalWrite(R1,HIGH);
      delay(200);
            analogWrite(Alarm, 250);
      digitalWrite(R1,LOW);
      digitalWrite(B2,HIGH);
      delay(200);
      analogWrite(Alarm, LOW);
      digitalWrite(B2,LOW);
      digitalWrite(R1,HIGH);
      delay(200);
    }

void detect(){
   int State = digitalRead(IR1);

  if (State == HIGH && systemArmed){
    alarm();
  }
}

void disarm(){
  digitalWrite(B2,LOW);
  digitalWrite(R1,LOW);
  
}

void WifiStatus() {

  IPAddress ip = WiFi.localIP();
  Serial.print("IP: ");
  Serial.println(ip);
    
      display.setTextColor(WHITE, BLACK);
    
     display.setCursor(1, 1);

  display.print(ip);
  display.display();
  delay(15000);
  display.clearDisplay();



}
 



