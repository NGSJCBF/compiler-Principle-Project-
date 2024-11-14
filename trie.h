#ifndef TRIE_H
#define TRIE_H

#include <string>
using namespace std;
constexpr int trie_N = 1e3;

// 字典树
class trie {
private:
    int size = 0;
    bool accept[trie_N << 2];
    int t[trie_N << 2][54];

public:
    void insert(const string &s) {
        int p = 0;
        for (auto &i:s){
            int k;
            if (i >= 'A' && i <= 'Z')
            {
                k = i - 'A';
            } else {
                k = i - 'a' + 26;
            }
            if (t[p][k]==0){
                ++size;
                t[p][k] = size;
            }
            p = t[p][k];
        }

        accept[p] = true;
    }

    int search(const string &s, int y) const {
        int p = 0;
        for (int i=y; i < s.size(); ++i)
        {
            int k;

            if (s[i]>='A' && s[i]<='Z'){
                k = s[i] - 'A';
            } else if (s[i]>='a' && s[i]<='z') {
                k = s[i] - 'a' + 26;
            } else {
                return y + 1;
            }

            if (t[p][k] == 0)
                return y + 1;

            p = t[p][k];

            if (accept[p])
                y = i;
        }

        return y + 1;
    }
};

#endif // TRIE_H
