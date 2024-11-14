#include "DFA.h"

dfa::dfa(const set<int> s, const set<string> &e, const int &ac_state){
    size=0;
    append(s,ac_state);
    edges=e;
}

void dfa::append(const set<int> &s, const int &ac_state){
    auto it=state_num.find(s);
    if (it!=state_num.end()) return;

    state_Set.push_back(s);
    state_num[s]=size;

    if (s.count(ac_state)!=0) accept.insert(size);

    map<string, int> tmp;
    v.push_back(tmp);

    ++size;
}

void dfa::insert(const int &k, const string &str, const set<int> &s){
    auto it=state_num.find(s);
    if (it==state_num.end()) {
        qDebug()<<"No such set<int>: " << k << "->{" ;
        for (auto &i:s){
            qDebug()<<i<<' ';
        }
        qDebug()<<"} through " << str.c_str();
        return ;
    }

    v[k][str]=it->second;
}

string dfa::state_set_str(const int &k, const string &str) const
{
    string result;

    auto it = v[k].find(str);
    if (it!=v[k].end()){
        auto sta_set=state_Set[it->second];
        auto tmp_it=sta_set.begin();

        result+="{";
        result+=to_string(*tmp_it);
        for(++tmp_it;tmp_it!=sta_set.end();++tmp_it){
            result+=",";
            result+=to_string(*tmp_it);
        }
        result+="}";
    }

    return result;
}

string dfa::state_set_str(const int &k)const
{
    string result;

    auto sta_set=state_Set[k];
    auto tmp_it=sta_set.begin();

    result+="{";
    result+=to_string(*tmp_it);
    for (++tmp_it;tmp_it!=sta_set.end();++tmp_it){
        result+=",";
        result+=to_string(*tmp_it);
    }
    result+="}";

    return result;
}

void dfa::merge(const dfa&other){
    int offset = size;
    int count=0;
    for (const auto& stateTransitions : other.v) {
        if(count==0){

            count++;
        }else{
            std::map<string, int> newTransitions;
            for (const auto& transition : stateTransitions) {
                newTransitions[transition.first] = transition.second + offset;
            }
            v.push_back(newTransitions);
        }
    }

    size += other.size;

    for (int acceptState : other.accept) {
        accept.insert(acceptState + offset);
    }

    edges.insert(other.edges.begin(), other.edges.end());
}
