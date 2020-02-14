#ifndef __MRHC_COMMON_H__
#define __MRHC_COMMON_H__

#include "httpd.h"
#include "http_config.h"
#include "http_log.h"
#include "http_protocol.h"

#define BUF_SIZE 1024
// I'm not sure hot to know the index of the module.
// I saw the httpd.conf and count the order of `LoadModule mrhc_module`
#define MODULE_INDEX 25

#endif
