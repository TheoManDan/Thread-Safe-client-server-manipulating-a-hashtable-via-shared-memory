TARGET=client server 
CXX=g++
RM=rm -f
CXXFLAGS=-std=c++20 -Wall -pedantic -pthread

all: $(TARGET)

client: client.cpp shared_memory.o
	$(CXX) $(CXXFLAGS) -o client client.cpp shared_memory.o

server: server.cpp shared_memory.o hash_table.o
	$(CXX) $(CXXFLAGS) -o server server.cpp  shared_memory.o hash_table.o

shared_memory.o: shared_memory.cpp shared_memory.h
	$(CXX) $(CXXFLAGS) -c shared_memory.cpp -o shared_memory.o

hash_table.o: hash_table.cpp hash_table.h
	$(CXX) $(CXXFLAGS) -c hash_table.cpp -o hash_table.o

clean:
	$(RM) $(TARGET) *.o