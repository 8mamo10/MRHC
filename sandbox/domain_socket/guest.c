#include "fdtransport.h"
#include <stdio.h>

#include "tmp_vnc_client.h"
#include <string>

using namespace std;

int main(void)
{
    int gate = connect_gate("./local.sock");
    pid_t msg = getpid();  /* pidをメッセージとして送る */

    // set up vnc client
    vnc_client *v = new vnc_client();
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

    if (!v->connect_to_server()) {
        cout << "failed to connect_to_server" << endl;
        delete v;
        return -1;
    }
    cout << "sockfd: " << v->get_sockfd() << endl;

    if (!v->recv_protocol_version()) {
        cout << "failed to recv_protool_version" << endl;
        delete v;
        return -1;
    }

    //
    //sendfd(gate, 1, &msg, sizeof(msg));
    sendfd(gate, v->get_sockfd(), &msg, sizeof(msg));
    perror("end");
    return 0;
}
