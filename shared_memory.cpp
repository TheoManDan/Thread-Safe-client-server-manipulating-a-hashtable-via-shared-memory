#include "shared_memory.h"

bool SharedMemory::isEmpty()
{
    return _readIndex == _writeIndex;
}

size_t SharedMemory::size()
{
    if (_readIndex <= _writeIndex)
    {
        return _writeIndex - _readIndex;
    }

    return SHM_SIZE - _readIndex + _writeIndex;
}

void SharedMemory::insert(Command command)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (size() == SHM_SIZE - 1)
    {
        throw std::runtime_error("Shared memory is full");
    }

    _commands[_writeIndex] = command;
    _writeIndex = (_writeIndex + 1) % SHM_SIZE;
}

Command SharedMemory::read()
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (isEmpty())
    {
        throw std::runtime_error("Shared memory is empty");
    }

    Command command = _commands[_readIndex];
    _readIndex = (_readIndex + 1) % SHM_SIZE;

    return command;
}