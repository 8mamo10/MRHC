// g++ pthread.cpp -std=c++11 -lpthread

#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

void *thread_func(void *param);

pthread_mutex_t m;
int total = 0;

int main(void) {
    pthread_t thread1;
    pthread_t thread2;
    int ret = 0;
    string name1 = "hoge";
    string name2 = "fuga";

    ret = pthread_mutex_init(&m, NULL);
    if (ret != 0) {
        exit(1);
    }
    ret = pthread_create(&thread1, NULL, thread_func, &name1);
    if (ret != 0) {
        exit(1);
    }
    ret = pthread_create(&thread2, NULL, thread_func, &name2);
    if (ret != 0) {
        exit(1);
    }
    ret = pthread_join(thread1, NULL);
    ret = pthread_join(thread2, NULL);
    if (ret != 0) {
        exit(1);
    }
    cout << "finished:" << total << endl;
    return 0;
}

void *thread_func(void *param)
{
    int ret = 0;
    string *name = (string*)param;
    cout << "thread:" << pthread_self() << ":" << (*name).c_str() << endl;
    for (int i = 0; i < 100000; i++) {
        ret = pthread_mutex_lock(&m);
        if (ret != 0) {
            exit(1);
        }
        total++;
        ret = pthread_mutex_unlock(&m);
        if (ret != 0) {
            exit(1);
        }
    }
}
