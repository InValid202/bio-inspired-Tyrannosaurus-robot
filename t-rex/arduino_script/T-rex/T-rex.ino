#include <SCServo.h>
#include <ros.h>
#include <std_msgs/Int16MultiArray.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Float32.h>

#define NUM_SERVOS 9

unsigned long last_time = 0;
unsigned long last_time_food = 0;
 
const long INTERVAL_move= 2; // Interval for publishing load data in milliseconds
const long INTERVAL_life = 200; 

int16_t distance_from_pray = 1000;
float pray = 0;
float pray_target = 0;

int16_t food_count = 0;
int16_t motor_speed = 1500;

int16_t state = 1;

float satiety_Hormone = 0.0;
float sleep_Hormone = 0.0;
float death_Hormone = 0.0;

float death_bar = 0;
float sleep_bar = 0;

int16_t camera_array[8]; 

// 1 = normal walking mode
// 2 = hunting mode
// 3 = sleep mode
// 4 = death mode
// 5 = eating mode
// 6 = wake up mode

const int to_low_motor2 = 0; // right front leg 
const int to_high_motor2 = 100; 

const int to_low_motor3 = 0; // right back leg 
const int to_high_motor3 = 180;

const int to_low_motor6 = 0; // left front leg 
const int to_high_motor6 = 100;

const int to_low_motor7 = 0; // left back leg 
const int to_high_motor7 = 180;

const int to_low_motor5 = 220; // left_head
const int to_high_motor5 = 350;

const int to_low_motor1 = 220; // right_head
const int to_high_motor1 = 350;

const int to_low_motor8 = 200; // left_tail 
const int to_high_motor8 = 270;

const int to_low_motor4 = 200; // right_tail
const int to_high_motor4 = 270;

int16_t input1 = 1;
int16_t input2 = 1;

// right
int motor1 = 0; 
int motor2 = 0; // front right
int motor3 = 0; // back right
int motor4 = 0;
int motor5 = 0;

// int motor6 = 0; // not found 

// left
int motor7 = 0; // back left
int motor8 = 0;
int motor6 = 0; // front left
int motor10 = 0;
int motor11 = 0;
int motor12 = 0;

float alpha = 1.03;
float phi = M_PI/10;
float MI = 0.05;
      
float B_CPG = 0;
            
float w00 = alpha * cos(phi);
float w01 = alpha * sin(phi+MI);
float w10 = alpha * (-sin(phi+MI));
float w11 = alpha * cos(phi);
            
float c0 = 0.5;
float c1 = 0.5;

float o0 = tanh(c0);
float o1 = tanh(c1);
        
// left VRN's variable
       
float w02 = 1.75;
float wi3 = 5;
float w24 = 0.5; // main A parameter
float w34 = w24;
float w25 = -0.5; // main c parameter
float w35 = w25;
float w26 = w24;
float w36 = w25;
float w27 = w25;
float w37 = w24;
float w48 = 0.5;
float w58 = 0.5;
float w68 = -0.5;
float w78 = -0.5;
   
float B_VRN = 1;
          
float h2 = 0;
float h3 = 0;
float h4 = 0;
float h5 = 0;
float h6 = 0;
float h7 = 0;
float h8 = 0;
      
float oi1 = tanh(input1);
float o2 = tanh(h2);
float o3 = tanh(h3);
float o4 = tanh(h4);
float o5 = tanh(h5);
float o6 = tanh(h6);
float o7 = tanh(h7);
float o8 = tanh(h8);
        
// right VRN's variable
      
float w19 = 1.75;
float wi10 = 5;
float w911 = w24;
float w1011 = w24;
float w912 = w25;
float w1012 = w25;
float w913 = w24;
float w1013 = w25;
float w914 = w25;
float w1014 = w24;
float w1115 = 0.5;
float w1215 = 0.5;
float w1315 = -0.5;
float w1415 = -0.5;     
      
      
float h9 = 0;
float h10 = 0;
float h11 = 0;
float h12 = 0;
float h13 = 0;
float h14 = 0;
float h15 = 0;
       
float oi2 = tanh(input2);
float o9  = tanh(h9);
float o10 = tanh(h10);
float o11 = tanh(h11);
float o12 = tanh(h12);
float o13 = tanh(h13);
float o14 = tanh(h14);
float o15 = tanh(h15);  


