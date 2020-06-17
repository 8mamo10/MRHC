#include "logger.h"

const std::string logger::filename = "/tmp/mrhc.log";
logger* logger::instance = NULL;
std::ofstream logger::logfile;
std::mutex logger::mtx;

logger::logger()
{
}

logger* logger::get_logger()
{
    std::unique_lock<std::mutex> lk(logger::mtx);
    if (instance == NULL) {
        instance = new logger();
        logfile.open(filename.c_str(), std::ios::out | std::ios::app);
    }
    if (!logfile) {
        std::cerr << "Error: " << strerror(errno);
    }
    return instance;
}

void logger::log(const std::string &message)
{
    std::unique_lock<std::mutex> lk(logger::mtx);
    logfile << message;
    logfile.flush();
}

void logger::logn(const std::string &message)
{
    std::unique_lock<std::mutex> lk(logger::mtx);
    logfile << message << "\n";
    logfile.flush();
}

void logger::log(const char *format, ...)
{
    std::unique_lock<std::mutex> lk(logger::mtx);
    char *message = NULL;
    va_list args;
    va_start(args, format);
    int length = vasprintf(&message, format, args);
    if (length < 0) {
        std::cerr << "Error: " << strerror(errno);
        return;
    }
    logfile << message;
    logfile.flush();
    free(message);
    va_end(args);
}

void logger::logn(const char *format, ...)
{
    std::unique_lock<std::mutex> lk(logger::mtx);
    char *message = NULL;
    va_list args;
    va_start(args, format);
    int length = vasprintf(&message, format, args);
    if (length < 0) {
        std::cerr << "Error: " << strerror(errno);
        return;
    }
    logfile << message << "\n";
    logfile.flush();
    free(message);
    va_end(args);
}

logger& logger::operator<<(const std::string &message)
{
    std::unique_lock<std::mutex> lk(logger::mtx);
    logfile << message << "\n";
    logfile.flush();
    return *instance;
}
