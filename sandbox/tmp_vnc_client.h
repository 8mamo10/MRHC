#include <string>

class vnc_client
{
public:
    int sockfd;
    std::string host;
    int port;
    std::string password;

    vnc_client() {
        this->sockfd = 0;
        this->host = "";
        this->port = 0;
        this->password = "";
    };
    bool connect_to_server()
    {
        if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            return false;
        }
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(this->port);
        addr.sin_addr.s_addr = inet_addr(this->host.c_str());
        if (connect(this->sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
            return false;
        }
        return true;
    }
};
