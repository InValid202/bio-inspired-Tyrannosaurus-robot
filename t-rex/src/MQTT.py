#!/usr/bin/env python3

import rospy
import paho.mqtt.client as mqtt
from std_msgs.msg import Float32, Int16

# Callback function to handle MQTT connection
def on_connect(client, userdata, flags, rc):
    rospy.loginfo("Connected to MQTT broker with result code " + str(rc))

# Callback function for receiving data from 'sleep_hormone' topic and publishing to MQTT topic 'sleep_hormone'
def sleep_hormone_callback(data):
    mqtt_client.publish("sleep_hormone", str(data.data))

# Callback function for receiving data from 'satiety_hormone' topic and publishing to MQTT topic 'satiety_hormone'
def satiety_hormone_callback(data):
    mqtt_client.publish("satiety_hormone", str(data.data))

# Callback function for receiving data from 'death_hormone' topic and publishing to MQTT topic 'death_hormone'
def death_hormone_callback(data):
    mqtt_client.publish("death_hormone", str(data.data))
    
def death_bar_callback(data):
    mqtt_client.publish("death_bar", str(data.data))
    
def sleep_bar_callback(data):
    mqtt_client.publish("sleep_bar", str(data.data))

# Initialize ROS node
rospy.init_node('ros_to_mqtt')

# Set MQTT broker address and port
mqtt_broker_address = "mqtt-dashboard.com"
mqtt_broker_port = 1883

# Initialize MQTT client
mqtt_client = mqtt.Client()
mqtt_client.on_connect = on_connect

# Connect to MQTT broker
mqtt_client.connect(mqtt_broker_address, mqtt_broker_port)

# Set ROS loop rate (optional)
rate = rospy.Rate(10)  # 10 Hz

# Subscribe to 'sleep_hormone', 'satiety_hormone', and 'death_hormone' topics and set respective callback functions
rospy.Subscriber("sleep_hormone", Float32, sleep_hormone_callback)
rospy.Subscriber("satiety_hormone", Float32, satiety_hormone_callback)
rospy.Subscriber("death_hormone", Float32, death_hormone_callback)
rospy.Subscriber("death_bar", Int16, death_bar_callback)
rospy.Subscriber("sleep_bar", Int16, sleep_bar_callback)

while not rospy.is_shutdown():
    # Your ROS topic data fetching logic goes here
    # For example, if you want to publish the current time
    current_time = rospy.get_rostime()

    # Sleep to maintain ROS loop rate
    rate.sleep()

# Spin ROS
rospy.spin()

