/*
 * Imp Bot Source Code
 * By: Ajay Pillay (DEEE), Tan Hwee Peng (DEEE), Thomas Wong (DME), Ryanfranco Gonzaga (DME)
 * 
 * For the module ET1050: Engineering and Design
 * Under the Engineering Academy programme
 * Singapore Polytechnic
 *
 * For more information on electronics and Imp Bot: http://endteamsix.wordpress.com
 *
 * Connected to an Arduino Mega 2560 board.
 * 
 */
 
#include <Servo.h>           // Servo motor control library
#include <TinyGPS.h>         // TinyGPS library to communicate with GPS Shield

int MotorLeftA = 24;         //  Propeller Motor A uses pins 24 and 25
int MotorLeftB = 25;         //
int MotorRightA = 27;        //  Propeller Motor B uses pins 26 and 27 
int MotorRightB = 26;        //

int ContainerASpool_A = 50;  // Container A is controlled by a continuous servo motor, using pins 50 and 51
int ContainerASpool_B = 51;  //
int ContainerBSpool_A = 52;  // Container B is controlled by a continuous servo motor, using pins 52 and 53
int ContainerBSpool_B = 53;  //

Servo ContainerA;       // Creating an object of the Servo class for the bicycle brake controlling Container A
int ContainerA_pos;     // This integer dictates the servo's angle
Servo ContainerB;       // Creating an object of the Servo class for the bicycle brake controlling Container B
int ContainerB_pos;     // This integer dictates the servo's angle

TinyGPS gps;            // Creates an object of the TinyGPS class

String data;            // This string is used to read data that the Bluetooth module received from the Android application

String Time;            // This is a global variable to store the timestamp of water collection
float Lat_Global;       // Global variable to store the latitude
float Long_Global;      // Global variable to store the longitude

void setup() {
  Serial.begin(115200); // Begins serial monitor. Has to be this fast to pick up readings from the GPS
  Serial2.begin(4800); // GPS pin 2 (from GPS shield) to 17, 3 (from GPS shield) to 16. Hardware serial communication
  Serial1.begin(9600); // Bluetooth Rx to 18, Tx to 19. Hardware serial communication
  pinMode(MotorLeftA, OUTPUT);             //
  pinMode(MotorLeftB, OUTPUT);             //
  pinMode(MotorRightA, OUTPUT);            //
  pinMode(MotorRightB, OUTPUT);            // Setting all motor pins as digital output pins
  pinMode(ContainerASpool_A, OUTPUT);      //
  pinMode(ContainerASpool_B, OUTPUT);      //
  pinMode(ContainerBSpool_A, OUTPUT);      //
  pinMode(ContainerBSpool_B, OUTPUT);      //
  
  ContainerA.attach(23);   // Container A's bicycle brake servo's singal wire to pin 23
  ContainerA.write(0);     // Set the position to 0 degrees
  ContainerB.attach(22);   // Container B's bicycle brake servo's signal wire to pin 22
  ContainerB.write(0);     // Set the position to 0 degrees
}
void loop() {
  while (Serial1.available()) { // If the Bluetooth serial is available
    delay(3);                   //
    char c = Serial1.read();    // Reads the data in the data stream character by character
    data += c;                  // Then appends it to the "data" variable
  }
  if (data == "forward") {    //  
    Serial1.write("moving");  // Moves Imp Bot forward. Sends the text "moving" back to the Android application as confirmation
    move_forward();           // Calls the move_forward function
  }
  else if (data == "backwards") {
    Serial1.write("moving");
    move_backwards();
  }
  else if (data == "left") {
    Serial1.write("moving");
    turn_left();  
  }
  else if (data == "right") {
    Serial1.write("moving");
    turn_right();
  }
  else if (data == "stop") {
    Serial1.write("idle");
    stop();
    }
  else if (data == "containerA") {
    Serial1.write("lowerA");
    lowerA();
    getLocation();
    Serial1.print("X,");
    Serial1.print(Time);
    Serial1.print(",");
    Serial1.print(Lat_Global);
    Serial1.print(",");
    Serial1.print(Long_Global);
    Serial1.print(",");
    Serial1.write("doneA");
  }
  else if (data == "containerB") {
    Serial1.write("lowerB");
    lowerB();
    getLocation();
    Serial1.print("Y,");
    Serial1.print(Time);
    Serial1.print(",");
    Serial1.print(Lat_Global);
    Serial1.print(",");
    Serial1.print(Long_Global);
    Serial1.print(",");
    Serial1.write("doneB");
  }
  data = "";
}

void move_forward(void){
  Serial.println("forward");
  digitalWrite(MotorLeftA, HIGH);
  digitalWrite(MotorLeftB, LOW);
  digitalWrite(MotorRightA, HIGH);
  digitalWrite(MotorRightB, LOW);
}

