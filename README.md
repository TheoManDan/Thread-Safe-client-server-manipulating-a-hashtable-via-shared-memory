# Thread-Safe client/server manipulating a hashtable via shared memory

## Short architecture explaination

### the server

The server is the very first object to be launched. The server receives the size of the hash map
then initialize it. The hash-table is then attached in a segment of shared memory. The server 
calls also the ThreadPool class. Then we enter in a "server logic" in a infinite loop where we 
have the "command dispatcher" section. When it receives a command it create a new task (p-thread).

### the client 
The client will work in the same segment of shared memory (where the hashmap lives) and will 
perform a number of commands.

### the hash-table class
The hashmap class exists in languages such as Java or Python but not in C/C++ so
we have to do it. It may not be as rich as classical hashmaps but the functions needed 
for our use are implemented  

### the shared memory class
Here we define our shared memory segment attributes : key, size etc.
and of course we implement a logic to handle incoming commands. The class respect 
the constraint of "concurrent operations".

### thread pool
Since we want our system to be thread-safe we need a class to handle every incoming tasks.

## how to use

build everything
```
make
```

first start the server
```
./server <hashmap_size>
```

and finally the client side
```
./client <hashmap_size>
```

to clean binaries
```
make clean
```


## scalability analysis

    Hash Table Size: The scalability of the system could be impacted by the size of the hash table. As the size of the hash table increases, more memory would be required to store the data, and the time complexity of operations such as insertion, deletion, and search might increase, especially in case of collisions.

    Concurrency: The system utilizes multithreading to handle concurrent operations on the hash table. The scalability of the system with respect to concurrency depends on factors such as the number of CPU cores available, the efficiency of thread management by the operating system.

    Shared Memory Communication: The use of shared memory is a fast and efficient mean of inter-process communication, but it has limitations in terms of scalability. As the number of clients accessing the shared memory increases, contention for access to the shared memory region may arise, potentially leading to bottlenecks and so on.

    Hardware and Resource Constraints: Ultimately, the scalability of the system is also constrained by hardware limitations such as CPU processing power, memory capacity, and disk I/O bandwidth. 