#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "tmp_vnc_client.h"

using namespace std;

int main(){
    // shared memory id
    const string file_path = "./key.dat";
    const int id = 50;

    const key_t key = ftok(file_path.c_str(), id);

    const int seg_id = shmget(key, 0, 0);
    if (seg_id == -1) {
        cerr << "Failed to acquire segment" << endl;
        return EXIT_FAILURE;
    }

    cout << "key: " << key << endl;
    cout << "seg_id: " << seg_id << endl;

    // attach to process
    //char* const shared_memory = reinterpret_cast<char*>(shmat(seg_id, 0, 0));
    void *shared_memory = shmat(seg_id, 0, 0);
    cout << "shared_memory: " << shared_memory << endl;
    vnc_client *v = (vnc_client*)shared_memory;
    //string host = v->get_host();
    //cout << "host: " << host << endl;
    int port = v->get_port();
    cout << "port: " << port << endl;
    /*
    if (!v->send_protocol_version()) {
        cout << "failed to send_protool_version" << endl;
    }
    */
    // read frm sahred memory
    // int flag = 0;
    // char c;
    // cout << "If you want to close, please type 'q'" << endl;
    // cout << "If you want to read the shared memory, push enter button." << endl;
    // while(flag == 0){
    //     cin.get(c);
    //     if(c == 'q') flag = 1;
    //     else printf("%s\n", shared_memory);
    // }

    // detach from process
    shmdt(shared_memory);

    return 0;
}
