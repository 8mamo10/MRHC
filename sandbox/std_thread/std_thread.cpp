// g++ -Wall -pthread -std=c++11 std_thread.cpp
#include <iostream>
#include <thread>

using namespace std;

void func()
{
    cout << "Hello thread." << endl;
}

int main()
{
    thread(func).join();
}
