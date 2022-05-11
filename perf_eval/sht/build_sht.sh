#!/bin/bash -x

#SHT
echo
echo $"compiling sht to cpp"
cd ../sht
ivy_to_cpp target=repl isolate=impl sht_system.ivy

sed -i '590 i class udp_reader;' sht_system.h
sed -i  '/enum msg_kind/c\enum msg_kind{msg_kind__request_t,msg_kind__reply_t,msg_kind__delegate_t,msg_kind__ack_t,msg_kind__server_req_t};' sht_system.h
sed -i '705 i udp_reader *net__impl__rdr;' sht_system.h
sed -i '767 i virtual void ext__trans__handle_serverRequest(const req& rq);' sht_system.h

sed -i '32 i #include "../perf_eval/config.h"' sht_system.cpp
sed -i '33 i #include<unistd.h>' sht_system.cpp
sed -i '793i myaddr.sin_addr.s_addr = inet_addr(server_map.at(my_id)); // added to read from config.h' sht_system.cpp
sed -i '203r ../perf_eval/test.txt' sht_system.cpp
sed -i '831i         thunk__net__impl__handle_recv rccb;' sht_system.cpp
sed -i '835s/const thunk__net__tcp__impl__handle_connected ccb)/const thunk__net__tcp__impl__handle_connected ccb,/' sht_system.cpp
sed -i '836i const thunk__net__impl__handle_recv rccb)' sht_system.cpp
sed -i '837i : acb(acb), rcb(rcb), fcb(fcb), ccb(ccb), rccb(rccb) {}' sht_system.cpp
sed -i '838d' sht_system.cpp
sed -i '839r ../perf_eval/sht_send_to_client.txt' sht_system.cpp
sed -i '1264,1333d' sht_system.cpp
sed -i '1263r ../perf_eval/sht/net_rec_with_client.txt' sht_system.cpp
sed -i '1586r ../perf_eval/sht/sht_handle_serverReq.txt' sht_system.cpp
sed -i '1852r ../perf_eval/sht/clientMsg.txt' sht_system.cpp
#sed -i '1276r ../perf_eval/handleServerReq.txt' sht_system.cpp
#sed -i '2034 i if (t == sht_system::msg_kind__server_req_t) s<<"server_req_t";' sht_system.cpp
#sed -i '2460 i if(arg.atom == "server_req_t") return sht_system::msg_kind__server_req_t;' sht_system.cpp
g++ -std=c++11 -o sht_server sht_system.cpp -pthread



