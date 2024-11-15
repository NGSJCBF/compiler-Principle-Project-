#include "SDFA.h"

sdfa::sdfa(const dfa &a){
    edges=a.edges;
    accept=a.accept;
}
sdfa::sdfa(const dfa &a,int) {
    size = 0;
    edges = a.edges;

    // 映射每个状态集合到唯一编号
    map<set<int>, int> state_map;
    for (int i = 0; i < a.state_Set.size(); ++i) {
        const auto &state_set = a.state_Set[i];
        if (state_map.find(state_set) == state_map.end()) {
            state_map[state_set] = size++;  // 为新状态集合分配编号
        }
    }

    // 存储转换关系，将 `dfa` 转换为编号后的形式
    for(auto&x:a.v){
        map<string,set<int>>temp_v;
        for(auto&[name,destination]:x){
            temp_v[name].insert(destination);
        }
        v.push_back(temp_v);
    }
    // for (int i = 0; i < a.state_Set.size(); ++i) {
    //     int src_id = state_map[a.state_Set[i]];  // 当前状态的编号
    //     for (const auto &edge_pair : a.v[i]) {
    //         const string &edge = edge_pair.first;
    //         const int &dest_states = edge_pair.second;

    //         int dest_id = state_map[dest_states];  // 获取目标状态的编号

    //         v[src_id][edge].insert(dest_id);       // 使用编号存储转换
    //     }
    // }

    // 处理接受状态，假设 `a.accept` 是 `int` 类型
    for (const auto &accept_state : a.accept) {
        accept.insert(accept_state);
    }
}


void sdfa::simplify(const dfa &a,const map<string,sdfa>){
    queue<set<int>> q;

    set<int> not_accept;
    for (int i=0;i<a.size;++i){
        if (accept.count(i)==0) not_accept.insert(i);
    }

    if (!not_accept.empty()) q.push(not_accept);
    q.push(accept);


    for (auto &str:edges){
        bool stop = true;   // 如果全都只有一个元素则停止
        int n = q.size();

        vector<int> disjoint_set(a.size);
        map<int, set<int>> m;

        for (int i=0;i<n;++i){
            djset_Initialize(disjoint_set,q);

            set<int> tmp_set = q.front();
            q.pop();

            if (tmp_set.size()==1 || tmp_set.empty()){
                q.push(tmp_set);
                continue;
            }

            stop = false;

            for (auto &j:tmp_set){
                auto it=a.v[j].find(str);
                if (it==a.v[j].end()){
                    m[-1].insert(j);
                } else {
                    m[disjoint_set[it->second]].insert(j);
                }
            }

            for (auto &it:m){
                q.push(it.second);
            }
            m.clear();
        }

        if (stop) break;
    }

    merge(q,a);
}

void sdfa::simplify(const dfa &a){
    queue<set<int>> q;

    set<int> not_accept;
    for (int i=0;i<a.size;++i){
        if (accept.count(i)==0) not_accept.insert(i);
    }

    if (!not_accept.empty()) q.push(not_accept);
    q.push(accept);


    for (auto &str:edges){
        bool stop = true;   // 如果全都只有一个元素则停止
        int n = q.size();

        vector<int> disjoint_set(a.size);
        map<int, set<int>> m;

        for (int i=0;i<n;++i){
            djset_Initialize(disjoint_set,q);

            set<int> tmp_set = q.front();
            q.pop();

            if (tmp_set.size()==1 || tmp_set.empty()){
                q.push(tmp_set);
                continue;
            }

            stop = false;

            for (auto &j:tmp_set){
                auto it=a.v[j].find(str);
                if (it==a.v[j].end()){
                    m[-1].insert(j);
                } else {
                    m[disjoint_set[it->second]].insert(j);
                }
            }

            for (auto &it:m){
                q.push(it.second);
            }
            m.clear();
        }

        if (stop) break;
    }

    merge(q,a);
}

void sdfa::simplify(const sdfa &a) {
    queue<set<int>> q;

    set<int> not_accept;
    for (int i = 0; i < a.size; ++i) {
        if (accept.count(i) == 0) not_accept.insert(i);
    }

    if (!not_accept.empty()) q.push(not_accept);
    q.push(accept);

    for (auto &str : edges) {
        bool stop = true;   // Stop if all sets contain only one element
        int n = q.size();

        vector<int> disjoint_set(a.size);
        map<int, set<int>> m;

        for (int i = 0; i < n; ++i) {
            djset_Initialize(disjoint_set, q);

            set<int> tmp_set = q.front();
            q.pop();

            if (tmp_set.size() == 1 || tmp_set.empty()) {
                q.push(tmp_set);
                continue;
            }

            stop = false;

            for (auto &j : tmp_set) {
                auto it = a.v[j].find(str);
                if (it == a.v[j].end()) {
                    m[-1].insert(j);  // No transition for this symbol
                } else {
                    for (int target_state : it->second) {
                        m[disjoint_set[target_state]].insert(j);
                    }
                }
            }

            for (auto &it : m) {
                q.push(it.second);
            }
            m.clear();
        }

        if (stop) break;
    }

    merge(q, a);
}


