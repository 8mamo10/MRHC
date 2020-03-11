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

#include <unistd.h>
#include <vector>

#include "ap_config.h"

#include "mrhc_common.h"
#include "vnc_client.h"

extern "C" module AP_MODULE_DECLARE_DATA mrhc_module;

static apr_status_t ap_get_vnc_param_by_basic_auth_components(const request_rec *r, const char **host, int *port, const char **password);
static std::vector<std::string> split_string(std::string s, std::string delim);

// TODO: Need to support multi process but only support single process for now
vnc_client *client_cache = NULL;

/* The sample content handler */
static int mrhc_handler(request_rec *r)
{
    log_access("called");
    log_error("called");
    log_debug("called");

    if (strcmp(r->handler, "mrhc")) {
        return DECLINED;
    }
    if (r->header_only) {
        return DECLINED;
    }

    if (client_cache != NULL) {
        log_debug("VNC Client is already running.");
        client_cache->clear_buf();

        if (r->parsed_uri.query) {
            std::vector<std::string> pointer_params = split_string(r->parsed_uri.query, "&");
            uint16_t x = 0;
            uint16_t y = 0;
            uint8_t button = 0;
            for (unsigned int i = 0; i < pointer_params.size(); i++) {
                log_debug(pointer_params[i]);
                std::vector<std::string> params = split_string(pointer_params[i], "=");
                if (params[0] == std::string("x")) x = stoi(params[1]);
                if (params[0] == std::string("y")) y = stoi(params[1]);
                if (params[0] == std::string("b")) button = stoi(params[1]);
            }
            if (!client_cache->send_pointer_event(x, y, button)) {
                ap_rputs("Failed to send_pointer_event.", r);
                return OK;
            }
            // for emulating double click
            // if (!client_cache->send_pointer_event(x, y, button)) {
            //     ap_rputs("Failed to send_pointer_event.", r);
            //     return OK;
            // }
            // wait for pointer event to be reflected on the screen
            sleep(1);
        }
        if (!client_cache->send_frame_buffer_update_request()) {
            ap_rputs("Failed to send_frame_buffer_update_request.", r);
            return OK;
        }
        if (!client_cache->recv_frame_buffer_update()) {
            ap_rputs("Failed to recv_frame_buffer_update.", r);
            return OK;
        }

        // output image
        if (!client_cache->draw_image()) {
            ap_rputs("Failed to draw_image.", r);
            return OK;
        }
        std::vector<uint8_t> jpeg_buf = client_cache->get_jpeg_buf();
        log_debug("jpeg size:" + std::to_string(jpeg_buf.size()));
        char jpeg[jpeg_buf.size()] = {};
        for (unsigned int i = 0; i < jpeg_buf.size(); i++) {
            jpeg[i] = jpeg_buf[i];
        }
        r->content_type = "image/jpeg";
        ap_rwrite(jpeg, jpeg_buf.size(), r);
        return OK;
    }

    const char *host;
    int port = 0;
    const char *password;
    //apr_status_t ret = ap_get_basic_auth_components(r, &username, &password);
    apr_status_t ret = ap_get_vnc_param_by_basic_auth_components(r, &host, &port, &password);
    if (ret == APR_EINVAL) {
        apr_table_set(r->err_headers_out, "WWW-Authenticate", "Basic real=\"\"");
        return HTTP_UNAUTHORIZED;
    }
    if (ret == APR_SUCCESS) {
        log_debug("host: " + std::string(host));
        log_debug("port: " + std::to_string(port));
        log_debug("password: " + std::string(password));

        log_debug("Start VNC Client");
        vnc_client *client = new vnc_client(host, port, password);
        if (!client->connect_to_server()) {
            ap_rputs("Failed to connect_to_server.", r);
            return OK;
        }
        log_debug("Connected");
        // protocol version
        if (!client->recv_protocol_version()) {
            ap_rputs("Failed to recv_protocol_version.", r);
            return OK;
        }
        if (!client->send_protocol_version()) {
            ap_rputs("Failed to send_protocol_version.", r);
            return OK;
        }
        log_debug("Exchanged protocol version");
        // security type
        if (!client->recv_supported_security_types()) {
            ap_rputs("Failed to recv_supported_security_types.", r);
            return OK;
        }
        if (!client->send_security_type()) {
            ap_rputs("Failed to send_security_type.", r);
            return OK;
        }
        log_debug("Exchanged security type");
        // vnc auth
        if (!client->recv_vnc_auth_challenge()) {
            ap_rputs("Failed to recv_vnc_auth_challenge.", r);
            return OK;
        }
        if (!client->send_vnc_auth_response()) {
            ap_rputs("Failed to send_vnc_auth_response.", r);
            return OK;
        }
        if (!client->recv_security_result()) {
            ap_rputs("Failed to recv_security_result.", r);
            return OK;
        }
        log_debug("VNC authenticated");
        // client/server init
        if (!client->send_client_init()) {
            ap_rputs("Failed to send_client_init.", r);
            return OK;
        }
        if (!client->recv_server_init()) {
            ap_rputs("Failed to recv_server_init.", r);
            return OK;
        }
        log_debug("Exchanged Client/Server Init");
        // frame buffer update
        if (!client->send_set_pixel_format()) {
            ap_rputs("Failed to send_set_pixel_format.", r);
            return OK;
        }
        if (!client->send_set_encodings()) {
            ap_rputs("Failed to send_set_encodings.", r);
            return OK;
        }
        // return initial html page with url and image size
        r->content_type = "text/html";
        std::string hostname = r->hostname;
        std::string path = r->unparsed_uri;
        std::string width = std::to_string(client->get_width());
        std::string height = std::to_string(client->get_height());
        std::string html ="\
<html>                                                                  \
  <body>                                                                \
    <image id='mrhc' src='http://" + hostname + path + "' width='" + width + "' height='" + height + "'> \
  </body>                                                               \
</html>                                                                 \
<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js'></script> \
<script type=text/javascript>                                           \
  $('#mrhc').on('click', (e) => {                                       \
    $('#mrhc').attr('src', 'http://" + hostname + path + "?x=' + e.offsetX + '&y=' + e.offsetY + '&b=0'); \
  }).on('contextmenu', (e) => {                                         \
    $('#mrhc').attr('src', 'http://" + hostname + path + "?x=' + e.offsetX + '&y=' + e.offsetY + '&b=2'); \
    return false                                                        \
  });                                                                   \
</script>";
        log_debug(html.c_str());
        ap_rputs(html.c_str(), r);
        client_cache = client;
        return OK;
    }
    ap_rputs("not reach here", r);
    return OK;
}

