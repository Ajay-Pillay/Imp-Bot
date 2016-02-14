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

int MotorLeftA = 24;         //  Propeller Motor A uses pins 24 and 25. H bridges are used for all motors
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
  else if (data == "backwards") { //
    Serial1.write("moving");      // Moves Imp Bot backwards. Sends the text "moving" back to the Android application as confirmation
    move_backwards();             // Calls the move_backwards function
  }
  else if (data == "left") {      //
    Serial1.write("moving");      // Makes Imp Bot turn left. Sends the text "moving" back to the Android application as confirmation
    turn_left();                  // Calls the turn_left function
  }
  else if (data == "right") {     //
    Serial1.write("moving");      // Makes Imp Bot turn right. Sends the text "moving" back to the Android application as confirmation
    turn_right();                 // Calls the turn_right function
  }
  else if (data == "stop") {      //
    Serial1.write("idle");        // Halts the Imp Bot. Sends the text "idle" back to the Android application as confirmation
    stop();                       // Calls the stop function
    }
  else if (data == "containerA") {
    Serial1.write("lowerA");      // Collects water in Container A, 1 metre underwater. Sends the text "lowerA" back to the Android application as confirmation
    lowerA();                     // Calls the lowerA function
    getLocation();                // Gets the current location from the GPS Shield
    Serial1.print("X,");          // X is used to identify as coordinates from Container A, this is noted on the Android application's side
    Serial1.print(Time);          // Sends the timestamp to the Android application
    Serial1.print(",");           // , is used as a separator for different portions of the transmitted string
    Serial1.print(Lat_Global);    // Sends the latitude
    Serial1.print(",");           // , is used as a separator for different portions of the transmitted string
    Serial1.print(Long_Global);   // Sends the longitude
    Serial1.print(",");           // , is used as a separator for different portions of the transmitted string
    Serial1.write("doneA");       // doneA signifies that the whole process is over and allows user to control Android application
  }
  else if (data == "containerB") {
    Serial1.write("lowerB");       // Refer to above, this section is for Container B which collects water from 2 metres underwater
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
  data = ""; // String's data is reset back to nothing so that it doesn't save previous data
}

void move_forward(void) {
  Serial.println("forward");            // Prints this on the Serial monitor for debugging purposes
  digitalWrite(MotorLeftA, HIGH);       // Makes both propellers turn in the same direction 
  digitalWrite(MotorLeftB, LOW);        //
  digitalWrite(MotorRightA, HIGH);      //
  digitalWrite(MotorRightB, LOW);       //
}

void move_backwards(void) {
  Serial.println("backwards");         // Prints this on the Serial monitor for debugging purposes
  digitalWrite(MotorLeftA, LOW);       // Makes both propellers turn in the same direction 
  digitalWrite(MotorLeftB, HIGH);      //
  digitalWrite(MotorRightA, LOW);      //
  digitalWrite(MotorRightB, HIGH);     //
}

void turn_left(void) {
  Serial.println("turn left");         // Prints this on the Serial monitor for debugging purposes
  digitalWrite(MotorLeftA, LOW);       // This motor doesn't turn
  digitalWrite(MotorLeftB, LOW);       //
  digitalWrite(MotorRightA, HIGH);     // Only the RIGHT motor (Motor B) turns so that the vessel turns LEFT
  digitalWrite(MotorRightB, LOW);      //
}

void turn_right(void) {
  Serial.println("turn right");       // Prints this on the Serial monitor for debugging purposes
  digitalWrite(MotorLeftA, HIGH);     // Only the LEFT motor (Motor A) turns so that the vessel turns RIGHT
  digitalWrite(MotorLeftB, LOW);      //
  digitalWrite(MotorRightA, LOW);     // This motor doesn't turn
  digitalWrite(MotorRightB, LOW);     //
}

void stop(void) {
  Serial.println("halt");             // Prints this on the Serial monitor for debugging purposes
  digitalWrite(MotorLeftA, LOW);      //
  digitalWrite(MotorLeftB, LOW);      // Sets both propeller motors to turn OFF
  digitalWrite(MotorRightA, LOW);     //
  digitalWrite(MotorRightB, LOW);     //
}

void lowerA(void){
  Serial.println("lower A");
  digitalWrite(ContainerASpool_A, LOW);   // Makes the container spool motor uncoil (lower) the container
  digitalWrite(ContainerASpool_B, HIGH);  //
  delay(3500);                            // Time to lower container, manually calibrated
  digitalWrite(ContainerASpool_A, LOW);   // Turns off the container spool motor so that the container remains at 1 metre
  digitalWrite(ContainerASpool_B, LOW);   //
  delay(8000);
  int x = 0;
  while (x < 4) {                                                               //
    for (ContainerA_pos = 0; ContainerA_pos <= 180; ContainerA_pos += 1) {      // Rotates the bicycle brake servo 180 degrees in one direction
      ContainerA.write(ContainerA_pos);                                         //
      delay(15);                                                                //
    }                                                                           //
    for (ContainerA_pos = 180; ContainerA_pos >= 0; ContainerA_pos -= 1) {      // Rotates the bicycle brake servo 180 degrees in the opposite direction
      ContainerA.write(ContainerA_pos);                                         //
      delay(15);                                                                //
    }                                                                           //
    x += 1;                                                                     // This motion continues for 4 times to make sure the container is shut
  }                                                                             //
  digitalWrite(ContainerASpool_A, HIGH);                                        // Spool motor turns in the opposite direction from above
  digitalWrite(ContainerASpool_B, LOW);                                         //
  delay(4300);                                                                  // Time to raise container, manually calibrated. Different from above as container is heavier
  digitalWrite(ContainerASpool_A, LOW);                                         // Stops the motor to keep the container in place
  digitalWrite(ContainerASpool_B, LOW);                                         // 
  delay(500);
}

void lowerB(void){
  Serial.println("lower B");             // Refer to above, this code is for Container B 
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

void getLocation(void) {               // This function gets the coordinates of the current location from the GPS shield
  int y = 0;
  while(1) {
    while(Serial2.available()) {
      int x = Serial2.read();
      if(gps.encode(x)) { // Only enters this loop upon successfuly activation of the GPS
        getgps(gps);
        y = 1;
        break;
      }
    }
    if (y == 1) break;
  }
  delay(100);
}

void getgps(TinyGPS &gps) {                       // Makes use of the gps object made from the TinyGPS class

  // Define the variables that will be used
  float latitude, longitude;
  // Then call this function
  gps.f_get_position(&latitude, &longitude); // Longitude and latitude saved
  
  int year;
  byte month, day, hour, minute, second, hundredths;
  gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);
  String h_string, m_string, s_string; // Initializes string variables to save the current timestamp
  hour = hour + 8;                     // Adds 8 hours to the obtained UTC time as our timezone is +8 UTC, in Singapore
  if (hour < 10) {                     // The following chunk of code deals with how to display the time and make sure it's correct, extensive handling of various numbers that may be fed
    h_string = "0" + String(hour);
  }
  else if (hour > 23) {
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
  Lat_Global = latitude*10000; // Multiplies the longitude and latitude by 10000 (divided by 10000 on the Android application's side) so that the entire GPS coordinate is transmitted
  Long_Global = longitude*10000;
  Time = h_string + ":" + m_string + ":" + s_string; // Creates the timestamp string
  Serial.println(Time);                              // Prints the timestamp on the Serial monitor for debugging purposes
}
