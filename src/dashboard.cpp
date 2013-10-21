#include "dashboard.h"


CapyBoard::CapyBoard()
{
    initscr();
    _leftSpeed=0;
    _rightSpeed=0;
    _systemTime=0;
    _leftTick=0;
    _rightTick=0;
    _globalLeftTick=0;
    _globalRightTick=0;
    _posX=0;
    _posY=0;
    _posTheta=0;
    _globalPosX=0;
    _globalPosY=0;
    _globalPosTheta=0;
    _seq=0;
    memset(_dashboardName,'\0',100);
    memcpy(_errors,"no errors",100);
    sprintf(_dashboardName,"***************** Capybara Dashboard *****************");
}

CapyBoard::~CapyBoard()
{
    endwin();
}

void CapyBoard::draw()
{
    char buff[1024];

    mvprintw(0,0,_dashboardName);

    sprintf(buff,"Left speed:  %f",_leftSpeed);
    mvprintw(2,0,buff);
    sprintf(buff,"Right speed: %f",_rightSpeed);
    mvprintw(3,00,buff);


    sprintf(buff,"Left tick: %06d",_leftTick);
    mvprintw(2,35,buff);
    sprintf(buff,"Right tick: %06d",_rightTick);
    mvprintw(3,35,buff);

    sprintf(buff,"Global left tick:  %06d",_globalLeftTick);
    mvprintw(4,35,buff);
    sprintf(buff,"Global right tick: %06d",_globalRightTick);
    mvprintw(5,35,buff);

    sprintf(buff,"X: %06f",_posX);
    mvprintw(7,0,buff);
    sprintf(buff,"Y: %06f",_posY);
    mvprintw(8,0,buff);
    sprintf(buff,"Z: %06f",_posTheta);
    mvprintw(9,0,buff);

    sprintf(buff,"Global X: %06f",_globalPosX);
    mvprintw(7,35,buff);
    sprintf(buff,"Global Y: %06f",_globalPosY);
    mvprintw(8,35,buff);
    sprintf(buff,"Global Z: %06f",_globalPosTheta);
    mvprintw(9,35,buff);

    sprintf(buff,"-----------------------------------------------------");
    mvprintw(11,0,buff);
    sprintf(buff,"Seq: %d Errors: %s",_seq,_errors);
    mvprintw(12,0,buff);

    sprintf(buff,"Message: %s",_message);
    mvprintw(13,0,buff);

    refresh();
}

void CapyBoard::setEncoderTicks(int t1, int t2)
{
    _leftTick=t1;
    _rightTick=t2;
    _globalLeftTick+=t1;
    _globalRightTick+=t2;
}

void CapyBoard::setOdometry(float x, float y, float t, float gx, float gy, float gt)
{
    _posX=x;
    _posY=y;
    _posTheta=t;

    _globalPosX=gx;
    _globalPosY=gy;
    _globalPosTheta=gt;
}

void CapyBoard::setSeqNumber(int s)
{
    _seq=s;
}

void CapyBoard::setSystemTime(float t)
{
    _systemTime=t;
}

void CapyBoard::setError(char* e)
{
    memcpy(_errors,e,100);
}

void CapyBoard::setSpeed(int s1, int s2)
{
    _leftSpeed=s1;
    _rightSpeed=s2;
}


void CapyBoard::setMessage(char* c)
{
    memcpy(_message,c,1024);
}
