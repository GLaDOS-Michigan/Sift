#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "sht_system.h"
#include "config.h"


struct timeval stTimeOut;

std::ofstream outfile ("latency.csv");
using namespace std;

#define BUFLEN 1122

#define REQUEST_MSG 0
#define REPLY_MSG 1
#define DELEGATE_MSG 2
#define ACK_MSG 3
#define SERVER_MSG 4

#define SET 1
#define GET 0
#define DELEGATE 0

fd_set rset;

struct deser_err {
};

struct ivy_ser {
  virtual void  set(long long) = 0;
  virtual void  set(bool) = 0;
  virtual void  setn(long long inp, int len) = 0;
  virtual void  set(const std::string &) = 0;
  virtual void  open_list(int len) = 0;
  virtual void  close_list() = 0;
  virtual void  open_list_elem() = 0;
  virtual void  close_list_elem() = 0;
  virtual void  open_struct() = 0;
  virtual void  close_struct() = 0;
  virtual void  open_field(const std::string &) = 0;
  virtual void  close_field() = 0;
  virtual void  open_tag(int, const std::string &) {throw deser_err();}
  virtual void  close_tag() {}
  virtual ~ivy_ser(){}
};

struct ivy_binary_ser : public ivy_ser {
  std::vector<char> res;
  void setn(long long inp, int len) {

    for (int i = len-1; i >= 0 ; i--)
      res.push_back((inp>>(8*i))&0xff);

  }
  void set(long long inp) {
    setn(inp,sizeof(long long));
  }
  void set(bool inp) {
    set((long long)inp);
  }
  void set(const std::string &inp) {
    for (unsigned i = 0; i < inp.size(); i++)
      res.push_back(inp[i]);
    res.push_back(0);
  }
  void open_list(int len) {
    set((long long)len);
  }
  void close_list() {}
  void open_list_elem() {}
  void close_list_elem() {}
  void open_struct() {}
  void close_struct() {}
  virtual void  open_field(const std::string &) {}
  void close_field() {}
  virtual void  open_tag(int tag, const std::string &) {
    set((long long)tag);
  }
  virtual void  close_tag() {}
};


void  serialize(ivy_ser &res, int msgType, int reqType, int key, int data){
  res.open_struct();
  res.open_field("m_kind");
  // __ser<sht_system::msg_kind>(res,t.m_kind);
  res.setn(msgType,8);
  res.close_field();
  res.open_field("src");
  // __ser<unsigned>(res,t.src);
  res.setn(0,8);
  res.close_field();
  res.open_field("rq");
  // __ser<sht_system::req>(res,t.rq);
  res.open_struct();
  res.open_field("src");
  // __ser<unsigned>(res,t.src);
  res.setn(0,8);
  res.close_field();
  res.open_field("rid");
  // __ser<unsigned long long>(res,t.rid);
  res.setn(0,8);
  res.close_field();
  res.open_field("rkey");
  // __ser<unsigned>(res,t.rkey);
  res.setn(key,8);
  res.close_field();
  res.open_field("rtype");
  // __ser<sht_system::otype>(res,t.rtype);
  res.setn(reqType,8);
  res.close_field();
  res.open_field("rdata");
  // __ser<unsigned>(res,t.rdata);
  res.setn(data,8);
  res.close_field();
  res.close_struct();

  // res.setn(1,8);
  res.close_field();
  res.open_field("num");
  // __ser<unsigned>(res,t.num);
  res.setn(0,8);
  res.close_field();
  res.open_field("sh");
  // __ser<sht_system::shard>(res,t.sh);

  res.open_struct();
  res.open_field("skey");
  // __ser<unsigned>(res,t.skey);
  res.setn(0,8);
  res.close_field();
  res.open_field("sdata");
  // __ser<unsigned>(res,t.sdata);
  res.setn(0,8);
  res.close_field();
  res.close_struct();

  res.close_field();
  res.close_struct();
}

int cnt = 0;
int missed = 0;
long long msgTime = 0;
int totalReq = 0;

//added
double totalLatency = 0;

