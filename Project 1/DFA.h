#ifndef DFA_H
#define DFA_H
#include <string>
#include <vector>
#include <map>
#include <set>
#include <QDebug>
using namespace std;

class dfa{
public:
    int size;
    vector<set<int>> state_Set;
    map<set<int>, int> state_num;
    set<string> edges;
    set<int> accept;
    vector<map<string,int>> v;
    dfa(){}
    dfa(const set<int>, const set<string> &, const int &);      //初始化dfa节点
    void append(const set<int> &, const int &);
    void insert(const int &, const string &, const set<int> &);
    string state_set_str(const int &) const;
    string state_set_str(const int &, const string &) const;
    void merge(const dfa&);
};

#endif // DFA_H
