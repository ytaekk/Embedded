//Sensor Header
#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
//IPC Header
#include <iostream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define MAX_TIMINGS 85
#define DHT_PIN 7

int data[5]={0, 0, 0, 0, 0};

std::vector<int> read_dht_data(int size)
{
    std::vector<int> ipc_arr(size);

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
            //printf("break\n", i);
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
        int h = (int)data[0];
        int c = (int)(data[2]&0x7F);
        if(data[2]&0x80) c=-c;
        //printf("Humidity = %d %% Temp = %d *C \n",h,c);
        ipc_arr[0] = h;
        ipc_arr[1] = c;

    }
    // else{
    //     //printf("Data not good, skip \n");

    // }
    return ipc_arr;
    
}

void ipc_func(int humi, int temp) //센서로 부터 값을 받아 인자로 입력
{
    // ftok to generate unique key
    key_t key = ftok("shmfile", 65);
 
    // shmget returns an identifier in shmid
    int shmid = shmget(key, sizeof(int)*5, 0666 | IPC_CREAT);
 
    // shmat to attach to shared memory
    int* arr = (int*)shmat(shmid, (void*)0, 0);

    // IPC 공유메모리 배열에 값 저장.
    arr[0] = humi;
    arr[1] = temp;
    arr[2] = 0;
    shmdt(arr);
}

int main()
{
    
    //Sensor
    printf("DHT11 TEMP / HUMIDITY TEST \n");

    if(wiringPiSetup()==-1)
        exit(1);

    while(1)
    {
        std::cout<<".. ";
        
        auto sns_value = read_dht_data(3);
        if(sns_value.size()>=2 && sns_value[0]>0){
            std::cout<<std::endl;
            std::cout << " humidity : " << sns_value[0] << " temp : " << sns_value[1] << std::endl;

            ipc_func(sns_value[0],sns_value[1]);
        }
        delay(1000);
    }


    return 0;
}
