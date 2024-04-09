//Relay
#include<stdio.h>
#include<wiringPi.h>

//IPC
#include <iostream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>

#define RELAY 2 //Pin 설정

 std::vector<int> ipc_read()
{
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(int)*5, 0666 | IPC_CREAT);
    int* arr = (int*)shmat(shmid, (void*)0, 0);

    std::vector<int> value(2);
    for(int i=0; i<2; i++)
        value[i] = arr[i];
    
    shmdt(arr);

    return value;
}

int main()
{
    int ON = 1;
    int OFF = 0;

    if(wiringPiSetup() == -1) return -1; //wiringPI 초기화

    pinMode(RELAY, OUTPUT); // 핀모드 출력 설정
    digitalWrite(RELAY, ON); // 초기상태 : off

    while(1)
    {
        auto sns_value = ipc_read();
        if(sns_value[0]>50)
        {
            digitalWrite(RELAY, OFF);
            std::cout << "Relay is OFF";
            //break;
        }
        else
            digitalWrite(RELAY,ON);
        delay(1000);

    }

    

 
    return 0;

}