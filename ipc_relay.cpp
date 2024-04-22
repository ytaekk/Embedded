//Relay
#include<stdio.h>
#include<wiringPi.h>

//IPC
#include "ipc.h"

#define RELAY 2 //Pin 설정

int main()
{
    IPC ipc("shmfile", 65, 5);

    int ON = 1;
    int OFF = 0;

    if(wiringPiSetup() == -1) return -1; //wiringPI 초기화

    pinMode(RELAY, OUTPUT); // 핀모드 출력 설정
    digitalWrite(RELAY, ON); // 초기상태 : off

    while(1)
    {
        int count = 1;
        std::cout << "Data Received [" << count << "] Relay Operting code : " << ipc.read(2) << std::endl;

        if(ipc.read(2)==1) //ipc.read(2) : relay operating code
        {
            //릴레이 Open
            digitalWrite(RELAY, OFF);
            std::cout << "Relay is Operated";
        }
        else
            digitalWrite(RELAY,ON);
        count++;
        delay(1000);
    }
 
    return 0;
}