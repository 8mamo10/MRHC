
#ifndef __MRHC_COMMON_H__
#define __MRHC_COMMON_H__

#include <bits/stdc++.h>

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"

using namespace std;

#define BUF_SIZE 1024
// I'm not sure hot to know the index of the module.
// I saw the httpd.conf and count the order of `LoadModule mrhc_module`
#define MODULE_INDEX 25

#define log_access(msg) \
    apr_table_set(r->notes, "mrhc_log", ("[" + string(__FILE__) + ":" + to_string(__LINE__) + "] [" + string(__FUNCTION__) + "] "+ string(msg)).c_str());

#define log_error(msg) \
    ap_log_rerror(__FILE__, __LINE__, MODULE_INDEX, APLOG_NOTICE, OK, r, "[%s:%d] [%s] %s", __FILE__, __LINE__, __FUNCTION__, msg);

#define log_debug(msg) \
    cerr << "[" << __FILE__ << ":" << __LINE__ << "] [" << __FUNCTION__ << "] " << msg << endl;

#endif
