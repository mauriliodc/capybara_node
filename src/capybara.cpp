#include "capybara.h"
#include <iostream>

Capybara::Capybara(CapyBoard* b)
{
    this->_serialFriend=0;
    this->_lastCommand=0;
    StatePacket_initHeader(&this->_statePacket);
    SpeedPacket_initHeader(&this->_speedPacket);
    WheelSpeedPacket_initHeader(&this->_wheelSpeedPacket);
    parsedPacket = (PacketHeader *) packetBuffer;
    HexMessage_setBuffer(&outputStream, outputBuffer, MAX_BUFFER_SIZE);
    HexMessage_setBuffer(&inputStream, inputBuffer, MAX_BUFFER_SIZE);

    this->bstart=this->c;
    this->bcurr=this->c;
    this->bend=(c+MAX_BUFFER_SIZE);
    this->receiving_command=0;

    board=b;
}

Capybara::~Capybara()
{

}


bool Capybara::openConnection(std::string device)
{
    _serialFriend = new SerialFriend(device, B115200);
    return true;
}

void Capybara::closeConnection()
{
    if (this->_serialFriend) {
        delete this->_serialFriend;
        this->_serialFriend = 0;
    }
}

bool Capybara::isConnectionOpen() {
    return this->_serialFriend;
}


void Capybara::handleCommunication()
{
    char cc;
    while(1)
    {
        if (_lastCommand)
        {
            Packet_write(&outputStream,_lastCommand);
            this->_serialFriend->write(outputStream.start);
            HexMessage_reset(&outputStream);
            _lastCommand=0;
        }

        if(this->_serialFriend->read(&cc)<0)
        {
            continue;
        }

        if(cc==this->_serialFriend->getHeader())
        {
            receiving_command=1;
            *bcurr =cc;
            bcurr++;
        }
        else if (bcurr<bend-1 && receiving_command)
        {
            if (cc==this->_serialFriend->getFooter()){
                int size = bcurr-bstart;

                HexMessage_setBuffer(&inputStream, &c[1], size-2); // skip the newline
                lastMessage=c;
                PacketHandlerLoop(&inputStream,&outputStream);
                HexMessage_reset(&inputStream);
                memset(c,'\0',size);
                //printf("%s \n",c);
                receiving_command=0;
                bcurr = bstart;
            }
            if(cc!=this->_serialFriend->getFooter())
            {
                *bcurr =cc;
                bcurr++;
            }
        }
        this->board->draw();
    }
    usleep(3000000);
}

void Capybara::setWheelSpeed(int lws, int rws)
{
    WheelSpeedPacket_initHeader(&this->_wheelSpeedPacket);
    this->_wheelSpeedPacket.leftSpeed = lws;
    this->_wheelSpeedPacket.rightSpeed = rws;
    this->board->setSpeed(lws,rws);
    this->_lastCommand = (PacketHeader*)(&this->_wheelSpeedPacket);
}


void Capybara::PacketHandlerLoop(HexMessage* inputStream, HexMessage* outputStream){
    int error=0;
    HexMessage_reset(&this->outputStream);
    while(! error && this->inputStream.current!=this->inputStream.end){
        enum HexMessageStatus status=Packet_read(&this->inputStream,this->parsedPacket);
        if (status==Ok )
        {
            if (this->parsedPacket->type>=0)
            {
                //printf("PRINT\n");
                //Packet_print(this->parsedPacket);
                //printf("EXECUTE\n");
                //Packet_execute(parsedPacket, outputStream);

                if(this->parsedPacket->type==0x1)
                {
                StatePacket* s=(StatePacket*)this->parsedPacket;
                this->board->setEncoderTicks(s->leftEncoder,s->rightEncoder);
                this->board->setSeqNumber(s->seq);
                this->board->setOdometry(s->incrementalOdometry[0],s->incrementalOdometry[1],s->incrementalOdometry[2],s->globalOdometry[0],s->globalOdometry[1],s->globalOdometry[2]);
                this->board->setMessage(lastMessage);
                }
            }
        }
        if (status!=Ok )
        {
            char errorMessage[100];
            sprintf(errorMessage,"Parsing error %d \n",status);
            this->board->setError(errorMessage);
            error = 1;
        }
    }
}