SCSCL sc;
ros::NodeHandle nh;


std_msgs::Int16MultiArray load_msg;
//std_msgs::Float32 mi;
//std_msgs::Float32 motor_speed;
// std_msgs::Int16MultiArray position_msg;
// std_msgs::Int16MultiArray speed_msg;

std_msgs::Int16 light_msg;
std_msgs::Float32 food_msg;       
                  
std_msgs::Int16 death_bar_msg;
std_msgs::Int16 sleep_bar_msg;  

// Array to store servo load data

int16_t load_data[NUM_SERVOS];
int16_t speed_data[NUM_SERVOS];
int16_t position_data[NUM_SERVOS];

int sensorPin = A0;
int sensorValue = 0;


// Function prototypes
int mapf(double , double , double , double , double );

void inputCallback(const std_msgs::Int16 &msg);

void camera_callback(const std_msgs::Int16MultiArray &msg);

void satiety_callback(const std_msgs::Float32 &msg);

void sleep_callback(const std_msgs::Float32 &msg);

void death_callback(const std_msgs::Float32 &msg);
 
void get_feedback_pub();


// feedback publisher 
ros::Publisher load_pub("arduino_Load", &load_msg);
ros::Publisher food_pub("food_stimulate", &food_msg); 

// publish status
ros::Publisher death_bar_pub("death_bar", &death_bar_msg); 
ros::Publisher sleep_bar_pub("sleep_bar", &sleep_bar_msg); 

// sensor publisher
ros::Publisher Light("Light", &light_msg);


// subscriber for state and turn right and left
ros::Subscriber<std_msgs::Int16> sub_input("/input", inputCallback); 
ros::Subscriber<std_msgs::Int16MultiArray> sub_camera("/pixy", camera_callback); 
ros::Subscriber<std_msgs::Float32> sub_satiety("/satiety_hormone", satiety_callback); 
ros::Subscriber<std_msgs::Float32> sub_sleep("/sleep_hormone", sleep_callback); 
ros::Subscriber<std_msgs::Float32> sub_death("/death_hormone", death_callback); 

void setup() {
  Serial1.begin(1000000);
  // Serial.begin(9600); // for test
  // Serial.begin(1000000);
  sc.pSerial = &Serial1;
  
  
  nh.initNode();
  nh.getHardware()->setBaud(57600);
  
  nh.subscribe(sub_input);
  nh.subscribe(sub_camera);
  nh.subscribe(sub_satiety);
  nh.subscribe(sub_sleep);
  nh.subscribe(sub_death);

  nh.advertise(load_pub);
  nh.advertise(food_pub);
  
  nh.advertise(death_bar_pub);
  nh.advertise(sleep_bar_pub);
  
  nh.advertise(Light);
}



