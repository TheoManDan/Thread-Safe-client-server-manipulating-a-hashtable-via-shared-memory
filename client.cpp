#include <iostream>
#include <chrono>
#include <thread>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include "shared_memory.h"

int main(int argc, char **argv)
{
    //parsing the arguments
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <hashmap_size>" << std::endl;
        return 1;
    }

    // init the seed for random operations
    srand((unsigned)time(NULL));


    // first we try to reach the shared memory segment
    // shmget() returns an identifier for the shared memory segment
    int shmid = shmget(SHM_KEY, 1024, 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        std::cerr << "Shared memory creation failed" << std::endl;
        return 1;
    }

    // then we try to attach to the segment
    SharedMemory *sharedMemory = reinterpret_cast<SharedMemory *>(shmat(shmid, NULL, 0));
    if (sharedMemory == (void *)-1)
    {
        std::cerr << "Shared memory attach failed" << std::endl;
        return 1;
    }


    // performing random commands
    for (int i = 0; i < atoi(argv[1]); i++)
    {
        Command command = {static_cast<Action>(rand() % 3), rand(), NULL};
        sharedMemory->insert(command);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // at the end we detach to the shared memory
    if (shmdt(sharedMemory) == -1)
    {
        std::cerr << "Shared memory detach failed" << std::endl;
        return 1;
    }

    return 0;
}