void sdfa::djset_Initialize(vector<int> &dj_set, queue<set<int>> q){
    while (!q.empty()){
        set<int> tmp_set=q.front();
        q.pop();

        auto it=tmp_set.begin();
        int root=*it;
        dj_set[root]=root;
        for (++it;it!=tmp_set.end();++it){
            dj_set[*it]=root;
        }
    }
}

void sdfa::djset_map(vector<int> &dj_set, queue<set<int>> q){
    int k=0;
    while (!q.empty()){
        set<int> tmp_set=q.front();
        q.pop();

        for (auto &i:tmp_set){
            dj_set[i]=k;
        }

        ++k;
    }
}

void sdfa::merge(queue<set<int>> q, const dfa &a){
    set<int> tmp_accept;

    // 找到初态所在的集合
    set<int> tmp_set = q.front();
    while (tmp_set.count(0)==0){
        q.pop();
        q.push(tmp_set);
        tmp_set=q.front();
    }

    size=q.size();
    vector<int> dj_set(a.size);
    djset_map(dj_set,q);
    while (!q.empty()){
        map<string,set<int>> m;

        tmp_set=q.front();
        q.pop();
        auto one = tmp_set.begin();

        if (accept.count(*one)!=0) tmp_accept.insert(dj_set[*one]);

        for (auto &str:edges){
            auto it = a.v[*one].find(str);
            if (it!=a.v[*one].end()){
                m[str].insert(dj_set[it->second]);
            }
        }

        v.push_back(m);
    }

    accept=tmp_accept;
}

void sdfa::merge(queue<set<int>> q, const sdfa &a) {
    set<int> tmp_accept;

    // Locate the set containing the initial state
    set<int> tmp_set = q.front();
    while (tmp_set.count(0) == 0) {
        q.pop();
        q.push(tmp_set);
        tmp_set = q.front();
    }

    size = q.size();
    vector<int> dj_set(a.size);
    djset_map(dj_set, q);

    while (!q.empty()) {
        map<string, set<int>> m;

        tmp_set = q.front();
        q.pop();
        auto one = tmp_set.begin();

        // Add to accept states if this group contains an accept state
        if (accept.count(*one) != 0) tmp_accept.insert(dj_set[*one]);

        for (auto &str : edges) {
            auto it = a.v[*one].find(str);
            if (it != a.v[*one].end()) {
                // Insert each target state into `m[str]`
                for (int target_state : it->second) {
                    m[str].insert(dj_set[target_state]);
                }
            }
        }

        v.push_back(m);
    }

    accept = tmp_accept;
}


void sdfa::merge(const sdfa &other) {
    int stateOffset = this->v.size();  // Offset for state indices in `other`

    // Merge edges from the other `sdfa`
    edges.insert(other.edges.begin(), other.edges.end());

    // Merge transitions from the other `sdfa`
    int count=0;
    for (const auto &stateTransitions : other.v) {
        if(count==0&&size!=0){
            auto &x=this->v[0];
            for(auto&[name,nextState]:stateTransitions){
                for (int target : nextState) {
                    x[name].insert(target + stateOffset);  // Offset target state
                }
            }
            count++;
            continue;
        }

        map<string, set<int>> adjustedTransitions;
        // Adjust each transition to the new index offset
        for (const auto &[symbol, targets] : stateTransitions) {
            set<int> adjustedTargets;
            for (int target : targets) {
                adjustedTargets.insert(target + stateOffset);  // Offset target state
            }
            adjustedTransitions[symbol] = adjustedTargets;
        }

        // Add the adjusted transitions to the current `v`
        v.push_back(adjustedTransitions);
    }

    // Adjust and merge accepting states
    for (int acceptState : other.accept) {
        if(acceptState==0){
            accept.insert(acceptState);
            continue;
        }
        accept.insert(acceptState + stateOffset);  // Offset accept state
    }

    // Update the size
    size += other.size;
    size -= 1;
}
