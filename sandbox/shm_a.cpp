#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

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

    // attach to process
    cout << "vnc_client size: " << sizeof(vnc_client) << endl;
    void *shared_memory = shmat(seg_id, 0, 0);
    //T*  ptr =  new ( adr ) T();
    vnc_client *ptr = new (shared_memory) vnc_client();
    string host = ptr->get_host();
    cout << "host: " << host << endl;

    // // write to shared memory
    // string s;

    // int flag = 0;
    // cout << "if you want to close, please type 'q'" << endl;
    // while(flag == 0){
    //     cout << "word: ";
    //     cin >> s;
    //     if(s == "q") flag = 1;
    //     else {
    //         //sprintf(shared_memory, s.c_str());
    //     }
    // }

    if (!ptr->connect_to_server()) {
        cout << "failed to connect_to_server" << endl;
        shmdt(shared_memory);
        shmctl(seg_id, IPC_RMID, NULL);
        return false;
    }
    // detach from process
    shmdt(shared_memory);
    // free shared memory
    shmctl(seg_id, IPC_RMID, NULL);

    return 0;
}
