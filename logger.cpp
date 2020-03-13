#include "logger.h"

const std::string logger::filename = "/usr/local/apache2/logs/mrhc.log";
logger* logger::instance = NULL;
std::ofstream logger::logfile;

const std::string current_datetime()
{
    time_t     now = time(NULL);
    struct tm  local;
    char       buf[80];
    localtime_r(&now, &local);
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &local);
    return buf;
}

logger::logger()
{
}

logger* logger::get_logger()
{
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
    logfile << "[" << current_datetime() << "]" << message << "\n";
    logfile.flush();
}

void logger::log(const char *format, ...)
{
    char *message = NULL;
    va_list args;
    va_start(args, format);
    int length = vasprintf(&message, format, args);
    if (length < 0) {
        std::cerr << "Error: " << strerror(errno);
        return;
    }
    logfile << "[" << current_datetime() << "]" << message << "\n";
    logfile.flush();
    free(message);
    va_end(args);
}

logger& logger::operator<<(const std::string &message)
{
    logfile << "[" << current_datetime() << "]" << message << "\n";
    logfile.flush();
    return *instance;
}
