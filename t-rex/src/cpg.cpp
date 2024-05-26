#include "ros/ros.h"
#include "std_msgs/Int16MultiArray.h"
#include "std_msgs/Int16.h"
#include <vector>
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For time()
#include <cmath>
#include <math.h>


//impedance control
float stiffness = 0.05;
float damping = 0.032;


const int to_low_motor2 = 70;
const int to_high_motor2 = 250;

const int to_low_motor3 = 200;
const int to_high_motor3 = 350;

const int to_low_motor9 = 70;
const int to_high_motor9 = 250;
// const int to_high_motor9 = 370;

const int to_low_motor7 = 200;
const int to_high_motor7 = 350;
// const int to_high_motor7 = 500;


int16_t input1 = 1;
int16_t input2 = 1;



// right
int motor1 = 0; 
int motor2 = 0; // front right
int motor3 = 0; // back right
int motor4 = 0;
int motor5 = 0;


int motor6 = 0; // not found 

// left
int motor7 = 0; // back left
int motor8 = 0;
int motor9 = 0; // front left
int motor10 = 0;
int motor11 = 0;
int motor12 = 0;

float interval = 15;

std::vector<int16_t> load_data { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
std::vector<int16_t> speed_data { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
std::vector<int16_t> position_data { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

std::vector<int16_t> motor_command;


int mapf(double val, double in_min, double in_max, double out_min, double out_max) 
{
  return (val - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void input1_callback(const std_msgs::Int16::ConstPtr& msg)
{
    input1 = msg->data; 	
}


void input2_callback(const std_msgs::Int16::ConstPtr& msg)
{
    input2 = msg->data; 
}



void load_callback(const std_msgs::Int16MultiArray::ConstPtr& msg)
{
    load_data = msg->data;
}



void speed_callback(const std_msgs::Int16MultiArray::ConstPtr& msg)
{

    speed_data = msg->data;
}


void position_callback(const std_msgs::Int16MultiArray::ConstPtr& msg)
{
    position_data = msg->data; 
}


int main(int argc, char **argv)
{


	// Initialize the ROS system and become a node.
	    ros::init(argc, argv, "cpg");
	    ros::NodeHandle nh;
	    
	    // CPG's variable

	    float alpha = 1.03;
	    float phi = M_PI/10;
	    float MI = 0;
	    
	    float B_CPG = 0;
	    	    
	    float w00 = alpha * cos(phi);
  	    float w01 = alpha * sin(phi);
  	    float w10 = -(alpha * (sin(phi)) + MI);
	    float w11 = alpha * cos(phi);
	    	    
	    float c0 = 0.5;
	    float c1 = 0.5;
	    
	    float o0 = tanh(c0);
	    float o1 = tanh(c1);
	    
	    ROS_INFO("first o2: %f", o0);
	    ROS_INFO("first o1: %f", o1);
	    
	    
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
	    
	    // Create a publisher object.
	    ros::Publisher pub = nh.advertise<std_msgs::Int16MultiArray>("cpg", 12);
	    
	    
	    // Create a subscriber object.
	    
	    ros::Subscriber sub1 = nh.subscribe("input_1", 1000, input1_callback);
	    ros::Subscriber sub2 = nh.subscribe("input_2", 1000, input2_callback);
	    
	    ros::Subscriber sub_load = nh.subscribe("arduino_Load", 1000, load_callback);
	    ros::Subscriber sub_speed = nh.subscribe("arduino_Position", 1000, speed_callback);
	    ros::Subscriber sub_position = nh.subscribe("arduino_Speed", 1000, position_callback);
	    


	    // Loop at 1Hz until the node is shut down.
	    ros::Rate rate(interval);
	    while (ros::ok())
	    {
	    	// CPG SO2 equation
	    	c0 = w10 * o1 + w00 * o0;
	    	c1 = w01 * o0 + w11 * o1;
	    	
	    	// ROS_INFO("c0: %f", c0);
	    	// ROS_INFO("C1: %f", c1);
	    
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
		

	    	ROS_INFO("o0 : %f", o0);
	    	ROS_INFO("o1 : %f", o1);
	    	
	    	ROS_INFO("o8 : %f", o8);
	    	ROS_INFO("o15: %f", o15);
	    	
	    	ROS_INFO("next");
	    	
	    	
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
		
		ROS_INFO("checkpoint start");
		
		motor_command.push_back(0); // 1
		motor_command.push_back(1023 - mapf(o15, -0.3031119357, 0.3033082662, to_low_motor2, to_high_motor2)); // 2
		motor_command.push_back(1023 - mapf(o1, -0.3352863688, 0.3384303953, to_low_motor3, to_high_motor3)); // 3
		motor_command.push_back(0); // 4
		motor_command.push_back(0); // 5
		motor_command.push_back(0); // 6
		motor_command.push_back(mapf(o0, -0.3353811311, 0.3358224348, to_low_motor7, to_high_motor7)); // 7 
		motor_command.push_back(0); // 8
		motor_command.push_back(mapf(o8, -0.3031119357, 0.3033082662, to_low_motor9, to_high_motor9)); // 9
		motor_command.push_back(0); // 10
		motor_command.push_back(0); // 11
		motor_command.push_back(0); // 12
		
		
	  	// motor2 = 1023 - mapf(o15, -0.3031119357, 0.3033082662, to_low_motor2, to_high_motor2);
	  	// motor3 = 1023 - mapf(o1, -0.3352863688, 0.3384303953, to_low_motor3, to_high_motor3);
	  	
	  	
	  	// motor7 = mapf(o0, -0.3353811311, 0.3358224348, to_low_motor7, to_high_motor7);
	  	// motor9 = mapf(o8, -0.3031119357, 0.3033082662, to_low_motor9, to_high_motor9);
	  	
	    	

	      	// Create and fill in the message.
		std_msgs::Int16MultiArray msg;

		// Define layout if needed (optional).
		msg.layout.dim.push_back(std_msgs::MultiArrayDimension());
		msg.layout.dim[0].size = 12;
		msg.layout.dim[0].stride = 1;
		msg.layout.dim[0].label = "motor_output";
		
	    	for (int i = 0; i < 12; i++) 
	    	{
	    		
  			//float torque = (stiffness*(motor_command[i] - position_data[i])) + (damping * (0.0- speed_data[i]));
  			
  			// msg.data.push_back(motor_command[i]+ torque);
  			msg.data.push_back(motor_command[i]);
		} 
	  	
	  	motor_command.clear();
	  	
		// Publish the message.
		pub.publish(msg);

		// Wait until it's time for another iteration.
		rate.sleep();
    }
}
