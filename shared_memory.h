#pragma once

#include <mutex>

#define SHM_KEY 0x88
#define SHM_SIZE 200

enum Action
{
    READ,
    INSERT,
    REMOVE
};

struct Command
{
    Action action;
    int key;
    void *value;
};

class SharedMemory
{
public:
    bool isEmpty();
    size_t size();
    void insert(Command command);
    Command read();

private:
    std::mutex _mutex;
    Command _commands[SHM_SIZE];
    int _readIndex = 0;
    int _writeIndex = 0;
};