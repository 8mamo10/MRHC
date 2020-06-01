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
    std::condition_variable cv;
public:
    void push(int data) {
        std::unique_lock<std::mutex> lk(mtx);
        while (q.size() == capacity) {
            cv.wait(lk);
        }
        q.push(data);
        cv.notify_all();
    }
    int pop() {
        std::unique_lock<std::mutex> lk(mtx);
        while (q.empty()) {
            cv.wait(lk);
        }
        int data = q.front();
        q.pop();
        cv.notify_all();
        return data;
    }
};

/////
int main()
{
    m_queue mq;
    std::thread th1([&]{
            for (int i = 1; i <= 100; i++) {
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