void move_backwards(void){
  Serial.println("backwards");
  digitalWrite(MotorLeftA, LOW);
  digitalWrite(MotorLeftB, HIGH);
  digitalWrite(MotorRightA, LOW);
  digitalWrite(MotorRightB, HIGH);
}

void turn_left(void){
  Serial.println("turn left");
  digitalWrite(MotorLeftA, LOW);
  digitalWrite(MotorLeftB, LOW);
  digitalWrite(MotorRightA, HIGH);
  digitalWrite(MotorRightB, LOW);
}

void turn_right(void){
  Serial.println("turn right");
  digitalWrite(MotorLeftA, HIGH);
  digitalWrite(MotorLeftB, LOW);
  digitalWrite(MotorRightA, LOW);
  digitalWrite(MotorRightB, LOW);
}

void stop(void){
  Serial.println("halt");
  digitalWrite(MotorLeftA, LOW);
  digitalWrite(MotorLeftB, LOW);
  digitalWrite(MotorRightA, LOW);
  digitalWrite(MotorRightB, LOW);
}

void lowerA(void){
  Serial.println("lower A");
  digitalWrite(ContainerASpool_A, LOW);
  digitalWrite(ContainerASpool_B, HIGH);
  delay(3500); //Time to lower container
  digitalWrite(ContainerASpool_A, LOW);
  digitalWrite(ContainerASpool_B, LOW);
  delay(8000);
  int x = 0;
  while (x < 4) {
    for (ContainerA_pos = 0; ContainerA_pos <= 180; ContainerA_pos += 1){
      ContainerA.write(ContainerA_pos);
      delay(15);
    }
    for (ContainerA_pos = 180; ContainerA_pos >= 0; ContainerA_pos -= 1){
      ContainerA.write(ContainerA_pos);
      delay(15);
    }
    x += 1;
  }
  digitalWrite(ContainerASpool_A, HIGH);
  digitalWrite(ContainerASpool_B, LOW);
  delay(4300); //Time to raise container
  digitalWrite(ContainerASpool_A, LOW);
  digitalWrite(ContainerASpool_B, LOW);
  delay(500);
}

void lowerB(void){
  Serial.println("lower B");
  digitalWrite(ContainerBSpool_A, LOW);
  digitalWrite(ContainerBSpool_B, HIGH);
  delay(5000); //Time to lower container
  digitalWrite(ContainerBSpool_A, LOW);
  digitalWrite(ContainerBSpool_B, LOW);
  delay(10000);
  int x = 0;
  while (x < 4) {
    for (ContainerB_pos = 0; ContainerB_pos <= 180; ContainerB_pos += 1){
      ContainerB.write(ContainerB_pos);
      delay(15);
    }
    for (ContainerB_pos = 180; ContainerB_pos >= 0; ContainerB_pos -= 1){
      ContainerB.write(ContainerB_pos);
      delay(15);
    }
    x += 1;
  }
  digitalWrite(ContainerBSpool_A, HIGH);
  digitalWrite(ContainerBSpool_B, LOW);
  delay(5950); //Time to raise container
  digitalWrite(ContainerBSpool_A, LOW);
  digitalWrite(ContainerBSpool_B, LOW);
  delay(400);
}

void getLocation(void){
  int y = 0;
  while(1){
    while(Serial2.available()) {
      int x = Serial2.read();
      if(gps.encode(x)){
        getgps(gps);
        y=1;
        break;
      }
    }
    if (y==1){
      break;
    }
  }
  delay(100);
}

void getgps(TinyGPS &gps)
{
  // To get all of the data into varialbes that you can use in your code, 
  // all you need to do is define variables and query the object for the 
  // data. To see the complete list of functions see keywords.txt file in 
  // the TinyGPS and NewSoftSerial libs.
  
  // Define the variables that will be used
  float latitude, longitude;
  // Then call this function
  gps.f_get_position(&latitude, &longitude);
  // You can now print variables latitude and longitud
  
  // Same goes for date and time
  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
  // Print data and time
  String h_string, m_string, s_string, lat_string = "", long_string = "";
  hour = hour + 8;
  if (hour < 10){
    h_string = "0" + String(hour);
  }
  else if (hour > 23){
    hour = hour - 24;
    h_string = "0" + String(hour);
  }
  else {
    h_string = String(hour);
  }
  if (minute < 10){
    m_string = "0" + String(minute);
  }
  else {
    m_string = String(minute);
  }
  if (second < 10){
    s_string = "0" + String(second);
  }
  else {
    s_string = String(second);
  }
  Lat_Global = latitude*10000;
  Long_Global = longitude*10000;
  Time = h_string + ":" + m_string + ":" + s_string;
  Serial.println(Time);
  //Since month, day, hour, minute, second, and hundr
  
  // Here you can print statistics on the sentences.
  unsigned long chars;
  unsigned short sentences, failed_checksum;
  gps.stats(&chars, &sentences, &failed_checksum);
  //Serial.print("Failed Checksums: ");Serial.print(failed_checksum);
  //Serial.println(); Serial.println();
}
