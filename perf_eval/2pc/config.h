#include <map>
#include <string>
#include <iterator>
#include <arpa/inet.h>
#include <chrono>
#include <thread>

#define SERVER_PORT 4942 


const std::map<int, char*> server_map = {
  { -1, "127.0.0.1" },  // client threads
  { 0, "127.0.0.1" } , // ex. node #0 
  { 1, "127.0.0.1" },   // ex. node #1
  { 2, "127.0.0.1" }   // ex node  #2 
};

static const int num_threads = 1;
static const int totalReqs = 200;

//only used in SHT
static const int num_keys = 1000;

/////////
