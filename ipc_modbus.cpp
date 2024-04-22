//modbus
#include<iostream>
#include<modbus.h>
#include<unistd.h>
#include <vector>
//ipc
#include "ipc.h"

int main(int argc, char *argv[])
{
    IPC ipc("shmfile", 65, 5);

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
    // 센서 데이터 입력
    uint16_t sns_humid = ipc.read(0);
    
    // Write temperature to the server
    int rc = modbus_write_register(ctx, 0, sns_humid);
    
    if (rc == -1) {
        std::cerr << "Humid write failed: " << modbus_strerror(errno) << std::endl;
        modbus_close(ctx);
        modbus_free(ctx);
        return -1;
    }
    std::cout << "Data send [ Humid : " << sns_humid << " ] to WINDOW SERVER." << std::endl;

    // Read GPIO code from SERVER
    uint16_t control_code = ipc.read(2);
    rc = modbus_read_registers(ctx, 2, 1, &control_code); // (context, start regi addr, number of read, )
    if (rc == -1) {
        std::cerr << "Read Relay Control code failed: " << modbus_strerror(errno) << std::endl;
        modbus_close(ctx);
        modbus_free(ctx);
        return -1;
    }

    if(control_code == 1){
        std::cout << "Data Received [ Relay Control code : " << control_code << " ]"<<std::endl;
        ipc.write(2,1);  // write for send to Relay on Linux
        std::cout<< "Data send [ Relay Control code 1 to Relay ]"<<std::endl;
    }
    else
        ipc.write(2,0);
    
    sleep(1);
    }

    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}