#include "NFA.h"
//初始化空节点
nfa::nfa()
{
    size = 1;
    edges.insert(epsilon);
    map<string, set<int>> m;
    v.push_back(m);
    v.push_back(m);
    v[0][epsilon].insert(1);
}

nfa::nfa(const string &s)
{
    size = 1;
    edges.insert(epsilon);
    map<string, set<int>> m;
    v.push_back(m);
    v.push_back(m);
    v[0][s].insert(1);
    edges.insert(s);
}
//连接：关键在更新节点编号
void nfa::concatenation(const nfa &b)
{
    int n = size;
    for (auto &i : b.v[0])
    {
        for (auto &j : i.second)
        {
            v[size][i.first].insert(j + size);
        }
    }

    map<string, set<int>> tmp;
    for (int k = 1; k <= b.size; ++k)
    {
        v.push_back(tmp);
        ++n;
        // 遍历b.v[k]对应的map的每一个键值对 i:<first, second>
        for (auto &i : b.v[k])
        {
        // 遍历该键值对中的值, 即对应的集合里的整数j
            for (auto &j : i.second)
            {
                v[n][i.first].insert(j + size);
            }
        }
    }

    size = n;
    edges.insert(b.edges.begin(), b.edges.end());
}
//选择：关键在更新节点编号
void nfa::alternative(const nfa &b)
{
    for (auto &i:nail){
        v[i][epsilon].erase(size);
        v[i][epsilon].insert(size + b.size + 1);
    }

    v[0][epsilon].insert(size);

    for (auto &i:b.v[0]){
        for (auto &j:i.second){
            v[size][i.first].insert(j + size);
        }
    }

    int n = size;
    map<string, set<int>> tmp;
    for (int k = 1; k <= b.size; ++k){
        v.push_back(tmp);
        ++n;

        for (auto &i : b.v[k])
        {
            for (auto &j:i.second){
                v[n][i.first].insert(j + size);
            }
        }
    }

    nail.insert(n);
    v[n][epsilon].insert(n + 1);
    v.push_back(tmp);
    ++n;
    size = n;

    edges.insert(b.edges.begin(), b.edges.end());
}

void nfa::pos_closure()
{
    v[size][epsilon].insert(0);
}

void nfa::optional()
{
    v[0][epsilon].insert(size);
}

//闭包看作正闭包加可选
void nfa::closure()
{
    pos_closure();
    optional();
}

