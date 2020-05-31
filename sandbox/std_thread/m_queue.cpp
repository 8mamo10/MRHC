// g++ -Wall -pthread -std=c++11 m_queue.cpp
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class m_queue {
    static const int capacity = 10;
    std::queue<int> q;
    std::mutex mtx;
public:
    void push(int data) {
        std::unique_lock<std::mutex> lk(mtx);
        while (q.size() == capacity) {
            lk.unlock();
            std::this_thread::yield();
            lk.lock();
        }
        q.push(data);
    }
    int pop() {
        std::unique_lock<std::mutex> lk(mtx);
        while (q.empty()) {
            lk.unlock();
            std::this_thread::yield();
            lk.lock();
        }
        int data = q.front();
        q.pop();
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
