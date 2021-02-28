/*
 * This project allows to control a robot through an Android application, via WIFI.
 *  
 * developed by : SMOUNI Marwan
 * 
 * 
 * For more informations about how to use the L293D Motor Driver: https://rootsaid.com/h-bridge-and-l293d-motor-driver/
 * 
 * Infrared sensors 'IR' are used in this project, for more information: https://pijaeducation.com/arduino/sensor/ir-sensor-and-arduino/
 * 
 * ESP8266 WIFI module is used in this project, for more informations: https://www.electronicshub.org/wifi-controlled-led-using-esp8266-and-arduino/
 *                                                                     https://www.itead.cc/wiki/images/5/53/Esp8266_at_instruction_set_en_v1.5.4_0.pdf   
 *
 * This project uses the FreeRTOS class, for more informations: https://circuitdigest.com/microcontroller-projects/arduino-freertos-tutorial1-creating-freertos-task-to-blink-led-in-arduino-uno
 */

#include <Arduino_FreeRTOS.h>

#include <SoftwareSerial.h>
#define DEBUG true


//Connect the L293D Motor Driver to the Arduino digitial pins: 8,9,10,11
int Left_Motor_S1 = 10;
int Left_Motor_S2 = 11;
int Right_Motor_S1 = 8;
int Right_Motor_S2 = 9;


//connect front IR sensor to digital pin: 4
//connect Rear IR sensor to digital pin: 5
int IR_front = 4;
int IR_rear = 5;

int obs_IR_front = LOW; // NO OBSTACLE
int obs_IR_rear = LOW; // NO OBSTACLE

// ESP8266 WIFI module
SoftwareSerial esp8266(2,3); // Connect ESP8266's TX port to the Arduino's pin 2, and RX port to the Arduino's pin 3.

boolean alreadyConnected = false;

///////////////////////////////////////////////////////////////////


/*
 * Functions used in this project
 */

// Moving forward
void Move_F(){
  obs_IR_front = digitalRead(IR_front);
  if (obs_IR_front == LOW ) {
    digitalWrite(Left_Motor_S1,HIGH);
    digitalWrite(Left_Motor_S2,LOW);
    digitalWrite(Right_Motor_S1,HIGH);
    digitalWrite(Right_Motor_S2,LOW);
    
    //delay (2000);
    vTaskDelay( 2000 / portTICK_PERIOD_MS );
    
    //the action is executed during 2s, then the motors are stopped.
    digitalWrite(Left_Motor_S1,LOW);
    digitalWrite(Left_Motor_S2,LOW);
    digitalWrite(Right_Motor_S1,LOW);
    digitalWrite(Right_Motor_S2,LOW);
  }
}

// Moving backwards
void Move_B(){
  obs_IR_rear = digitalRead(IR_rear);
  if (obs_IR_rear == LOW ){
    digitalWrite(Left_Motor_S1,LOW); //the opposite logic to the one before
    digitalWrite(Left_Motor_S2,HIGH);
    digitalWrite(Right_Motor_S1,LOW);
    digitalWrite(Right_Motor_S2,HIGH);
    
    //delay (2000);
    vTaskDelay( 2000 / portTICK_PERIOD_MS );

    //the action is executed during 2s, then the motors are stopped.
    digitalWrite(Left_Motor_S1,LOW);
    digitalWrite(Left_Motor_S2,LOW);
    digitalWrite(Right_Motor_S1,LOW);
    digitalWrite(Right_Motor_S2,LOW);
  }
}


// Turn left
void Move_L(){
  obs_IR_front = digitalRead(IR_front);
  if (obs_IR_front == LOW ){
    digitalWrite(Left_Motor_S1,LOW); //stop the Left motor, and run the Right motor, which allows the robot to turn to the left
    digitalWrite(Left_Motor_S2,LOW); 
    digitalWrite(Right_Motor_S1,HIGH);
    digitalWrite(Right_Motor_S2,LOW);
    
    //delay (1000);
    vTaskDelay( 1000 / portTICK_PERIOD_MS );

    //the action is executed during 1s, then the motors are stopped.
    digitalWrite(Left_Motor_S1,LOW);
    digitalWrite(Left_Motor_S2,LOW);
    digitalWrite(Right_Motor_S1,LOW);
    digitalWrite(Right_Motor_S2,LOW);
  }

}


// Turn right
void Move_R(){
  obs_IR_front = digitalRead(IR_front);
  if (obs_IR_front == LOW ){
    digitalWrite(Left_Motor_S1,HIGH); //stop the Right motor, and run the Left motor, which allows the robot to turn to the right
    digitalWrite(Left_Motor_S2,LOW);
    digitalWrite(Right_Motor_S1,LOW);
    digitalWrite(Right_Motor_S2,LOW);
    
    //delay (1000);
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
    
    //the action is executed during 1s, then the motors are stopped.
    digitalWrite(Left_Motor_S1,LOW);
    digitalWrite(Left_Motor_S2,LOW);
    digitalWrite(Right_Motor_S1,LOW);
    digitalWrite(Right_Motor_S2,LOW);
  }
}


