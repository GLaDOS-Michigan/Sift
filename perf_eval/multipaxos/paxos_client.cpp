#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <netinet/in.h> 
#include <sys/types.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "paxos_system.h"
#include "config.h"


struct timeval stTimeOut;

std::ofstream outfile ("latency.csv");
using namespace std; 

#define BUFLEN 1122

#define REQUEST_MSG 0
#define REPLY_MSG 1
#define DELEGATE_MSG 2
#define ACK_MSG 3
#define CLIENT_REQ 8

#define SET 1
#define GET 0
#define DELEGATE 0

fd_set rset; 

struct deser_err {
};


struct ivy_deser {
    virtual void  get(long long&) = 0;
    virtual void  get(std::string &) = 0;
    virtual void  getn(long long &res, int bytes) = 0;
    virtual void  open_list() = 0;
    virtual void  close_list() = 0;
    virtual bool  open_list_elem() = 0;
    virtual void  close_list_elem() = 0;
    virtual void  open_struct() = 0;
    virtual void  close_struct() = 0;
    virtual void  open_field(const std::string &) = 0;
    virtual void  close_field() = 0;
    virtual int   open_tag(const std::vector<std::string> &) {throw deser_err();}
    virtual void  close_tag() {}
    virtual void  end() = 0;
    virtual ~ivy_deser(){}
};

struct ivy_binary_deser : public ivy_deser {
    std::vector<char> inp;
    int pos;
    std::vector<int> lenstack;
    ivy_binary_deser(const std::vector<char> &inp) : inp(inp),pos(0) {}
    virtual bool more(unsigned bytes) {return inp.size() >= pos + bytes;}
    virtual bool can_end() {return pos == inp.size();}
    void get(long long &res) {
       getn(res,8);
    }
    void getn(long long &res, int bytes) {
        if (!more(bytes))
            throw deser_err();
        res = 0;
        for (int i = 0; i < bytes; i++)
            res = (res << 8) | (((long long)inp[pos++]) & 0xff);
    }
    void get(std::string &res) {
        while (more(1) && inp[pos]) {
            res.push_back(inp[pos++]);
        }
        if(!(more(1) && inp[pos] == 0))
            throw deser_err();
        pos++;
    }
    void open_list() {
        long long len;
        get(len);
        lenstack.push_back(len);
    }
    void close_list() {
        lenstack.pop_back();
    }
    bool open_list_elem() {
        return lenstack.back();
    }
    void close_list_elem() {
        lenstack.back()--;
    }
    void open_struct() {}
    void close_struct() {}
    virtual void  open_field(const std::string &) {}
    void close_field() {}
    int open_tag(const std::vector<std::string> &tags) {
        long long res;
        get(res);
        if (res >= tags.size())
            throw deser_err();
        return res;
    }
    void end() {
        if (!can_end())
            throw deser_err();
    }
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

void derializePaxosMsg(ivy_deser &inp, __strlit &returnval){
  inp.open_struct();
  inp.open_field("m_kind");
  	long long temp;
  	inp.getn(temp,8);
  inp.close_field();
  inp.open_field("m_round");
 		long long m_round;
   	inp.getn(m_round,8);
  inp.close_field();
  inp.open_field("m_inst");
		long long m_inst;
 		inp.getn(m_inst,8);
  inp.close_field();
  inp.open_field("m_node");
		long long m_node;
		inp.getn(m_node,8);
  inp.close_field();
  inp.open_field("m_val");
		__strlit val;
		inp.get(val);
  inp.close_field();
  inp.open_field("m_votemap");

    inp.open_struct();
    inp.open_field("maxr");
			long long maxr;
    	inp.getn(maxr,8);
    inp.close_field();
    inp.open_field("maxv");
			long long maxv;
  		inp.getn(maxv,8);
    inp.close_field();
    inp.close_struct();

		inp.close_field();
	inp.close_struct();
	returnval = val;
	
}



void serializePaxos(ivy_ser &res, int msgType, int round,int reqType, int data){
  res.open_struct();
  res.open_field("m_kind");
  // __ser<paxos_system::msg_kind>(res,t.m_kind);
    res.setn(msgType,8);
    res.close_field();
  
  res.open_field("m_round");
  // __ser<unsigned long long>(res,t.m_round);
    res.setn(round,8);
    res.close_field();
  
  res.open_field("m_inst");
  // __ser<unsigned long long>(res,t.m_inst);
    res.setn(0,8);
    res.close_field();
  res.open_field("m_node");
  // __ser<paxos_system::node>(res,t.m_node);
    res.setn(0,8);
    res.close_field();
  res.open_field("m_value");
  // __ser<__strlit>(res,t.m_value);
    res.setn(0,8);
    res.close_field();
  res.open_field("m_votemap");
  // __ser<paxos_system::votemap_seg>(res,t.m_votemap);
      res.open_struct();
      res.open_field("maxr");
      res.setn(0,8);
      res.close_field();
      res.open_field("maxv");
      res.setn(0,8);
      res.close_field();
      res.close_struct();
  
  res.close_field();
  res.close_struct();
  
}


int cnt = 0;
int missed = 0;
int currentReq = 0;
double totalLatency = 0;

void  set_dst_udp(int my_id, sockaddr_in &remaddr) {
  char *server = server_map.at(my_id);	/* (LOCATION OF NODE 0) */
	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(4990); // send port -- Default send to id 0 
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

    
}

int send_recv(ivy_binary_ser &res, int &fd, sockaddr_in &remaddr,int client, int req,int sendVal){
  stTimeOut.tv_sec = 0;
  stTimeOut.tv_usec = 750000;
  int receivedVal = -1;
  
  // cout << "SENDING "<< currentReq <<"\n";
  auto start = std::chrono::high_resolution_clock::now();
  
  int slen=sizeof(remaddr);
   if (sendto(fd, &res.res[0],res.res.size(), 0, (struct sockaddr *)&remaddr, slen)==-1) {
                  perror("sendto");
                  exit(1);
                }
                
     FD_SET(fd, &rset);
		 	
            int len=0;
                  socklen_t lenlen=4;
  #ifdef _WIN32
            if (getsockopt(fd,SOL_SOCKET,SO_RCVBUF,(char *)&len,&lenlen))
  #else
              if( setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &stTimeOut, sizeof(struct timeval)))
			        // if (getsockopt(fd,SOL_SOCKET,SO_RCVBUF,&len,&lenlen))
								
