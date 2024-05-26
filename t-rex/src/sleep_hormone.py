#!/usr/bin/env python3

import rospy
from std_msgs.msg import Int16MultiArray, Int16, Float32

import statistics
import math

Alpha = 0.6
Beta = 0.55

previous_hormone_intense = 0
now_hormone_intense = 0
light_intensity = 0

# Global variable to store the latest 5 data from arduino_Load
latest_toque_data = []

def sigmoid(value):
    return 1 / (1 + math.exp(-value))

def Load_callback(data):
    global latest_toque_data
    global previous_hormone_intense
    global now_hormone_intense
    global light_intensity
    
    converted_data = [abs(value) for value in data.data]
 
    # rospy.loginfo(converted_data)
    
    # keep only 5 latest data
    if len(latest_toque_data) < 5:
        latest_toque_data.append(sum(converted_data))
    else:
        latest_toque_data.pop(0)
        latest_toque_data.append(sum(converted_data))
    
    # rospy.loginfo(latest_toque_data)
    mean_load = (statistics.mean(latest_toque_data))/1000
    # rospy.loginfo("Mean Load: %s", mean_load)
    
    Sigmoid_light_load = math.tanh(mean_load * light_intensity)
    
    now_hormone_intense = Alpha * Sigmoid_light_load + (Beta * previous_hormone_intense)
    previous_hormone_intense = now_hormone_intense
    
    # rospy.loginfo("sigmoid: %s", Sigmoid_light_load)
    # rospy.loginfo("now_hormone_intense: %s", now_hormone_intense)
    # rospy.loginfo("")
    
    # Publish now_hormone_intense to the topic sleep_hormone
    hormone_pub.publish(now_hormone_intense)


def Light_callback(data):
    global light_intensity

    light_intensity = abs(1030 - data.data) / 1030
    # rospy.loginfo("Light intensity: %s", light_intensity)



if __name__ == '__main__':
    # Initialize the publisher for the sleep hormone
    rospy.init_node('sleep_node')
    
    rospy.loginfo("sleep node has started")
    
    hormone_pub = rospy.Publisher('sleep_hormone', Float32, queue_size=10)
    rospy.Subscriber("arduino_Load", Int16MultiArray, Load_callback)
    rospy.Subscriber("Light", Int16, Light_callback)
    
    rospy.spin()