void Stopp(){
  digitalWrite(Left_Motor_S1,LOW);
  digitalWrite(Left_Motor_S2,LOW);
  digitalWrite(Right_Motor_S1,LOW);
  digitalWrite(Right_Motor_S2,LOW);
}

// obstacle avoidance in front of the robot
void avoid_front(){
  digitalWrite(Left_Motor_S1,LOW);
  digitalWrite(Left_Motor_S2,HIGH);
  digitalWrite(Right_Motor_S1,LOW);
  digitalWrite(Right_Motor_S2,LOW);

  vTaskDelay( 1000 / portTICK_PERIOD_MS );

  //the action is executed during 1s, then the motors are stopped.
  digitalWrite(Left_Motor_S1,LOW);
  digitalWrite(Left_Motor_S2,LOW);
  digitalWrite(Right_Motor_S1,LOW);
  digitalWrite(Right_Motor_S2,LOW);
}

// obstacle avoidance behind the robot
void avoid_rear(){
  digitalWrite(Left_Motor_S1,LOW);
  digitalWrite(Left_Motor_S2,LOW);
  digitalWrite(Right_Motor_S1,HIGH);
  digitalWrite(Right_Motor_S2,LOW);

  vTaskDelay( 1000 / portTICK_PERIOD_MS );

  //the action is executed during 1s, then the motors are stopped.
  digitalWrite(Left_Motor_S1,LOW);
  digitalWrite(Left_Motor_S2,LOW);
  digitalWrite(Right_Motor_S1,LOW);
  digitalWrite(Right_Motor_S2,LOW);
}


// Function used to send data to ESP8266.
String send_to_ESP8266(String command, const int timeout, boolean debug){
//                     command to send;  the time to wait for a response;
  String response = "";
  
  esp8266.print(command); // send the read character to the esp8266
  
  long int time = millis();
  
  while( (time+timeout) > millis()){
    while(esp8266.available()){
      // The esp has data so display its output to the serial window 
      char c = esp8266.read(); // read the next character.
      response+=c;
    }
  }
  
  if(debug){
    Serial.print(response);
  }
  
  return response;
}

///////////////////////////////////////////////////////////////////

/*
 * Task declaration
 */
// Motors control
void Task_control( void *pvParameters );

//obstacle avoidance task
void Task_avoid( void *pvParameters );
///////////////////////////////////////////////////////////////////


void setup()
{
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  esp8266.begin(9600); //Baud rate for communicating with ESP8266. Your's might be different.

  //Setting the L293D Motor Driver ports as OUTPUTS
  pinMode(Left_Motor_S1,OUTPUT);
  pinMode(Left_Motor_S2,OUTPUT);
  pinMode(Right_Motor_S1,OUTPUT);
  pinMode(Right_Motor_S2,OUTPUT);

  //Setting IR sensors  
  pinMode(IR_front,INPUT);
  pinMode(IR_rear,INPUT);

  //Setting ESP8266 WIFI Module
  send_to_ESP8266("AT+RST\r\n",2000,DEBUG); // Reset the ESP8266
  send_to_ESP8266("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point
  send_to_ESP8266("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  send_to_ESP8266("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  send_to_ESP8266("AT+CIPSERVER=1,77\r\n",1000,DEBUG); // turn on server on port 77
  
  delay(1000);

  // Tasks
  xTaskCreate(Task_control, "Control", 128, NULL, 1,  NULL);
  xTaskCreate(Task_avoid, "obstacle_avoidance", 128, NULL, 2,  NULL); //higher priority task

  vTaskStartScheduler();
  
}


void loop()
{

  // Empty. Things are done in Tasks.
   
}


/*
 * definition of the control task
 */
void Task_control( void *pvParameters ) {
  while (1){
    if(esp8266.available()) // check if the esp is sending a message
    obs_IR_front= digitalRead(IR_front);
    obs_IR_rear= digitalRead(IR_rear);
    {
      if (!alreadyConnected){
        // clear out the input buffer:
        esp8266.flush();
        alreadyConnected = true;
      }
  
      //Serial.find() reads data from the serial buffer until the target string of given length is found
      if(esp8266.find("+IPD,")){
        vTaskDelay( 10 / portTICK_PERIOD_MS );//delay(10); // wait for the serial buffer to fill up (read all the serial data)
  
        esp8266.find("cmd="); // advance cursor to "cmd="
        
        //Returns: the character read, or -1 if none is available 
        char rcv_CMD = esp8266.read();
  
        //Depending on the command sent by the Android application, the corresponding action is executed
        switch(rcv_CMD){
          case 'F':
            Move_F();
            break;
          case 'B':
            Move_B();
            break;
          case 'L':
            Move_L();
            break;
          case 'R':
            Move_R();
            break;
          case 'S':
            Stopp();
            break;
          default:
            break;
        }
      
      }
  
    }
  
    // Instead of delay(100);
    vTaskDelay( 100/ portTICK_PERIOD_MS ); 
  
  }
}


/*
 * definition of the obstacle avoidance task
 */
void Task_avoid( void *pvParameters ){
  while (1){
    if (obs_IR_front == HIGH ) {
      avoid_front();
    }
    else if (obs_IR_rear == HIGH ) {
      avoid_rear();
    }
    else {
      vTaskDelay( 300 / portTICK_PERIOD_MS );
    }
  }
}
