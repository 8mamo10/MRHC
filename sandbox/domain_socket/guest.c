#include "fdtransport.h"
#include <stdio.h>

int main(void)
{
    int gate = connect_gate("./local.sock");
    pid_t msg = getpid();  /* pidをメッセージとして送る */
    sendfd(gate, 1, &msg, sizeof(msg));
    perror("end");
    return 0;
}
