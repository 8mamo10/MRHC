/* 
**  mod_mrhc.c -- Apache sample mrhc module
**  [Autogenerated via ``apxs -n mrhc -g'']
**
**  To play with this sample module first compile it into a
**  DSO file and install it into Apache's modules directory 
**  by running:
**
**    $ apxs -c -i mod_mrhc.c
**
**  Then activate it in Apache's httpd.conf file for instance
**  for the URL /mrhc in as follows:
**
**    #   httpd.conf
**    LoadModule mrhc_module modules/mod_mrhc.so
**    <Location /mrhc>
**    SetHandler mrhc
**    </Location>
**
**  Then after restarting Apache via
**
**    $ apachectl restart
**
**  you immediately can request the URL /mrhc and watch for the
**  output of this module. This can be achieved for instance via:
**
**    $ lynx -mime_header http://localhost/mrhc 
**
**  The output should be similar to the following one:
**
**    HTTP/1.1 200 OK
**    Date: Tue, 31 Mar 1998 14:42:22 GMT
**    Server: Apache/1.3.4 (Unix)
**    Connection: close
**    Content-Type: text/html
**  
**    The sample page from mod_mrhc.c
*/ 

#include "httpd.h"
#include "http_config.h"
#include "http_protocol.h"
#include "ap_config.h"

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern "C" module AP_MODULE_DECLARE_DATA mrhc_module;

/* The sample content handler */
static int mrhc_handler(request_rec *r)
{
    if (strcmp(r->handler, "mrhc")) {
        return DECLINED;
    }
    r->content_type = "text/html";      

    if (r->header_only) {
        return DECLINED;
    }
    const char *username;
    const char *password;
    apr_status_t ret = ap_get_basic_auth_components(r, &username, &password);
    if (ret == APR_EINVAL) {
        apr_table_set(r->err_headers_out, "WWW-Authenticate", "Basic real=\"\"");
        return HTTP_UNAUTHORIZED;
    }
    if (ret == APR_SUCCESS) {
        char str[256];
        sprintf(str, "username: %s", username);
        ap_rputs(str, r);
        ap_rputs("<br/>", r);
        sprintf(str, "password: %s", password);
        ap_rputs(str, r);
        ap_rputs("<br/>", r);
        std::string hoge = "test string";
        ap_rputs(hoge.c_str(), r);
        ap_rputs("<br/>", r);

        // socket test
        int sockfd;
        struct sockaddr_in addr;
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            ap_rputs("failed to open socket", r);
            return OK;
        }
        addr.sin_family = AF_INET;
        addr.sin_port = htons(5900);
        addr.sin_addr.s_addr = inet_addr("192.168.1.14");
        connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
        char recv_str[1024];
        int recv_len = recv(sockfd, recv_str, 1024, 0);
        std::string output = recv_str;
        ap_rputs("<br/>", r);
        ap_rputs(output.substr(0, recv_len).c_str(), r);
        ap_rputs("<br/>", r);
        //
        return OK;
    }
    ap_rputs("not reach here\n", r);
    return OK;
}

static void mrhc_register_hooks(apr_pool_t *p)
{
    ap_hook_handler(mrhc_handler, NULL, NULL, APR_HOOK_MIDDLE);
}

extern "C" {
    /* Dispatch list for API hooks */
    module AP_MODULE_DECLARE_DATA mrhc_module = {
        STANDARD20_MODULE_STUFF,
        NULL,                  /* create per-dir    config structures */
        NULL,                  /* merge  per-dir    config structures */
        NULL,                  /* create per-server config structures */
        NULL,                  /* merge  per-server config structures */
        NULL,                  /* table of config file commands       */
        mrhc_register_hooks  /* register hooks                      */
    };
};
