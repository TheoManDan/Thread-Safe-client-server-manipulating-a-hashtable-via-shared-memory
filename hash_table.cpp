#include "hash_table.h"

HashTable::HashTable(int size)
{
    _size = size;
    _nodes = new Node *[_size];
    for (int i = 0; i < _size; i++)
    {
        _nodes[i] = new Node();
        _nodes[i]->key = -1;
        _nodes[i]->value = nullptr;
        _nodes[i]->next = nullptr;
    }
}

int HashTable::hash(int key)
{
    return key % _size;
}

void HashTable::insert(int key, void *value)
{
    int index = hash(key);
    Node *previous = _nodes[index];
    Node *current = previous;
    previous->mutex.lock();

    if (previous->next != nullptr)
    {
        current = previous->next;
        current->mutex.lock();
        while (current->next != nullptr && current->key != key)
        {
            previous->mutex.unlock();
            previous = current;
            current = previous->next;
            current->mutex.lock();
        }
        previous->mutex.unlock();
    }

    if (current->key == key)
    {
        current->value = value;
        current->mutex.unlock();
    }
    else
    {
        Node *newNode = new Node();
        newNode->key = key;
        newNode->value = value;
        newNode->next = nullptr;
        current->next = newNode;
        current->mutex.unlock();
    }
}

void *HashTable::read(int key)
{
    int index = hash(key);
    Node *previous = _nodes[index];
    Node *current = previous;
    void *value = nullptr;
    previous->mutex.lock_shared();

    if (previous->next != nullptr)
    {
        current = previous->next;
        current->mutex.lock_shared();
        while (current->next != nullptr && current->key != key)
        {
            previous->mutex.unlock_shared();
            previous = current;
            current = previous->next;
            current->mutex.lock_shared();
        }
        previous->mutex.unlock_shared();
    }

    if (current->key == key)
    {
        current->value = value;
    }

    current->mutex.unlock_shared();
    return value;
}

void HashTable::remove(int key)
{
    int index = hash(key);
    Node *previous = _nodes[index];
    Node *current = previous;
    previous->mutex.lock();

    if (previous->next != nullptr)
    {
        current = previous->next;
        current->mutex.lock();
        while (current->next != nullptr && current->key != key)
        {
            previous->mutex.unlock();
            previous = current;
            current = previous->next;
            current->mutex.lock();
        }
    }

    if (current->key == key)
    {
        previous->next = current->next;
        delete current;
    }
    else
    {
        current->mutex.unlock();
    }

    previous->mutex.unlock();
}

HashTable::~HashTable()
{
    for (int i = 0; i < _size; i++)
    {
        Node *previous = _nodes[i];
        Node *current = previous;

        if (previous->next != nullptr)
        {
            current = previous->next;
            while (current->next != nullptr)
            {
                delete previous;
                previous = current;
                current = previous->next;
            }
        }

        delete current;
    }

    delete[] _nodes;
}