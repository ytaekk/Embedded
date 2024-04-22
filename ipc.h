#pragma once

#include <iostream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <vector>
#include <stdexcept>

class IPC
{
private:
    key_t key;
    int shmid;
    int* arr;
    size_t size;

public:
    IPC(const char* name, int id, int size) : size(size) {
        key = ftok(name, id);
        if (key == -1) {
             perror("Error using ftok"); 
            throw std::runtime_error("ftok Fail");
        }

        shmid = shmget(key, sizeof(int)*size, 0666 | IPC_CREAT);
        if (shmid == -1) {
            throw std::runtime_error("shmget Fail");
        }

        arr = static_cast<int*>(shmat(shmid, (void*)0, 0));
        if (arr == (void*)-1) {
            throw std::runtime_error("shmat Fail");
        }
    }

    ~IPC(){
        if(shmdt(arr)==-1) {
            std::cerr << "Shmdt Fail \n";
        }
    }
    
    void write(int index, int value) {
        if(index >= size){
            throw std::out_of_range("인덱스 범위 초과");
        }
        arr[index] = value;
    }

    int read(size_t index) const {
        if (index >= size) {
            throw std::out_of_range("인덱스 범위 초과");
        }
        return arr[index];
    }
};

