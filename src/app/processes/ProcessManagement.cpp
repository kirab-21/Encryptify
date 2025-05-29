#include <iostream>
#include <cstring>
#include <sys/wait.h>
#include "ProcessManagement.hpp"
#include "../encryptDecrypt/Cryption.hpp" 

ProcessManagement::ProcessManagement() {}

bool ProcessManagement::submitToQueue(std::unique_ptr<Task> task) {
    taskQueue.push(std::move(task));
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
    while(!taskQueue.empty()) {
        std::unique_ptr<Task> taskToExecute = std::move(taskQueue.front());
        taskQueue.pop();
        std::cout<< "Executing task: " <<taskToExecute->toString() << std::endl;
        executeCryption(taskToExecute->toString());
    }
}
