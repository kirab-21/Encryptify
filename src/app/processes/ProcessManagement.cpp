#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <atomic>
#include <sys/fcntl.h>
#include "ProcessManagement.hpp"
#include "../encryptDecrypt/Cryption.hpp" 

ProcessManagement::ProcessManagement() {
    shmFd = shm_open(SHM_NAME, O_CREAT  | O_RDWR, 0666);
    ftruncate(shmFd, sizeof(SharedMemory));
    sharedMem = static_cast<SharedMemory*> (mmap(nullptr, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd,0));
    sharedMem->front = 0;
    sharedMem->rear = 0;
    sharedMem->size.store(0);
}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    if(sharedMem->size.load() >= 1000) {
        return false;
    }

    strcpy(sharedMem->tasks[sharedMem->rear], task->toString().c_str());

    sharedMem->rear = (sharedMem->rear+1) % 1000;
    sharedMem->size.fetch_add(1);

    int pid = fork();
    if(pid < 0) {
        return false;
    } else if (pid > 0) {
        std::cout<<"Entering the parent process"<<std::endl;
    } else {
        std::cout<<"Child Process";
        executeTasks();
        std::cout<<"Exit Child"<< std::endl;;
    }
    return true;
}

void ProcessManagement::executeTasks() {
    char taskStr[256];
    strcpy(taskStr, sharedMem->tasks[sharedMem->front]);
    sharedMem->front = (sharedMem->front+1) % 1000;
    sharedMem->size.fetch_sub(1);
    std::cout <<"Executing child process "<<std::endl;
    executeCryption(taskStr);
}