void loop() {
  
  if( millis() - last_time > INTERVAL_move && (state == 1 || state == 2)) // normal 
  {
     last_time = millis();
     
     if(state == 1)
     {
      MI = 0; 
     }
     else 
     {
      MI = 0.2;
     }

     w00 = alpha * cos(phi);
     w01 = alpha * sin(phi+MI);
     w10 = alpha * (-sin(phi+MI));
     w11 = alpha * cos(phi);

     c0 = w10 * o1 + w00 * o0;
     c1 = w01 * o0 + w11 * o1;
        

      
     // left VRNs equation
          
     h2 = o0 * w02; 
     h3 = oi1 * wi3;
     h4 = o2 * w24 + o3 * w34 + B_VRN; 
     h5 = o2 * w25 + o3 * w35 + B_VRN;
     h6 = o2 * w26 + o3 * w36 + B_VRN;
     h7 = o2 * w27 + o3 * w37 + B_VRN;
     h8 = o4 * w48 + o5 * w58 + o6 * w68 + o7 * w78;
        
     // right VRNs equation
         
     h9 = o1*w19;
     h10 =oi2 * wi10;
     h11 = o9 * w911 + o10 * w1011 + B_VRN;
     h12 = o9 * w912 + o10 * w1012 + B_VRN;
     h13 = o9 * w913 + o10 * w1013 + B_VRN;
     h14 = o9 * w914 + o10 * w1014 + B_VRN;
     h15 = o11 * w1115 + o12 * w1215 + o13 * w1315 + o14 * w1415; 
        
     // CPG output
        
     o0 = tanh(c0);
     o1 = tanh(c1);
      
    
     // left VRNs output
        
     oi1 = tanh(input1);
     o2 = tanh(h2);
     o3 = tanh(h3);
     o4 = tanh(h4);
     o5 = tanh(h5);
     o6 = tanh(h6);
     o7 = tanh(h7);
     o8 = tanh(h8);
        
     // right VRNS output
     
     oi2 = tanh(input2);
     o9  = tanh(h9);
     o10 = tanh(h10);
     o11 = tanh(h11);
     o12 = tanh(h12);
     o13 = tanh(h13);
     o14 = tanh(h14);
     o15 = tanh(h15); 

    if (o0 < -0.3353811311) 
    {
        o0 = -0.3353811311;
    }  
    else if (o0 > 0.3358224348)
    {
      o0 = 0.3358224348;
    }
    
    
    if (o1 < -0.3352863688) 
    {
        o1 = -0.3352863688;
    } 
    else if (o1 > 0.3384303953)
    {
      o1 = 0.3384303953;
    }
    
    
    if (o8 < -0.3031119357) 
    {
        o8 = -0.3031119357;
    }  
    else if (o8 > 0.3033082662)
    {
      o8 = 0.3033082662;
    }
    
    
    if (o15 < -0.3353811311) 
    {
      o15 = -0.3353811311;
    }  
    else if (o15 > 0.3358224348)
    {
      o15 = 0.3358224348;
    }

    // leg 
    motor2 = 1023 - mapf(o15, -0.3031119357, 0.3033082662, to_low_motor2, to_high_motor2);// right front 
    // motor2 = 1023 - mapf(o1, -0.3352863688, 0.3384303953, to_low_motor2, to_high_motor2);
    motor3 = 1023 - mapf(o1, -0.3352863688, 0.3384303953, to_low_motor3, to_high_motor3);// right back

    motor6 = mapf(o8, -0.3031119357, 0.3033082662, to_low_motor6, to_high_motor6); // left front
    // motor6 = mapf(o0, -0.3353811311, 0.3358224348, to_low_motor6, to_high_motor6); // left front
    motor7 = mapf(o0, -0.3353811311, 0.3358224348, to_low_motor7, to_high_motor7); // left back 

    // head
    motor5 = 1023 - mapf(o8, -0.3031119357, 0.3033082662, to_low_motor5, to_high_motor5); // left head 
    motor1 = mapf(-o8, -0.3031119357, 0.3033082662, to_low_motor1, to_high_motor1); // right head 

    // tail
    motor8 = mapf(o8, -0.3031119357, 0.3033082662, to_low_motor8, to_high_motor8); // left tail
    motor4 = 1023 - mapf(-o8, -0.3031119357, 0.3033082662, to_low_motor4, to_high_motor4); // right tail
    
    
    // right leg
    sc.WritePos(2, motor2, 0, motor_speed);
    sc.WritePos(11, motor3, 0, motor_speed);


    // left leg
    sc.WritePos(6, motor6, 0, motor_speed);
    sc.WritePos(7, motor7, 0, motor_speed);

    // head
    sc.WritePos(5, motor5, 0, motor_speed); // left
    sc.WritePos(1, motor1, 0, motor_speed); // right

    // tail
    sc.WritePos(8, motor8, 0, motor_speed); // left
    sc.WritePos(4, motor4, 0, motor_speed); // right
    
  
    sc.RegWriteAction();
  }


  if(state == 3) // eat 
  {
      sc.RegWritePos(2, 933, 0, 1000);
      sc.RegWritePos(11, 793, 0, 1000);
      sc.RegWritePos(9, 90, 0, 1000);
      sc.RegWritePos(7, 230, 0, 1000);
      sc.RegWritePos(5, 1023, 0, 1000);
      sc.RegWritePos(4 ,1023, 0, 1000);
      sc.RegWritePos(1, 0, 0, 1000); 
      sc.RegWritePos(8, 0, 0, 1000); 
      sc.RegWriteAction();
      delay(500);
      sc.RegWritePos(1, 423, 0, 600);
      sc.RegWritePos(5, 600, 0, 600);
      sc.RegWriteAction();
      delay(200);
      sc.RegWritePos(12, 0, 0, 800);
      sc.RegWriteAction();
      delay(500);
      sc.RegWritePos(12, 150, 0, 800);
      sc.RegWritePos(1, 823, 0, 600);
      sc.RegWritePos(5, 800, 0, 600);
      sc.RegWriteAction();
      delay(500);
      sc.RegWritePos(12, 0, 0, 800);
      sc.RegWritePos(1, 723, 0, 600);
      sc.RegWritePos(5, 800, 0, 600);
      sc.RegWriteAction();
  
      sc.RegWritePos(12, 150, 0, 800);
      sc.RegWritePos(1, 500, 0, 600);
      sc.RegWritePos(5, 200, 0, 600);
      sc.RegWriteAction();
      delay(500);
      sc.RegWritePos(12, 0, 0, 800);
      sc.RegWritePos(1, 0, 0, 600);
      sc.RegWritePos(5, 500, 0, 600);
      sc.RegWriteAction();
      delay(500);
      sc.RegWritePos(12, 0, 0, 800);
      sc.RegWritePos(1, 0, 0, 600);
      sc.RegWritePos(5, 1023, 0, 600);
      sc.RegWriteAction();
      
      state = 1;
  }

  if(state == 4) // sleep 
  {
    sc.RegWritePos(2, 933, 0, 1000);
    sc.RegWritePos(11, 793, 0, 1000);
    sc.RegWritePos(6, 90, 0, 1000);
    sc.RegWritePos(7, 230, 0, 1000);
    sc.RegWritePos(5, 623, 0, 1000);
    sc.RegWritePos(4 ,723, 0, 1000);
    sc.RegWritePos(1, 400, 0, 1000); 
    sc.RegWritePos(8 ,300, 0,1000); 
    sc.RegWriteAction();

    if (sleep_bar <= 0)
    {
      state = 6;
    }

  }

  if(state == 5) // die
  {
   
    for(int i; i<=500;i++)
    {
      sc.RegWritePos(1, 0, 0, 1000);
      sc.RegWritePos(4, 923, 0, 1000);
      sc.RegWritePos(5, 1023, 0, 1000);
      sc.RegWritePos(8, 100, 0, 1000);
      sc.RegWriteAction();
      delay(1);
      sc.RegWritePos(6, 165, 0, 1000);
      sc.RegWritePos(7, 200, 0, 1000);
      sc.RegWriteAction();
      sc.RegWritePos(2, 958, 0, 1000);
      sc.RegWritePos(11, 823, 0, 1000);
      sc.RegWritePos(6, 123, 0, 1000);
      sc.RegWritePos(7, 323, 0, 1000);
      sc.RegWriteAction();
      sc.RegWritePos(1, 0, 0, 1000);
      sc.RegWritePos(4, 1023, 0, 1000); 
      sc.RegWritePos(5, 1023, 0, 1000); 
      sc.RegWritePos(8, 0, 0, 1000);
      sc.RegWriteAction();
      delay(1);
      sc.RegWritePos(2, 800, 0, 1000);
      sc.RegWritePos(11, 1023, 0, 1000);
      sc.RegWriteAction();
      sc.RegWritePos(2, 900, 0, 1000);
      sc.RegWritePos(11, 700, 0, 1000);
      sc.RegWritePos(6, 65, 0, 1000);
      sc.RegWritePos(7, 200, 0, 1000);
      sc.RegWriteAction();
    }
   
    sc.RegWritePos(6, 300, 0, 50);
    sc.RegWritePos(7, 200, 0, 1000);
    sc.RegWritePos(12, 120, 0, 50);
    sc.RegWriteAction();
    
  }
  if (state == 6) // wake up 
  {
    sc.RegWritePos(2, 933, 0, 1000);
    sc.RegWritePos(11, 793, 0, 1000);
    sc.RegWritePos(6, 90, 0, 1000);
    sc.RegWritePos(7, 230, 0, 1000);
    sc.RegWritePos(5, 1023, 0, 1000);
    sc.RegWritePos(4 ,1023, 0, 1000);
    sc.RegWritePos(1, 0, 0, 1000); 
    sc.RegWritePos(8, 0, 0, 1000); 
    sc.RegWriteAction();
    
    state = 1;
  }

  
  if( millis() - last_time_food > INTERVAL_life) 
  {
    last_time_food = millis();
    sensorValue = analogRead(sensorPin); // get light sensor data
    
    if (food_count >= 1)
    {
      food_msg.data = pray_target;
      food_count = food_count - 1;
    }
    else
    {
      food_msg.data = 0;
    }
    food_pub.publish(&food_msg);

    
    sleep_bar = sleep_bar + mapf(sleep_Hormone, 0, 1, 0, 9) - 2;
    
    
    if (sleep_bar  < 0)  
    {
      sleep_bar  = 0;
    } 
    else if (sleep_bar  >= 100)
    {
      sleep_bar = 100;
      state = 4;
    }
    
    sleep_bar_msg.data = sleep_bar;
    sleep_bar_pub.publish(&sleep_bar_msg);

    // death_bar = death_bar + mapf(death_Hormone, 0, 1, 0, 4) - mapf(1 - sleep_Hormone, 0, 1, 0, 2);
    death_bar = death_bar + mapf(death_Hormone, 0, 1, 0, 4) - mapf(100 - sleep_bar, 0, 100, 0, 3);
    if (death_bar < 0)  
    {
      death_bar = 0;
    }
    else if (death_bar >= 100)
    {
      death_bar = 100; 
      state = 5;
    }

    death_bar_msg.data = death_bar;    
    death_bar_pub.publish(&death_bar_msg);
    
    light_msg.data =  sensorValue; 
    Light.publish( &light_msg ); // send light data to rostopic
    get_feedback_pub(); 
  }

    
  delay(1);
  nh.spinOnce();
   
}


