#!/usr/bin/env python3

import rospy
from std_msgs.msg import Int16MultiArray, Float32, Int16
import statistics
import math

Alpha =0.25
Beta =0.6

previous_hormone_intense = 0
now_hormone_intense = 0
hungry_stimulate = 0


# Global variable to store the latest 5 data from arduino_Load
latest_toque_data = []

def sigmoid(value):
    return 1 / (1 + math.exp(-value))

def Load_callback(data):

    global latest_toque_data
    global previous_hormone_intense
    global now_hormone_intense
    
    converted_data = [abs(value) for value in data.data]
 
    # rospy.loginfo(converted_data)
    
    # keep only 5 latest data
    if len(latest_toque_data) < 5:
        latest_toque_data.append(sum(converted_data))
    else:
        latest_toque_data.pop(0)
        latest_toque_data.append(sum(converted_data))
    
    # rospy.loginfo(latest_toque_data)
    mean_load = statistics.mean(latest_toque_data)/1000
    
    # rospy.loginfo("Mean Load death: %s", mean_load)
    # rospy.loginfo("hungry_stimulate: %s", hungry_stimulate)
    
    SI_hungry_load = math.tanh(mean_load * hungry_stimulate)
    
    
    now_hormone_intense = Alpha * SI_hungry_load + (Beta * previous_hormone_intense)
    previous_hormone_intense  = now_hormone_intense
    # rospy.loginfo("SI_hungry_load: %s", SI_hungry_load)
    # rospy.loginfo("now_hormone_intense: %s", now_hormone_intense)  
    # rospy.loginfo("")  
    hormone_pub.publish(now_hormone_intense) 
    
def hungry_callback(data):
    global hungry_stimulate 
    satiety_intensity = data.data
    hungry_stimulate = 1 - satiety_intensity
    # rospy.loginfo("hungry_stimulate: %s", hungry_stimulate)

if __name__ == '__main__':
    rospy.init_node('death_node')
    
    rospy.loginfo("death node has started")
    
    hormone_pub = rospy.Publisher('death_hormone', Float32, queue_size=10)
    rospy.Subscriber("arduino_Load", Int16MultiArray, Load_callback)
    rospy.Subscriber("satiety_hormone", Float32, hungry_callback)
    
    rospy.spin()

