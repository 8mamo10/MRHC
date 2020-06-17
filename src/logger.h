#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <bits/stdc++.h>
#include <mutex>

#define LOGGER logger::get_logger()

class logger
{
 public:
    void log(const std::string &message);
    void logn(const std::string &message);
    void log(const char *format, ...);
    void logn(const char *format, ...);
    logger& operator<<(const std::string &message);
    static logger* get_logger();
 private:
    logger();
    static const std::string filename;
    static logger *instance;
    static std::ofstream logfile;
    static std::mutex mtx;
};

#endif
