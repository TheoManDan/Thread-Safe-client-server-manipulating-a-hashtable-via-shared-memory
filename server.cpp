#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <chrono>
#include <thread>
#include <stdio.h>
#include "shared_memory.h"
#include "hash_table.h"
#include "thread_pool.h"

//The command dispatcher
void processCommand(Command command, HashTable *hashTable)
{
    switch (command.action)
    {
    case INSERT:
        std::cout << "Inserting " << command.key << std::endl;
        hashTable->insert(command.key, command.value);
        break;
    case READ:
        std::cout << "Reading " << command.key << std::endl;
        hashTable->read(command.key);
        break;
    case REMOVE:
        std::cout << "Removing " << command.key << std::endl;
        hashTable->remove(command.key);
        break;
    default:
        std::cerr << "Unknown command action" << std::endl;
    }
}

int main(int argc, char **argv)
{
    //parsing the arguments
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <hashmap_size>" << std::endl;
        return 1;
    }


    // first we open the shared memory segment
    // shmget() returns an identifier for the shared memory segment
    int shmid = shmget(SHM_KEY, 1024, 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        std::cerr << "Shared memory creation failed" << std::endl;
        return 1;
    }

    // Before you can use a shared memory segment 
    // you have to attach yourself to it using shmat(). 
    void *shmAddress = shmat(shmid, NULL, 0);
    if (shmAddress == (void *)-1)
    {
        std::cerr << "Shared memory attach failed" << std::endl;
        return 1;
    }

    // we create an instance of the shared memory class
    SharedMemory *sharedMemory = new (shmAddress) SharedMemory();

    // creates an instances of our hashtable
    HashTable *hashTable = new HashTable(atoi(argv[1]));

    // create our threadpool who will manage our tasks
    ThreadPool threadPool(std::thread::hardware_concurrency());


    // here we enter in the "server logic"
    while (true)
    {
        // check if we have something pending 
        // in our queue
        if (!sharedMemory->isEmpty())
        {
            Command command = sharedMemory->read();
            threadPool.enqueue(processCommand, command, hashTable);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }


    // When we are done with the shared memory segment
    // the program detach itself from it using shmdt().
    if (shmdt(sharedMemory) == -1)
    {
        std::cerr << "Shared memory detach failed" << std::endl;
        return 1;
    }


    // When we detach from shared memory, it is not destroyed. 
    // So, to destroy shmctl() is used.
    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        std::cerr << "Shared memory deletion failed" << std::endl;
        return 1;
    }

    // clear the hashtable
    delete hashTable;

    return 0;
}