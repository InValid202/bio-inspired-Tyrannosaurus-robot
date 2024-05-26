#!/usr/bin/env python3

import rospy
from std_msgs.msg import Int16MultiArray, Float32, Int16
import statistics
import math

Alpha =0.25
Beta =0.95

previous_hormone_intense = 0
now_hormone_intense = 0.75

# Global variable to store the latest 5 data from arduino_Load
latest_food_data = [0]

def sigmoid(value):
    return 1 / (1 + math.exp(-value))

def food_callback(data):
    global latest_food_data
    global previous_hormone_intense
    global now_hormone_intense
    
    converted_data = data.data
 
    # rospy.loginfo(converted_data)
    
    # keep only 5 latest data
    if len(latest_food_data) < 5:
        latest_food_data.append(converted_data)
    else:
        latest_food_data.pop(0)
        latest_food_data.append(converted_data)
    
    # rospy.loginfo(latest_food_data)
    sd_food = statistics.mean(latest_food_data)
    # rospy.loginfo("sd food: %s", sd_food)
    
    SI_food = math.tanh(sd_food)
    
    
    now_hormone_intense = Alpha* SI_food  + (Beta * previous_hormone_intense)
    previous_hormone_intense  = now_hormone_intense
    # rospy.loginfo("SI_food: %s", SI_food)
    # rospy.loginfo("now_hormone_intense: %s", (now_hormone_intense))  
    # rospy.loginfo("")  
    hormone_pub.publish(now_hormone_intense) 
    


if __name__ == '__main__':
    rospy.init_node('satiety_node')
    
    rospy.loginfo("satiety node has started")
    
    hormone_pub = rospy.Publisher('satiety_hormone', Float32, queue_size=10)    
    rospy.Subscriber("food_stimulate", Float32, food_callback)
    
    rospy.spin()

