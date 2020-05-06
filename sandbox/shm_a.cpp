#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "tmp_vnc_client.h"

using namespace std;

int main(){
    FILE *fp;
    const string file_path = "./key.dat";
    fp = fopen(file_path.c_str(), "w");
    fclose(fp);

    // IPC key
    const int id = 50;
    const key_t key = ftok(file_path.c_str(), id);
    if(key == -1){
        cerr << "Failed to acquire key" << endl;
        return EXIT_FAILURE;
    }

    // shm id
    //const int size = 0x6400;
    const int size = sizeof(vnc_client);
    cout << "shm size: " << size << endl;
    const int seg_id = shmget(key, size,
                              IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    if(seg_id == -1){
        cerr << "Failed to acquire segment" << endl;
        return EXIT_FAILURE;
    }

    cout << "key: " << key << endl;
    cout << "seg_id: " << seg_id << endl;

    // attach to process
    cout << "vnc_client size: " << sizeof(vnc_client) << endl;
    void *shared_memory = shmat(seg_id, 0, 0);
    cout << "shaderd_memory: " << shared_memory << endl;

    //T*  ptr =  new ( adr ) T();
    vnc_client *v = new (shared_memory) vnc_client();
    cout << "v: " << v << endl;

    string localhost = "127.0.0.1";
    v->set_host(localhost);
    string host = v->get_host();
    cout << "host: " << host << endl;

    v->set_host_c(localhost.c_str(), localhost.length());
    char host_c[BUF_SIZE] = {};
    v->get_host_c(host_c);
    cout << "host_c: " << host_c << endl;

    v->set_port(6623);
    int port = v->get_port();
    cout << "port: " << port << endl;

    // to wait other process
    string s;
    int flag = 0;
    if (!v->connect_to_server()) {
        cout << "failed to connect_to_server" << endl;
        goto fin;
    }

    cout << "sockfd: " << v->get_sockfd() << endl;

    if (!v->recv_protocol_version()) {
        cout << "failed to recv_protool_version" << endl;
        goto fin;
    }

    // wait another process
    cout << "if you want to close, please type 'q'" << endl;
    while (flag == 0) {
        //cout << "word: ";
        cin >> s;
        if (s == "q") {
            flag = 1;
        } else {
            //sprintf(shared_memory, s.c_str());
        }
    }
    /*
    if (!v->send_protocol_version()) {
        cout << "failed to send_protool_version" << endl;
        goto fin;
    }
    */

 fin:
    // detach from process
    shmdt(shared_memory);
    // free shared memory
    shmctl(seg_id, IPC_RMID, NULL);

    return 0;
}
