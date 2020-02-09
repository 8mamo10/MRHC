#include <bits/stdc++.h>

using namespace std;

int main() {
    string hoge = "192.168.1.2:5900:password";
    size_t i = hoge.find_first_of(":");
    cout << i << endl;
    // 192.168.1.2
    string hoge2 = hoge.substr(0, i);
    cout << hoge2 << endl;
    // 5900:password
    string hoge3 = hoge.substr(i+1, hoge.size());
    size_t j = hoge3.find_first_of(":");
    cout << j << endl;
    // 5900
    string hoge4 = hoge3.substr(0, j);
    cout << hoge4 << endl;
    // password
    string hoge5 = hoge3.substr(j+1, hoge3.size());
    cout << hoge5 << endl;

    vector<string> outputs;
    string delim = ":";
    while (true) {
        size_t i = hoge.find_first_of(delim);
        if (i == string::npos) {
            outputs.push_back(hoge);
            break;
        }
        string item = hoge.substr(0, i);
        outputs.push_back(item);
        hoge = hoge.substr(i+1, hoge.size());
    }
    for (auto j : outputs) {
        cout << j << endl;
    }
}
