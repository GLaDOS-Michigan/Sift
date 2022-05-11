#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/types.h>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include "config.h"
#include "raft_system.h"
 


struct timeval stTimeOut;

std::ofstream outfile ("latency.csv"); // not used currently
using namespace std; 

#define BUFLEN 1122
#define CLIENT_REQ 7


fd_set rset; 

// ----IVY de/serialization----
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



void serializeRaftMsg(ivy_ser &res, int msgType, string value){
  const __strlit val = value;
	res.open_struct();
  res.open_field("m_kind");
  //__ser<raft_system::msgkind>(res,t.m_kind);
    res.setn(msgType,8);
    res.close_field();
  res.open_field("m_ix");
  // __ser<int>(res,t.m_ix);
    res.setn(0,8);
    res.close_field();
		
  res.open_field("m_term");
  // __ser<int>(res,t.m_term);
    res.setn(0,8);
    res.close_field();
		
  res.open_field("m_node");
  // __ser<raft_system::node>(res,t.m_node);
    res.setn(0,8);
    res.close_field();
  res.open_field("m_val");
 // __ser<__strlit>(res,t.m_val);
    res.set(val);
    res.close_field();
   res.open_field("m_logt");
  // ___ser<int>(res,t.m_logt);
   res.setn(0,8);
   res.close_field();
	 
	 res.open_field("m_prevlogt");
	 //__ser<int>(res,t.m_prevlogt);
   res.setn(0,8);
   res.close_field();
	 
	 res.open_field("m_isrecover");
	 // __ser<bool>(res,t.m_isrecover);
	 res.set(false);
	 res.close_field();
	 res.close_struct();
  
}

void derializeRaftMsg(ivy_deser &inp, __strlit &returnval){
  inp.open_struct();
  inp.open_field("m_kind");
  	long long temp;
  	inp.getn(temp,8);
  inp.close_field();
  inp.open_field("m_ix");
		long long m_ix;
   	inp.getn(m_ix,8);
  inp.close_field();
  inp.open_field("m_term");
		long long m_term;
 		inp.getn(m_term,8);
  inp.close_field();
  inp.open_field("m_node");
		long long m_node;
		inp.getn(m_node,8);
  inp.close_field();
  inp.open_field("m_val");
		__strlit val;
		inp.get(val);
  inp.close_field();
  inp.open_field("m_logt");
		long long m_logt;
		inp.getn(m_logt,8);
  inp.close_field();
  inp.open_field("m_prevlogt");
		long long m_prevlogt;
		inp.getn(m_prevlogt,8);
  inp.close_field();
  inp.open_field("m_isrecover");
		long long m_isrecover;
		inp.getn(m_isrecover,8);
  inp.close_field();
  inp.close_struct();
	returnval = val;
	
}
// ----IVY de/serialization----


int clientRequestCnt = 0;
int missed = 0;
double totalLatency = 0;
bool DEBUG = false; // Change to true to see client side output

void  set_dst_udp(int my_id, sockaddr_in &remaddr) {
  char *server = server_map.at(my_id);	/* (LOCATION OF NODE 0) */
	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(4990+my_id); // send port -- Default send to id 0 -- hardcoded port
	if (inet_aton(server, &remaddr.sin_addr)==0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

    
}

int send_recv(ivy_binary_ser &res, int &fd, sockaddr_in &remaddr,int client, int req,int sendVal){
  stTimeOut.tv_sec = 2;
	stTimeOut.tv_usec = 0;  
  // std::cout << "client :" << client << " and total req = " <<res << "\n";
  auto start = std::chrono::high_resolution_clock::now();
  int receivedVal = -1;
	// while(sendVal != receivedVal){
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
  #endif
                { perror("getsockopt failed"); exit(1); }
            		std::vector<char> buf(len);
            		int bytes;

                if ((bytes = recvfrom(fd,&buf[0],len,0,0,0)) < 0)
                    { 
											missed = missed +1;
											return 0;
										}

									clientRequestCnt = clientRequestCnt +1;
                  auto finish = std::chrono::high_resolution_clock::now();
                  std::chrono::duration<double> elapsed = finish - start;
                  totalLatency += elapsed.count();
									return 1;
}



void sendrecvloop(int tid, int numOfNodes){
  
  
  struct sockaddr_in myaddr;
	int fd, i, slen=sizeof(myaddr);
	char buf[BUFLEN];	
	int recvlen;		
	char *server = server_map.at(1);	/* (LOCATION OF NODE 0) */

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
		if(DEBUG || currentReq%10000 == 0){cout << "Client " << tid << "Req # " << currentReq << " :: " << totalReqs <<" \n";}
    set_dst_udp(1, remaddr);
    ivy_binary_ser res;
    int value = (tid*100) + rand() % 100; // client requests are uniuqe based on TID
    serializeRaftMsg(res,7,std::to_string(value));
    // if(DEBUG){cout << "SENDING CLIENT : " << tid << "  -->" << value << " \n";}
    int recieved = 0;
		while(recieved < 1){
			recieved = send_recv(res,fd,remaddr,tid,0,value);
		}
		
		// if(DEBUG || currentReq%10000 == 0){cout << "Client " << tid << "Req # " << currentReq << " :: " << totalReqs <<" \n";}
		
     
  }
	if(DEBUG){cout << "done \n";}
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
   if(DEBUG || true){cout << "join client thread #" << i << "\n";}
   t[i].join();
 }
 
  auto finish = std::chrono::high_resolution_clock::now();
       
  //cout << "Experiment Finished: (number of resent requests =" << retry count << "\n";
  cout << "total replies : " << clientRequestCnt << "\n";
  long long duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
  std::chrono::duration<double> elapsed = finish - start;
  std::cout << "Elapsed time: " << elapsed.count() << " :: dur " << duration << " \n";
  std::cout << "Throughput : "<< clientRequestCnt << " : "<< clientRequestCnt/elapsed.count() <<"\n";
  std::cout << "total latency : " << totalLatency << " :: avg = " << totalLatency/clientRequestCnt << "\n"  ;
	
  return 0;
 
  
}
