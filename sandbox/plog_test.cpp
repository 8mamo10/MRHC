#include <bits/stdc++.h>

#include "plog/Log.h"

using namespace std;

int main() {
    plog::init(plog::debug, "hoge.log");
    LOG_DEBUG << "Hello, hoge!";
}