// Since I want to use `:` as a part of username of basic auth, I reinvent a function.
// see: httpd-2.4.41/server/protocol.c
static apr_status_t ap_get_vnc_param_by_basic_auth_components(const request_rec *r, const char **host, int *port, const char **password)
{
    const char *auth_header;
    const char *credentials;
    const char *decoded;
    //const char *user;

    auth_header = (PROXYREQ_PROXY == r->proxyreq) ? "Proxy-Authorization"
                                                  : "Authorization";
    credentials = apr_table_get(r->headers_in, auth_header);

    if (!credentials) {
        /* No auth header. */
        return APR_EINVAL;
    }

    if (ap_cstr_casecmp(ap_getword(r->pool, &credentials, ' '), "Basic")) {
        /* These aren't Basic credentials. */
        return APR_EINVAL;
    }

    while (*credentials == ' ' || *credentials == '\t') {
        credentials++;
    }

    /* XXX Our base64 decoding functions don't actually error out if the string
     * we give it isn't base64; they'll just silently stop and hand us whatever
     * they've parsed up to that point.
     *
     * Since this function is supposed to be a drop-in replacement for the
     * deprecated ap_get_basic_auth_pw(), don't fix this for 2.4.x.
     */
    decoded = ap_pbase64decode(r->pool, credentials);

    // vnc host is to be like 192.168.1.10:5900.
    //user = ap_getword_nulls(r->pool, &decoded, ':');
    std::vector<std::string> vnc_params = split_string(decoded, ":");

    if (host) {
        *host = vnc_params[0].c_str();
    }
    if (port) {
        *port = stoi(vnc_params[1]);
    }
    if (password) {
        *password = vnc_params[2].c_str();
    }

    return APR_SUCCESS;
}

static std::vector<std::string> split_string(std::string s, std::string delim)
{
    std::vector<std::string> v;
    while (true) {
        size_t i = s.find_first_of(delim);
        if (i == std::string::npos) {
            v.push_back(s);
            break;
        }
        std::string item = s.substr(0, i);
        v.push_back(item);
        s = s.substr(i+1, s.size());
    }
    return v;
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
