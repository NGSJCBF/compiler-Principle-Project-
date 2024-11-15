#ifndef SDFA_H
#define SDFA_H

#include <queue>
#include <set>
#include <string>
#include <vector>
#include <map>
#include "DFA.h"
using namespace std;

class sdfa{
public:

    int size;
    set<string> edges;
    set<int> accept;
    vector<map<string,set<int>>> v;
    sdfa(){size=0;};
    sdfa(const dfa &);
    sdfa(const dfa&,int);
    void simplify(const dfa &);
    void simplify(const dfa &,const map<string,sdfa>);
    void simplify(const sdfa &);
    // 合并另一个 sdfa 到当前 sdfa 中
    void merge(const sdfa& other);

private:
    void djset_Initialize(vector<int> &, queue< set<int> > q);
    void djset_map(vector<int> &, queue<set<int> >);
    void merge(queue<set<int> >, const dfa &);
    void merge(queue<set<int> >, const sdfa &);
};

/*
class meragesdfa{
public:
    int size;
    set<string> edges;
    set<int> accept;
    vector<map<string,set<int>>> v;

    meragesdfa(){size=0;}
    void simplify(const dfa &);
    void merge(const sdfa& other,const string&name);
private:
    void djset_Initialize(vector<int> &, queue< set<int> > q);
    void djset_map(vector<int> &, queue<set<int> >);
    void merge(queue<set<int> >, const dfa &);
};
*/
#endif // SDFA_H
