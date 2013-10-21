#!/usr/bin/env python
import rospy
import roslib
roslib.load_manifest('capybara_running_wild')
from std_msgs.msg import String
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Quaternion
from geometry_msgs.msg import Twist
from geometry_msgs.msg import Pose
import tf
from math import sin
from math import cos
import serial
import threading
import sys
import traceback
import time

def odometryFromTicks(tr, tl, kr, kl, b):
	global poseX
	global poseY
	global poseT
	dr = kr*tr;
	dl = kl*tl;
	R = (dl+dr)/(2);
	theta = (dr-dl)/(2*b);
	poseT+=theta
	x = R * cos(poseT);
	y = R * sin(poseT);
	poseX+=x
	poseY+=y

	return (x,y,theta)

#############################################################################################
#THREAD TO UNFILL THE BUFFER
#############################################################################################
class CleaningThread(threading.Thread):
	buffer=''
	getCommand=0
	def run(self):
		global ser
		global calibBaseline
		global calibTickLeft
		global calibTickRight
		global poseX
		global poseY
		global poseT
		global pub
		global started
		global totalTickLeft
		global totalTickRight
		while 1 :
			#ser.flushInput()
			#ser.flushOutput()
			if self.getCommand==0:
				firstChar=str(ser.read(1))
				if firstChar=="#":
					self.getCommand=1
			if self.getCommand==1:
				commandChar=str(ser.read(1))
				if str(commandChar)!='@':
					self.buffer+=commandChar
					#print "LEN: "+`len(self.buffer)`
				if str(commandChar)=="@":
					SplittedMessage=self.buffer.split(" ");
					#print str(int(SplittedMessage[0])) + " "+str(int(SplittedMessage[1]))
					tl=int(SplittedMessage[0])
					tr=int(SplittedMessage[1])
					 #print str(tl)+" "+str(tr)
					(x,y,theta)=odometryFromTicks(-tl, tr, calibTickLeft,calibTickRight,calibBaseline)
					totalTickLeft+=(-tl)
					totalTickRight+=tr
					print str(totalTickLeft)+" "+str(totalTickRight)
					print "X: "+ str(poseX)+ " Y: "+str(poseY)+" Theta: "+str(poseT)
					self.getCommand=0

					quaternion = Quaternion()
					quaternion.x = 0.0 
					quaternion.y = 0.0
					quaternion.z = sin(poseT / 2.0)
					quaternion.w = cos(poseT / 2.0)
					odomBroadcaster.sendTransform((poseX,poseY, 0), (quaternion.x, quaternion.y, quaternion.z, quaternion.w),rospy.Time.now(),"base_footprint","odom")
					odom = Odometry()
					odom.header.frame_id = "/odom"
					odom.child_frame_id = "base_footprint"
					odom.header.stamp = rospy.Time.now()
					odom.pose.pose.position.x = poseX
					odom.pose.pose.position.y = poseY
					odom.pose.pose.position.z = 0
					odom.pose.pose.orientation = quaternion
					odom.twist.twist.linear.x = 0
					odom.twist.twist.linear.y = 0
					odom.twist.twist.angular.z = 0
					
					
					pub.publish(odom)
					self.buffer=""

#############################################################################################

##GLOBAL STUFF#####################################################################
LINEAR_SPEED = 30
ANGULAR_SPEED = 20
header = "$"
footer = "%"
k1=1 
k2=1
BigBuffer=''
started=0
calibTickLeft=0.014
calibTickRight=0.014
calibBaseline=0.195
poseX=0
poseY=0
poseT=0
totalTickLeft=0
totalTickRight=0
###################################################################################


##SERIAL STUFF####################################################################
ser = serial.Serial('/dev/ttyUSB0', 115200)
ser.xonxoff = False
ser.rtscts = False
ser.dsrdtr = False
ser.open()
ser.isOpen()
###################################################################################

##UTILITY FUNCTIONS###############################################################
def add_nulls(num, cnt=2):
	cnt = cnt - len(str(num))
	nulls = '0' * cnt
	return '%s%s' % (nulls, num)
##################################################################################


def SerialVelocityCommandSender(data):
	leftSpeed = -data.linear.x* LINEAR_SPEED
	rightSpeed = data.angular.z* ANGULAR_SPEED
	#print "LEFT  "+str(leftSpeed)
	#print "RIGHT "+str(rightSpeed)
	
	command=""
	rval=str(int(k1*leftSpeed+k2*rightSpeed))
	lval=str(int(k1*leftSpeed-k2*rightSpeed))
	rval=int(rval)
	lval=int(lval)
	#if rval>=0:
	#	code1="02"
	#if rval<0:
	#	code1="03"
	#	rval=-rval
	#if lval>=0:
	#	code2="05"
	#if lval<0:
	#	code2="06"
	#	lval=-lval
	rdir="0"
	ldir="1"
	if rval<0:
		rdir="1"
		rval=abs(rval)
	if lval<0:
		ldir="0"
		lval=abs(lval)
	#command=header+str(code1)+add_nulls(int(rval),4)+footer+"     "+header+str(code2)+add_nulls(int(lval),4)+footer
	command=header+"01"+ldir+rdir+add_nulls(int(lval),2)+add_nulls(int(rval),2)+footer
	ser.write(command)
	#print command
	#print "COMMAND "+command + " at time " + str(rospy.get_rostime())


def MainLoop():
	rospy.Subscriber("cmd_vel", Twist, SerialVelocityCommandSender)
	rospy.spin()


if __name__ == '__main__':
	rospy.init_node('capybara_control', anonymous=True)
	pub = rospy.Publisher('/odom', Odometry)
	odomBroadcaster = tf.TransformBroadcaster()
	t = CleaningThread()
	t.start()
	MainLoop()
	
	

