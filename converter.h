#ifndef CONVERTER_H
#define CONVERTER_H
#include "trie.h"
#include "NFA.h"
#include "DFA.h"
#include "SDFA.h"
#include<sstream>
// 报错信息
// void Error();

//获取不需要生成DFA的标识符
int get_token(const string &);
/***************************
        生成NFA
***************************/
// 检查是否需要生成DFA, 并将标识符存入 字典树 中
int check(const string &);

// 从 正则表达式RE 中读取一个单词并返回该单词
string getToken_RE(const trie &, const string &, int &, bool &);

// 对于一个NFA, 处理单个或多个 单目运算符
void unary_op(nfa &, const string &, int &);

// 将 正则表达式 转化为 NFA
nfa toNFA(const trie &, const string &, int &);

nfa toNFA(const trie &, const string &, int &,const map<string,nfa>&);

/***************************
        生成DFA
***************************/
// 求NFA中某一个状态的ε闭包
void e_closure(const nfa &, int, set<int> &);

// 求NFA的某一个状态集合的ε闭包
void e_closure(const nfa &, const set<int> &, set<int> &);

// 求NFA的某一个状态集合经过字符串或边 s-转换 能够到达的状态集合
set<int> move(const nfa &, const set<int> &, const string &);

// 去掉 nfa 中的边集中的 ε
set<string> del_epsilon(const set<string> &);

// 将 NFA 转化为 DFA
dfa toDFA(const nfa &);

//代码生成
string generateLexerCode(const map<string, sdfa> &sdfas,const set<string>&);

#endif // CONVERTER_H
