#include "fdtransport.h"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

int main(void)
{
    int gate = listen_gate("./local.sock");
    pid_t msg;  /* pidをメッセージとして受け取る */
    int fd = recvfd(gate, &msg, sizeof(msg));
    printf("file descriptor %d from pid %d\n", fd, msg);

    /* 標準入力 → 受け取ったファイルディスクリプタ */
    /* 正しいwrite(2)の使い方：http://kzk9.net/column/write.html */
    char buf[BUFSIZ];
    ssize_t len;
    while( (len = read(0, buf, sizeof(buf))) > 0 ) {
        const char *p = buf;
        const char * const endp = p + len;
        do {
            const ssize_t nbytes = write(fd, p, endp - p);
            if( nbytes < 0 ) {
                if( errno == EINTR || errno == EAGAIN ) {
                    continue;
                } else {
                    goto end;
                }
            }
            p += nbytes;
        } while (p < endp);
    }
 end:
    perror("end");
    unlink("./local.sock");
    return 0;
}
