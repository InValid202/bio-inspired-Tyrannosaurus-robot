#include <ros.h>
#include <std_msgs/Int16MultiArray.h>
#include <Pixy2.h>

// This is the main Pixy object
Pixy2 pixy;

// ROS node handle
ros::NodeHandle nh;

// ROS publisher
std_msgs::Int16MultiArray load_msg;
ros::Publisher load_pub("pixy", &load_msg);

#define center_x 150
#define range_x 50
#define speed 30
#define K_miniball 936
#define K_bigball 1616
#define k_object 2765

void setup() {
  pixy.init();

  // Initialize ROS node handle and publisher
  nh.initNode();
  nh.getHardware()->setBaud(57600); // Set baud rate to 57600
  nh.advertise(load_pub);

  // Initialize load_msg
  load_msg.data_length = 8; // since we are publishing two values: sig and d
  load_msg.data = (int16_t*)malloc(sizeof(int16_t) * load_msg.data_length);
}

void loop() { 
  // Grab blocks
  pixy.ccc.getBlocks();
  
  // If there are detected blocks, publish them
  if (pixy.ccc.numBlocks) {
    for (int i = 0; i < pixy.ccc.numBlocks; i++) {
      int sig = pixy.ccc.blocks[i].m_signature;
      int x = pixy.ccc.blocks[i].m_x;
      int y = pixy.ccc.blocks[i].m_y;
      long w = pixy.ccc.blocks[i].m_width;
      long h = pixy.ccc.blocks[i].m_height;
      long s = w * h;
      int miniball = K_miniball / w;
      int bigball = K_bigball / w;
      int object = k_object / w;

      if(i==0){
        load_msg.data[0] = {0};
        load_msg.data[1] = {0};
        load_msg.data[2] = {0};
        load_msg.data[3] = {0};
        load_msg.data[4] = {0};
        load_msg.data[5] = {0};
      }
      
      if(sig==1){
        load_msg.data[0] = sig;
        load_msg.data[1] = miniball;
      }
      else if(sig==2){
        load_msg.data[2] = sig;
        load_msg.data[3] = bigball;
      }
      else if(sig==3){
        load_msg.data[4] = sig;
        load_msg.data[5] = bigball;
      }
      else{
        load_msg.data[6] = sig;
        load_msg.data[7] = object;
      }
      
//      load_msg.data[0] = sig;
//      load_msg.data[1] = d;
      load_pub.publish(&load_msg);
    }
  }
  else{
    load_msg.data[0] = {0};
    load_msg.data[1] = {1000};
    load_msg.data[2] = {0};
    load_msg.data[3] = {1000};
    load_msg.data[4] = {0};
    load_msg.data[5] = {1000};
    load_msg.data[6] = {0};
    load_msg.data[7] = {1000};
    load_pub.publish(&load_msg);
  }
  nh.spinOnce();
  delay(1000); 
}