void inputCallback(const std_msgs::Int16 &msg)
{
  // direction 1 --> forward 
  // direction 2 --> left
  // direction 3 --> right
  
  if (msg.data == 1)
  {
    input1 = 1;
    input2 = 1;
  }
  else if (msg.data == 2)
  {
    input1 = 0.1;
    input2 = 1;
  }
  else
  {
    input1 = 1;
    input2 = 0.1;
  }
  
}

void camera_callback(const std_msgs::Int16MultiArray &msg)
{
    for (int i = 0; i < 8 ;++i) {
      camera_array[i] = msg.data[i];
    }
    
    if (camera_array[4] == 3)
    {
      distance_from_pray = camera_array[5];
      pray = 1.8;
    }
        
    else if (camera_array[2] == 2)
    {
      distance_from_pray = camera_array[3];
      pray = 0.9;
    }
        
    else if (camera_array[0] == 1)
    {
      distance_from_pray = camera_array[1];
      pray = 0.45;
    }
        
    
    if ((distance_from_pray <= 30 - satiety_Hormone * 13)  && state == 1)
    {
      pray_target = pray;
      state = 2;
    } 
    else if((distance_from_pray <= 10)  && state == 2)
    {
      state = 3;
      food_count = 5;
    }
        
}

void satiety_callback(const std_msgs::Float32 &msg)
{
  satiety_Hormone = msg.data;
}