  #endif
                { perror("getsockopt failed"); exit(1); }
            std::vector<char> buf(len);
            int bytes;
						
					  // auto timeoutstart = std::chrono::high_resolution_clock::now();
						

            if ((bytes = recvfrom(fd,&buf[0],len,0,0,0)) < 0)
                { std::cerr << "retry\n";  
									missed = missed +1;
									return 0;}
									cnt = cnt +1;
                  auto finish = std::chrono::high_resolution_clock::now();
                  std::chrono::duration<double> elapsed = finish - start;
                  // auto ms = std::chrono::duration_cast< std::chrono::milliseconds >( finish - start );
                  totalLatency += elapsed.count();
									return 1;
 
                  
}



void sendrecvloop(int tid, int numOfNodes){
  
  
  struct sockaddr_in myaddr;
	int fd, i, slen=sizeof(myaddr);
	char buf[BUFLEN];	
	int recvlen;		
	char *server = server_map.at(0);	/* (LOCATION OF NODE 0) */

	/* create a socket */

	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");

	/* bind it to all local addresses and pick any port number */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(4942 + tid);

	if (::bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
    return;
	}       
  FD_ZERO(&rset); 
	
	
	
  struct sockaddr_in remaddr;
	int currentReq = 0;

  while(currentReq < totalReqs){
    currentReq = currentReq + 1;
    set_dst_udp(0, remaddr);
    ivy_binary_ser res;
    int value = (tid*100) + rand() % 100; // client requests are uniuqe based on TID
    if(currentReq%10000 == 0) {cout << "Client " << tid << "Req # " << currentReq << " :: " << totalReqs <<" \n";}
    serializePaxos(res,8,value,1,1);
    send_recv(res,fd,remaddr,tid,0,value);
    int recieved = 0;
		while(recieved < 1){
			recieved = send_recv(res,fd,remaddr,tid,0,value);

		}
  
     
  }
  close(fd);

  return;
  
}


int main(int argc, char** argv) 
{

  std::cout << "Starting Experiment with " << num_threads << " and total req = " <<totalReqs << "\n";
  int numOfNodes = stoi(argv[1]); // not used -- clients always query node 0 (leader)
  std::thread t[num_threads];
 
 //Launch a group of client threads
  auto start = std::chrono::high_resolution_clock::now();
  
 for (int i = 0; i < num_threads; ++i) {
     t[i] = std::thread(sendrecvloop, i, numOfNodes);
 }

 //Join client threads with the main thread
 for (int i = 0; i < num_threads; ++i) {
   cout << "join " << i << "\n";
     t[i].join();
 }
 

 auto finish = std::chrono::high_resolution_clock::now();
  cout << "total replies : " << cnt << "\n";

  
  long long duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count() << " :: dur " << duration << " \n";
  
  std::cout << "Throughput : "<< cnt << " : "<< cnt/elapsed.count() <<"\n";
  std::cout << "total latency : " << totalLatency << " :: avg = " << totalLatency/cnt << "\n"  ;
	
  return 0;
 
  
}
