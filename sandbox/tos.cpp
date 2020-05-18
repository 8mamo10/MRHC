#include <iostream>
#include <netinet/ip.h>
#include <arpa/inet.h>

using namespace std;

int main() {
    cout << "IP_TOS: " << IP_TOS << endl;

    cout << "IPTOS_LOWDELAY: " << IPTOS_LOWDELAY << endl;
    cout << "IPTOS_THROUGHPUT: " << IPTOS_THROUGHPUT << endl;
    cout << "IPTOS_LOWCOST: " << IPTOS_LOWCOST << endl;
    cout << "IPTOS_MINCOST: " << IPTOS_MINCOST << endl;

    cout << "IPTOS_TOS(IPTOS_LOWDELAY): " << IPTOS_TOS(IPTOS_LOWDELAY) << endl;
    cout << "IPTOS_TOS(IPTOS_THROUGHPUT): " << IPTOS_TOS(IPTOS_THROUGHPUT) << endl;
    cout << "IPTOS_TOS(IPTOS_LOWCOST): " << IPTOS_TOS(IPTOS_LOWCOST) << endl;
    cout << "IPTOS_TOS(IPTOS_MINCOST): " << IPTOS_TOS(IPTOS_MINCOST) << endl;

    int sockfd = 0;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "failed to socket" << endl;
        return false;
    }

    int tos = 0x08;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TOS,  &tos, sizeof(tos))) {
        cout << "failed to setsockopt" << endl;
        return false;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = inet_addr("192.168.1.7");
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        cout << "failed to connect" << endl;
        return false;
    }
    string s = "teststring";
    int length = send(sockfd, s.c_str(), s.length(), 0);
    if (length < 0) {
        cout << "failed to send" << endl;
        return false;
    }
    cout << "send: " << length << endl;
    return true;
}