void sleep_callback(const std_msgs::Float32 &msg)
{
  sleep_Hormone = msg.data;
  if (state == 1)
  {
     motor_speed = mapf(1 - sleep_Hormone, 0, 1, 250, 1500);  
  }
  else if (state == 2)
  {
    motor_speed = 1500;
  }
  
}

void death_callback(const std_msgs::Float32 &msg)
{
  death_Hormone = msg.data;
}

int mapf(double val, double in_min, double in_max, double out_min, double out_max) 
{
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void get_feedback_pub()
{
  //  Read feedback for each servo
  for (uint8_t servo_id = 1; servo_id <= NUM_SERVOS; ++servo_id) 
  {
    int servo_set = NUM_SERVOS;
    
    
    if (servo_id == 9)
    {
       servo_set = 12;
    }
    else if(servo_id == 3)
    {
       servo_set = 11; 
    }

    int load_feedback = sc.ReadLoad(servo_id);
    
    if (load_feedback != -1) 
    {
      load_data[servo_id - 1] = load_feedback;
    } 
    else 
    {  
      load_data[servo_id - 1] = 0; // Set load data to 0 as default value
    }
  }

  // Publish the load data array to "arduino_Load" topic
  load_msg.data_length = NUM_SERVOS;
  load_msg.data = load_data;
  load_pub.publish(&load_msg);
  

}
