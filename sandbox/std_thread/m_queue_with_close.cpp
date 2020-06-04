// g++ -Wall -pthread -std=c++11 m_queue_with_close.cpp
#include <condition_variable>
#include <exception>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

struct closed_exception: std::exception {};
struct aborted_exception: std::exception {};

class m_queue {
    static const int capacity = 10;
    std::queue<int> q;
    std::mutex mtx;
    std::condition_variable cv_nofull;
    std::condition_variable cv_noempty;
    bool closed = false;
    bool aborted = false;
public:
    void push(int data) {
        std::unique_lock<std::mutex> lk(mtx);
        cv_nofull.wait(lk, [&] {
                return (q.size() < capacity) || closed || aborted;
            });
        if (closed) {
            throw closed_exception();
        }
        if (aborted) {
            throw aborted_exception();
        }
        bool do_signal = q.empty();
        q.push(data);
        if (do_signal) {
            cv_noempty.notify_one();
        }
    }
    int pop(int &data) {
        std::unique_lock<std::mutex> lk(mtx);
        cv_noempty.wait(lk, [&]{
                return !q.empty() || (q.empty() && closed) || aborted;
            });
        if (q.empty() && closed) {
            return false; // closed queue
        }
        if (aborted) {
            throw aborted_exception();
        }
        bool do_signal = (q.size() == capacity);
        data = q.front();
        q.pop();
        if (do_signal) {
            cv_nofull.notify_one();
        }
        return true;
    }
    void close() {
        std::lock_guard<std::mutex> lk(mtx);
        closed = true;
        cv_nofull.notify_all();
        cv_noempty.notify_all();
    }
    void abort() {
        std::lock_guard<std::mutex> lk(mtx);
        aborted = true;
        cv_nofull.notify_all();
        cv_noempty.notify_all();
    }
};

/////

m_queue mq;

// void jammer()
// {
//     int v;
//     while ((v = mq.pop()) > 0) {
//         std::cout << "jammer:" << v << std::endl;
//     }
//     std::cout << "(EOD)" << std::endl;
// }

int main()
{
    std::thread th1([&]{
            try {
                for (int i = 1; i <= 100; i++) {
                    mq.push(i);
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                mq.close();
            } catch (closed_exception&) {
                std::cout << "closed queue" << std::endl;
            } catch (aborted_exception&) {
                std::cout << "abort producer" << std::endl;
            }
        });
    std::thread th2([&]{
            try {
                int v;
                while (mq.pop(v)) {
                    std::cout << "th2:" << v << std::endl;
                }
                std::cout << "(EOD)" << std::endl;
            } catch (aborted_exception&) {
                std::cout << "abort consumer" << std::endl;
            }
        });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //std::thread th3(jammer);
    mq.abort();
    th1.join();
    th2.join();
    //th3.join();
}
