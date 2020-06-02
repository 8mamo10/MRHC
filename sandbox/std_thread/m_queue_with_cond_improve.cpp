// g++ -Wall -pthread -std=c++11 m_queue_with_cond.cpp
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class m_queue {
    static const int capacity = 10;
    std::queue<int> q;
    std::mutex mtx;
    std::condition_variable cv_nofull;
    std::condition_variable cv_noempty;
public:
    void push(int data) {
        std::unique_lock<std::mutex> lk(mtx);
        cv_nofull.wait(lk, [&] {
                return (q.size() < capacity);
            });
        bool do_signal = q.empty();
        q.push(data);
        if (do_signal) {
            cv_noempty.notify_one();
        }
    }
    int pop() {
        std::unique_lock<std::mutex> lk(mtx);
        cv_noempty.wait(lk, [&]{
                return !q.empty();
            });
        bool do_signal = (q.size() == capacity);
        int data = q.front();
        q.pop();
        if (do_signal) {
            cv_nofull.notify_one();
        }
        return data;
    }
};

/////
int main()
{
    m_queue mq;
    std::thread th1([&]{
            for (int i = 1; i <= 1000; i++) {
                mq.push(i);
            }
            mq.push(-1);
        });
    std::thread th2([&]{
            int v;
            while ((v = mq.pop()) > 0) {
                std::cout << v << std::endl;
            }
            std::cout << "(EOD)" << std::endl;
        });
    th1.join();
    th2.join();
}
