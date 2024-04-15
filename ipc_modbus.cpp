//modbus
#include<iostream>
#include<modbus.h>
#include<unistd.h>

//ipc
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>

std::vector<int> ipc_read(int control)
{
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(int)*5, 0666 | IPC_CREAT);
    int* arr = (int*)shmat(shmid, (void*)0, 0);

    //공유메모리로부터 받을 값 저장을 위한 벡터 생성
    std::vector<int> value(3);
    //공유메모리 저장되어있는 값을 새 벡터에 복사
    for(int i=0; i<3; i++)
        value[i] = arr[i];
    
    if(control==1)
        arr[2]=1;
    if(control==0)
        arr[2]=0;
    
    shmdt(arr);
    //새 백터 리턴
    return value;
}

int main(int argc, char *argv[])
{
    // 0. Set Server IP and Port, 502: Tcp/Ip
    const char *ip = "192.168.219.110";
    int port = 502;

    // 1. Make Modbus TCP Context
    modbus_t *ctx;
    ctx = modbus_new_tcp(ip,port);

    // 1-E. Error to Context
    if(ctx==nullptr){
        std::cerr<< "Error to make the libmodbus [Context]"<<std::endl;
        return -1;
    }

    // 2. Connect to Server
    // 2-E. Error to Connection
    if(modbus_connect(ctx)==-1){
        std::cerr<< "Connection Failed : "<<modbus_strerror(errno)<<std::endl;
        modbus_free(ctx);
        return -1;
    }

    while(1)
    {
    // 공유메모리 값  가져오기
    auto sns_value = ipc_read(3);
    
    // 센서 데이터 입력
    uint16_t sensor_temperature = sns_value[0];
    
    // Write temperature to the server
    int rc = modbus_write_register(ctx, 0, sensor_temperature);
    
    if (rc == -1) {
        std::cerr << "Temperature write failed: " << modbus_strerror(errno) << std::endl;
        modbus_close(ctx);
        modbus_free(ctx);
        return -1;
    }
    std::cout << "Temperature " << sensor_temperature << " degrees Celsius sent to server." << std::endl;

    // Read GPIO code from SERVER
    uint16_t control_code;
    rc = modbus_read_registers(ctx, 1, 1, &control_code); // Assume control code is stored at register 1
    if (rc == -1) {
        std::cerr << "Read control code failed: " << modbus_strerror(errno) << std::endl;
        modbus_close(ctx);
        modbus_free(ctx);
        return -1;
    }
    std::cout << "Control code received from server: " << control_code << std::endl;
    if(control_code == 1){
        ipc_read(1);
        std::cout<< "send gpio 1 to relay"<<std::endl;
    }
    else
        ipc_read(0);
   
    sleep(1);
    }

    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}