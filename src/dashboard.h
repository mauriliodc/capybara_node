#ifndef _DASHBOARD_H
#define _DASHBOARD_H

#include <string>
#include "stdio.h"
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <stdint.h>

class CapyBoard{

public:
    CapyBoard();
    ~CapyBoard();

    void draw();
    void setEncoderTicks(int, int);
    void setOdometry(float x, float y, float t, float gx, float gy, float gt);
    void setSeqNumber(int);
    void setSystemTime(float);
    void setError(char*);
    void setSpeed(int, int);
    void setMessage(char*);
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
