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

    //공유메모리로부터 받을 값 저장을 위한 벡터 생성
    std::vector<int> value(3);
    //공유메모리 저장되어있는 값을 새 벡터에 복사
    for(int i=0; i<3; i++)
        value[i] = arr[i];
    shmdt(arr);
    //새 백터 리턴
    return value;
}

int main()
{
    int ON = 1;
    int OFF = 0;

    if(wiringPiSetup() == -1) return -1; //wiringPI 초기화

    pinMode(RELAY, OUTPUT); // 핀모드 출력 설정
    std::cout<<"초기상태"<<std::endl;
    digitalWrite(RELAY, ON); // 초기상태 : off
    int c=1;
    // while(1)
    // {
    //     // 공유메모리로부터 받은 값을 저장한 벡터를 sns_value 벡터에 저장
    //     auto sns_value = ipc_read();
    //     // 공유메모리[0](습도)가 50이상이면
    //             std::cout<<"count " << sns_value[2] << std::endl;

    //     if(sns_value[2]==1)
    //     {
    //         //릴레이 동작
    //         digitalWrite(RELAY, OFF);
    //         std::cout << "Relay is OFF";
    //         //break;
    //     }
    //     else
    //         digitalWrite(RELAY,ON);
    //     delay(1000);
    //     c++;
    // }

    

 
    return 0;

}