void send_recv(ivy_binary_ser &res, int &fd, sockaddr_in &remaddr,int client, int req){
  stTimeOut.tv_sec = 1;
  stTimeOut.tv_usec = 5;

  // cout << "SENDING "<< totalReq <<"\n";
  auto start = std::chrono::high_resolution_clock::now();

  int slen=sizeof(remaddr);
  if (sendto(fd, &res.res[0],res.res.size(), 0, (struct sockaddr *)&remaddr, slen)==-1) {
    perror("sendto");
    exit(1);
  }

  FD_SET(fd, &rset);

  // printf("waiting on port %d\n", SERVER_PORT);
  int len=0;
  socklen_t lenlen=4;
                #ifdef _WIN32
  if (getsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char *)&len,&lenlen))
                  #else
    // if (getsockopt(fd,SOL_SOCKET,SO_RCVBUF,&len,&lenlen))
    if( setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &stTimeOut, sizeof(struct timeval)))
                    #endif
      { perror("getsockopt failed"); exit(1); }
  std::vector<char> buf(len);
  int bytes;

  if ((bytes = recvfrom(fd,&buf[0],len,0,0,0)) < 0)
    { std::cerr << "recvfrom failed\n";  missed = missed +1;}

  // std :: cout << cnt <<" :: REPLY RECIEVED : Client " <<client << " , " << req << "\n";
  cnt = cnt +1;
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = finish - start;
  // msgTime = msgTime +elapsed.count();
  // outfile << elapsed.count() <<",\n";
  totalLatency += elapsed.count();
  // std::cout << "Msg(" << cnt <<") Elapsed time: " << elapsed.count() << " total =" << msgTime <<"\n";
}
void  set_dst_udp(int my_id, sockaddr_in &remaddr) {
  char *server = server_map.at(my_id);/* (LOCATION OF NODE 0) */
  memset((char *) &remaddr, 0, sizeof(remaddr));
  remaddr.sin_family = AF_INET;
  remaddr.sin_port = htons(4990+my_id); // send port -- Default send to id 0
  if (inet_aton(server, &remaddr.sin_addr)==0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }
}





void sendrecvloop(int tid, int numOfNodes,int fd){


  struct sockaddr_in remaddr;
  while(totalReq < totalReqs){
    // std::this_thread::sleep_for(std::chrono::milliseconds(25));
    int dst = rand() % numOfNodes;
    set_dst_udp(dst, remaddr);
    ivy_binary_ser res;

    int opChance = rand() % 100;
    int randomKey = rand() % num_keys; //out if 100
    string op;
    if(opChance > 90){
      int randomValue = rand() % 100; //out if 100
      op = "set("+ std::to_string(randomKey) + "," + std::to_string(randomValue)+ ")";
      auto startserialize= std::chrono::high_resolution_clock::now();

      serialize(res,SERVER_MSG,SET,randomKey,randomValue);

      auto finishserialize = std::chrono::high_resolution_clock::now();
      std::chrono::duration<double> elapsed = finishserialize - startserialize;
      int sizeX = sizeof(struct ivy_binary_ser);
      send_recv(res,fd,remaddr,tid,0);
    }else{
      op = "get(" + std::to_string(randomKey) + ")";
      serialize(res,SERVER_MSG,GET,randomKey,0);

      send_recv(res,fd,remaddr,tid,0);
    }
    totalReq = totalReq + 1;

  }


}



int main(int argc, char** argv)
{
  struct sockaddr_in myaddr;
  int fd, i, slen=sizeof(myaddr);
  char buf[BUFLEN];
  int recvlen;
  char *server = server_map.at(-1);/* (LOCATION OF NODE 0) */

  /* create a socket */

  if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    printf("socket created\n");

  /* bind it to all local addresses and pick any port number */

  memset((char *)&myaddr, 0, sizeof(myaddr));
  myaddr.sin_family = AF_INET;
  myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  myaddr.sin_port = htons(4942);

  if (::bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
    perror("bind failed");
    // return 0;
  }
  FD_ZERO(&rset);

  int numOfNodes = stoi(argv[1]);
  // thread th1(abc, fd);
  //UDP SOCET CONFIG
  std::thread t[num_threads];
  //Launch a group of threads
  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < num_threads; ++i) {
    t[i] = std::thread(sendrecvloop, i, numOfNodes,fd);
  }
  //Join the threads with the main thread
  for (int i = 0; i < num_threads; ++i) {
    cout << "join " << i << "\n";
    t[i].join();
  }


  auto finish = std::chrono::high_resolution_clock::now();

  close(fd);
  cout << "total time : " << msgTime << " - cnt : " << cnt << "average = " << msgTime/cnt << "\n";
  cout << "total latency : " << totalLatency << " :: avg = " << totalLatency/cnt << "\n"  ;

  long long duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();

  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count() << " :: dur " << duration << " s\n";
  std::cout << "Throughput : "<< totalReq<< " : "<< totalReq/duration ;

  return 0;





}
