#!/usr/bin/env python3

import rospy
from std_msgs.msg import Float32
from std_msgs.msg import Int16MultiArray	

state = 1;
"""
1 = normal walking mode
2 = hunting mode
3 = sleep mode
4 = death mode
5 = eating mode
"""

satiety_stimulator = 0;

distance_from_pray1 = 1000;
distance_from_pray2 = 1000;
distance_from_pray3 = 1000;


def sleep_callback(data):
    sleep_Hormone = data.data
    if sleep_Hormone >= 0.73 and state == 1:
        state = 3;
    rospy.loginfo("sleep_hormone: %s", sleep_Hormone)
    
def satiety_callback(data):
    satiety_Hormone = data.data
    rospy.loginfo("satiety_Hormone: %s", satiety_Hormone)
    
def death_callback(data):
    death_Hormone = data.data
    rospy.loginfo("death_Hormone: %s", death_Hormone)
    
def camera_callback(data):
    camera_array = data.data
    if satiety_Hormone <= 0.25 and state == 1:
        if camera_array[4] == 3:
            distance_from_pray3 = camera_array[5]
            state = 2;
        else if camera_array[2] == 2:
            distance_from_pray2 = camera_array[3]
            state = 2;
        else if camera_array[0] == 1:
            distance_from_pray1 = camera_array[1]
            state = 2;
     



if __name__ == '__main__':

    rospy.init_node('brain_node')
    
    rospy.Publisher("food_stimulate", Int16, queue_size=10)
    # rospy.Publisher("food_stimulate", Int16, queue_size=10)
    
    rospy.Subscriber("sleep_hormone", Float32, sleep_callback)
    rospy.Subscriber("satiety_hormone", Float32, satiety_callback)
    rospy.Subscriber("death_hormone", Float32, death_callback)
    rospy.Subscriber("pixy", Int16MultiArray, camera_callback)

        
    if state == 2:
                
    
    
    rospy.spin()

