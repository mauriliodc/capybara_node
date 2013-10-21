#ifndef _DASHBOARD_H
#define _DASHBOARD_H

#include <string>
#include "stdio.h"
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <iostream>


class ScreenWriter{

    public:
    ScreenWriter();
    ~ScreenWriter();
    void espettorator();

    private:

    float _leftSpeed;
    float _rightSpeed;
    float _systemTime;
    int _leftTick;
    int _rightTick;
    int _globalLeftTick;
    int _globalRightTick;
    float _posX;
    float _posY;
    float _posTheta;
    float _globalPosX;
    float _globalPosY;
    float _globalPosTheta;
    char _dashboardName[100];
    int _seq;
    char _errors[100];
    char _message[1024];

};

#endif

