#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

bool cmp(pair<char, int> a, pair<char, int> b) {
    return a.second > b.second;
}

void sort(string str) {
    map<char, int> count;
    for (int i = 0; i < str.size(); i++) {
        count[str[i]]++;
    }
    vector<pair<char, int>> v(count.begin(), count.end());
    sort(v.begin(), v.end(), cmp);
    for (int i = 0; i < v.size(); i++) {
        cout << v[i].first << v[i].second;
    }
}

int main() {
    string str;
    cin >> str;
    sort(str);
    return 0;
}