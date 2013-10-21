#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <string>
#include "serialFriend.h"
#include "platform_defs.h"
#include "stdio.h"
#include "packets.h"
#include <unistd.h>
#include <pthread.h>
#include "capybara.h"
#include <ncurses.h>
#include "dashboard.h"
#include <math.h>
int i=0;
float oldR=0;
float oldL=0;
float maxInc=0.1;

Capybara* capybot=0;
void joystickCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
    float numba = 150.0f;
    float k1=0.5;
    float k2=0.2;
    float leftSpeed = numba*msg->linear.x;
    float rightSpeed = numba*msg->angular.z;

    if(abs(leftSpeed)>abs(oldL)){
        if(leftSpeed>0) leftSpeed+=maxInc;
        else leftSpeed-=maxInc;
    }
    oldL=leftSpeed;

    if(abs(rightSpeed)>abs(oldR)){
        if(rightSpeed>0) rightSpeed+=maxInc;
        else rightSpeed-=maxInc;
    }
    oldR=rightSpeed;


    float rval=k1*leftSpeed+k2*rightSpeed;
    float lval=k1*leftSpeed-k2*rightSpeed;
    capybot->setWheelSpeed(rval, lval);
    usleep(5000);
}


void *Run(void* context)
{

    while(1)
    {
        Capybara* c=(Capybara*)context;
        c->handleCommunication();
    }
    return NULL;
}



int main(int argc, char **argv)
{

    CapyBoard c;
    c.draw();
    PacketHandler_init();
    capybot = new Capybara(&c);
    capybot->openConnection(argv[1]);
    Packets_init();

    pthread_t threads[1];
    pthread_create(&threads[0], NULL, Run,capybot);

    ros::init(argc, argv, "capybara");

    ros::NodeHandle n;

    ros::Subscriber sub = n.subscribe("/cmd_vel", 1000, joystickCallback);

    ros::spin();


    return 0;
}

