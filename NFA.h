#ifndef NFA_H
#define NFA_H
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <set>
using namespace std;

const string epsilon = "#";


class nfa
{
public:
    int size; // 终态的序号
    set<int> nail;   // 记录每个小nfa的终态节点, 方便"|"运算符中编号的处理
    set<string> edges;   // 保存所有出现的边
    vector<map<string, set<int>>> v;
//public:
    nfa();                             //初始化无字符的节点
    nfa(const string &);               //初始化带字符的节点
    void concatenation(const nfa &);   // 连接
    void alternative(const nfa &);     // 选择 "|"
    void pos_closure();                // 正闭包 "+"
    void optional();                   // 可选  "?"
    void closure();                    // 闭包  "*"
};


#endif // NFA_H
