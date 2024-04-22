//Sensor Header
#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>

//IPC
#include "ipc.h"

#define MAX_TIMINGS 85
#define DHT_PIN 7

int data[5]={0, 0, 0, 0, 0}; //센서 배열

std::vector<int> read_dht_data(int size) // 센서 측정 함수
{
    std::vector<int> sensor(size);

    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j=0;
    uint8_t i;

    for(i=0;i<5;i++) {data[i] = 0;}

    pinMode(DHT_PIN, OUTPUT);
    digitalWrite(DHT_PIN, LOW);
    delay(18);
    pinMode(DHT_PIN, INPUT);

    for(i=0; i<MAX_TIMINGS; i++)
    {
        counter = 0;
        
        while(digitalRead(DHT_PIN)==laststate)
        {
            counter++;
            delayMicroseconds(1);
            if(counter==255) 
            {
                break;
            }
        }

        laststate = digitalRead(DHT_PIN);
        if(counter==255) {
            break;}

        if((i>=4)&&(i%2==0))
        {
            data[j/8]<<=1;
            if(counter>16){
                data[j/8] |= 1;
            }
            j++;
        
        }
    }
    
    if((j>=40)&&(data[4]==((data[0]+data[1]+data[2]+data[3])&0xFF)))
    {
        int h = (int)data[0];   // 습도
        int c = (int)(data[2]&0x7F);    // 온도
        if(data[2]&0x80) c=-c;
        sensor[0] = h;
        sensor[1] = c;
        sensor[2] = 0;  //Contorl code 초기화 0
    }

    return sensor;
    
}

int main()
{
    IPC ipc("shmfile", 65, 5);
    
    //Sensor
    printf("Humidity , Temp Sensor Run! \n");

    if(wiringPiSetup()==-1)
        exit(1);

    while(1)
    {
        auto sns_value = read_dht_data(3);
        if(sns_value.size()>=2 && sns_value[0]>0){
            std::cout<<std::endl;
            std::cout << "Data send [ humidity : " << sns_value[0] << " temp : " << sns_value[1] << " ]" <<std::endl;

            ipc.write(0,sns_value[0]);  // IPC 공유메모리 index 0 : Humid
            ipc.write(1,sns_value[1]);  // IPC 공유메모리 index 1 : Temperature
        }
        delay(1000);
    }


    return 0;
}
