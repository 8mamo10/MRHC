#ifndef __MRHC_COMMON_H__
#define __MRHC_COMMON_H__

#include <bits/stdc++.h>

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"

#include "logger.h"

#define BUF_SIZE 1024
// I'm not sure hot to know the index of the module.
// I saw the httpd.conf and count the order of `LoadModule mrhc_module`
#define MODULE_INDEX 25

#define log_access(msg) \
    apr_table_set(r->notes, "mrhc_log", ("[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "] [" + std::string(__FUNCTION__) + "] "+ std::string(msg)).c_str());

#define log_error(msg) \
    ap_log_rerror(__FILE__, __LINE__, MODULE_INDEX, APLOG_NOTICE, OK, r, "[%s:%d] [%s] %s", __FILE__, __LINE__, __FUNCTION__, msg);

#define log_debug(msg) \
    std::cerr << "[" << __FILE__ << ":" << __LINE__ << "] [" << __FUNCTION__ << "] " << msg << std::endl;

#define log_xdebug(msg, len)                             \
    mrhc_log = "";                                       \
    for (int i = 0; i < len; i++) {                      \
        char aznable[BUF_SIZE];                          \
        sprintf(aznable, "0x%02X ", msg[i]);             \
        mrhc_log += aznable;                             \
    }                                                    \
    log_debug(mrhc_log);

#define DEBUG(msg) LOGGER->logn("[%s][%s:%d][%s] %s", current_datetime().c_str(), __FILE__, __LINE__, __FUNCTION__, msg)

#define DEBUGX(msg, len)                        \
    for (int i = 0; i < len; i++) {             \
        LOGGER->log("0x%02x ", msg[i]);         \
    }                                           \
    LOGGER->log("\n");                          \

static const std::string current_datetime()
{
    time_t     now = time(NULL);
    struct tm  local;
    char       buf[80];
    localtime_r(&now, &local);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &local);
    return buf;
}

#endif
