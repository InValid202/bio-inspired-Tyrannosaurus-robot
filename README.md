# Bio-inspired-Tyrannosaurus-robot
Dinosaur project for RE60429
## Feature
### Robot

Front view

<img src="https://github.com/InValid202/bio-inspired-Tyrannosaurus-robot/blob/main/source%20image/front_t-rex.jpg" width="800" height="400" />

Side view

<img src="https://github.com/InValid202/bio-inspired-Tyrannosaurus-robot/blob/main/source%20image/side_t-rex.jpg" width="800" height="400" />

Bird's eye view

<img src="https://github.com/InValid202/bio-inspired-Tyrannosaurus-robot/blob/main/source%20image/up_t-rex.jpg" width="800" height="400" />

### T-REX's food

<img src="https://github.com/InValid202/bio-inspired-Tyrannosaurus-robot/blob/main/source%20image/food_list.jpg" width="800" height="400" />

### T-REX's life cycle
[![Watch the video](https://img.shields.io/badge/YouTube-Watch-red?logo=youtube)](https://youtu.be/6K5D1mEeCWA?si=galV0f8BpNHcJzAX)

## Theory
### CPG with VRNs

<img src="https://github.com/InValid202/bio-inspired-Tyrannosaurus-robot/blob/main/source%20image/so2.jpg" width="800" height="400" />

spreadsheets for represent CPG and VRNs: [https://docs.google.com/spreadsheets/d/1AZzXm8qZo_j3j6RvV-UtCaSccyqd9fPiu6YYuq5pqpM/edit#gid=0](https://docs.google.com/spreadsheets/d/1AZzXm8qZo_j3j6RvV-UtCaSccyqd9fPiu6YYuq5pqpM/edit?usp=sharing)

### Hormone

<img src="https://github.com/InValid202/bio-inspired-Tyrannosaurus-robot/blob/main/source%20image/hormone_concept.png" width="800" height="400" />

Example spreadsheets for represent artificial hormones: https://docs.google.com/spreadsheets/d/1hGDivs0pLVSncBLwVF0gHtu8sqr4KoLUj5qkTngY2NQ/edit?usp=sharing

addition paper: [FIBO205_240514_210108.pdf](https://github.com/InValid202/bio-inspired-Tyrannosaurus-robot/files/15449666/FIBO205_240514_210108.pdf)

### System diagram

<img src="https://github.com/InValid202/bio-inspired-Tyrannosaurus-robot/blob/main/source%20image/idea%20system%20overview.jpg" width="800" height="400" />

After trying to use this system diagram as the core concept for programming, I found that rosserial on the Arduino UNO R4 couldn't handle a message from the ROS CPG node. So, I integrated the following nodes together in the Arduino UNO R4 and ended up with this diagram:

<img src="https://github.com/InValid202/bio-inspired-Tyrannosaurus-robot/blob/main/source%20image/idea%20system%20overview_real.jpg" width="800" height="400" />

### Distance calculation Between an Object and a Camera
#### Equation for Finding the Distance Between an Object
and a Camera

𝐷 = 𝑘 / d

Where:

• 𝐷 is the size of the object in the image

• 𝑑 is the distance between the camera and the object

• 𝑘 is a constant that needs to be determined from the actual size of the object

#### Finding the Constant 𝑘
If you know the actual size of the object ( 𝑆 ), you can use the following relationship to
find the constant 𝑘 :

𝑘 = 𝐷 * 𝑑

However, since we do not have the distance ( 𝑑 ) and the diameter in the image ( 𝐷 ), we
need to use the actual size of the object ( 𝑆 ) as a basis to create a general equation.
Suppose you have the actual size of the object 𝑆 = 40 mm.

#### Creating the General Equation
We know that the size of the object in the image ( 𝐷 ) is inversely proportional to the
distance ( 𝑑 ), with a constant 𝑘 that is related to the actual size of the object 𝑆 :

𝑘 = 𝑆 * 𝑑

Thus, the equation becomes:

𝐷 = 𝑘 / d

When 𝑘 = 𝑆 * 𝑑

#### Summary of the Equation

𝐷 = (𝑆 * 𝑑) / 𝑑 = 𝑆

If you know the actual size of the object and the size in the image, you can find the
distance:

𝑑 = 𝑘 / 𝐷

If the constant 𝑘k is not known, you need to know the size of the object in the image
for a given distance or know the focal length of the camera to establish this relationship.

#### Example Usage

Suppose you know the actual size of the object 𝑆=40 mm. and the size of the object in
the image 𝐷 :

If you know that the size of the object in the image at a distance 𝑑 = 10 cm. is 𝐷 = 100
mm. :

1. Find the constant 𝑘 :
𝑘 = 𝐷 * 𝑑 = 100 * 10 = 1000
2. Use the general equation:
𝐷 = 1000 * 𝑑
3. If the size of the object in the image 𝐷D is 50 mm and you want to find 𝑑d :
𝑑 = 1000 / 50 = 20 cm.

This equation can be used when you know the actual size of the object and have
information about the size of the object in the image or the distance in one case to find
the constant 𝑘 first.

## installation

### Requirement:

#### software:

1. Ubuntu 20.04
2. ROS noetic
3. Arduino IDE
4. Node-RED (Node-RED install tutorial: https://www.arubacloud.com/tutorial/how-to-install-node-red-on-ubuntu-20-04.aspx)

#### Hardware:

1. Arduino Uno R4 WiFi

2. Arduino Uno R3 

3. Feetech SCS SERVO CONTROLLER

4. Jetson nano

5. Servo scs15

6. pixy2.1
   
7. ZX-02F Light sensor

8. PowerBank 5 V 2A

9. LiPO Battery 3000mAh

10. Rocker switch ON-OFF

step 1: go to src in your catkin_ws with terminal and run this command in terminal

         git clone https://github.com/InValid202/bio-inspired-Tyrannosaurus-robot.git

step 2: copy both SCServo-master and Pixy2 library file and paste them in libraries folder of Arduino folder

step 3: add the lastest version of Rosserial_Arduino_Library with arduino IDE, follow by this guide: https://support.arduino.cc/hc/en-us/articles/5145457742236-Add-libraries-to-Arduino-IDE

step 4: go to src in your catkin_ws with terminal and run this command in terminal to clone rosserial package for ros

         https://github.com/ros-drivers/rosserial.git

step 5: go back to catkin_ws with terminal and run this command to make package

         catkin_make

step 6: upload code in T-rex.ino to Arduino UNO R4 WIFI

step 7: upload code in pixy_ros.ino to Arduino Uno

step 8: go in to rosserial_arduino package in rosserial that you just clone from step above and go in to nodes folder and edit rospy.init_node

         rospy.init_node("serial_node") --> rospy.init_node("serial_node", anonymous=True)

step 9: launch node-red with this command 

         node-red

step 10: import flows.json that located in node-red_flow folder to Node-RED

step 11: click deploy and use ctrl+shift+d to open dash board

step : use this command to start ros master

         roscore

step : start both rosserial for both arduino

         rosrun rosserial_arduino serial_node.py _port:=/dev/ttyACM0 _baud:=57600
         rosrun rosserial_arduino serial_node.py _port:=/dev/ttyACM1 _baud:=57600

step : launch hormones nodes with this command

         roslaunch t-rex hormone.launch

## Group member

Tumpoom Kanta-in 6452500368 

Natthachai piyatarathibeth 6452500171 

Paradon chaiyasat 6452500333 

Terdsak Kanjanawerod 6452500406 

Precha Chuwong 6452500414 

Panchapol Supankhan 6452500091 
