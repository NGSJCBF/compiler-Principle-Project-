#include "converter.h"
#include<unordered_set>
std::set<char> visiableChar = {'!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/',
                              '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
                              '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
                              'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
                              '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
                              'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~'};

int get_token(const string &line) {
    int k = 0;
    // Start searching from the beginning until '=' is found
    for (int i = 0; i < line.size(); ++i) {
        if (line[i] == '=') {

            k = i;
            break;
        }
    }
    return k;  // Returns the position right before '='
}

int check(const string &line){
    int k = 0;
    // if (line[0]=='_')
    //     ++k;

    while (line[k]=='\t'||line[k]==' ')
        ++k;
    return k;
}

string getToken_RE(const trie &id, const string &s, int &k, bool &b){
    int last = k;
    b = false;

    // 判断 []
    if (s[last] == '['){
        return "0x00";
    // 判断 (
    } else if (s[last] == '(') {
        ++last;
        b = true;
    }
    // 判断 转义 '\'
    else if (s[last] == '\\')
    {
        ++k;
        last += 2;
    }
    else
        last = id.search(s, k);

    string result = s.substr(k, last - k);
    k = last;
    return result;
}

void unary_op(nfa &a, const string &s, int &k){
    while (k<s.size())
    {
        switch (s[k]){
            case '*':
                a.closure();
                break;
            case '+':
                a.pos_closure();
                break;
            case '?':
                a.optional();
                break;
            default:
                return;
        }
        ++k;
    }
}

nfa toNFA(const trie &id, const string &s, int &k) {
    nfa result;
    bool exist = false; // 判断是否存在选择 "|" 运算
    bool lbracket = false;  // 判断是否为左括号

    while (k < s.size()) {
        string tmpstr = getToken_RE(id, s, k, lbracket);
        nfa cur;
        // if(name.find(tmpstr)!=name.end()){
        //     // cur.concatenation()
        // }
        // 处理 []
        if (tmpstr == "0x00") {  // 检测到 `[]` 开始符
            nfa charClassNFA;
            ++k;
            while (s[k] != ']' && k < s.size()) {
                if (s[k + 1] == '-' && s[k + 2] != ']') {  // 检查范围符号，例如 [0-9]
                    char start = s[k];
                    char end = s[k + 2];
                    for (char c = start; c <= end; ++c) {
                        string str;
                        str+=c;
                        nfa charNFA(str);  // 为范围内每个字符创建一个单独的 NFA
                        charClassNFA.alternative(charNFA);  // 将字符 NFA 添加到字符集 NFA
                    }
                    k += 3;  // 跳过 "x-y"
                }else if(s[k]=='^'){
                    for(auto&i:visiableChar){
                        if(i==s[k+1])continue;
                        string str;
                        str+=i;
                        nfa charNFA(str);  // 为范围内每个字符创建一个单独的 NFA
                        charClassNFA.alternative(charNFA);  // 将字符 NFA 添加到字符集 NFA
                    }
//最抽象的地方了
                    string temp = "\\";
                    temp += 't';
                    charClassNFA.alternative(nfa(temp));
                    temp = "\\";
                    temp += 'n';
                    charClassNFA.alternative(nfa(temp));
                    temp = "\\";
                    temp += 'r';
                    charClassNFA.alternative(nfa(temp));
                    k++;//跳过{
                }else if(s[k]=='\\'&&(s[k+1]=='t'||s[k+1]=='r'||s[k+1]=='n')){
                    string temp;
                    temp=std::string(2,s[k])+s[k+1];
                    nfa charNFA(temp);  // 为单个字符创建一个 NFA
                    charClassNFA.alternative(charNFA);  // 添加到字符集 NFA
                    ++k;
                    ++k;
                }
                else {
                    nfa charNFA(std::to_string(s[k]-'0'));  // 为单个字符创建一个 NFA
                    charClassNFA.alternative(charNFA);  // 添加到字符集 NFA
                    ++k;
                }
            }

            ++k;  // 跳过 ']'
            cur = charClassNFA;  // 将字符集 NFA 赋值给当前 NFA
        }else if(tmpstr=="^"&&s[k-2]!='\\'){
            nfa charClassNFA;
            for(auto&i:visiableChar){
                if(i==s[k+1])continue;
                string str;
                str+=i;
                nfa charNFA(str);  // 为范围内每个字符创建一个单独的 NFA
                charClassNFA.alternative(charNFA);  // 将字符 NFA 添加到字符集 NFA
            }
            k++;//跳过{
            cur=charClassNFA;
        }
        else {
            cur = nfa(tmpstr);  // 非字符集情况，直接转换成 NFA
        }

        if (lbracket)
            cur = toNFA(id, s, k);

        unary_op(cur, s, k);

        while (k < s.size() && s[k] != '|' && s[k] != ')'&&s[k]!='[') {
            tmpstr = getToken_RE(id, s, k, lbracket);
            nfa tmp_nfa(tmpstr);
            if (lbracket)
                tmp_nfa = toNFA(id, s, k);
            unary_op(tmp_nfa, s, k);

            cur.concatenation(tmp_nfa);
        }

        if (k == s.size() || s[k] == ')') {
            ++k;
            if (exist) {
                result.alternative(cur);
                return result;
            }
            return cur;
        }
        if(s[k]=='[')continue;
        ++k;
        result.alternative(cur);
        exist = true;
    }

    // 正常情况不会到达
    return result;
}

nfa toNFA(const trie &id, const string &s, int &k, const map<string, nfa> &nfas) {
    nfa result;
    bool exist = false; // 判断是否存在选择 "|" 运算
    bool lbracket = false;  // 判断是否为左括号

    while (k < s.size()) {
        string tmpstr = getToken_RE(id, s, k, lbracket);
        nfa cur;

        // 检测命名规则
        if (nfas.find(tmpstr) != nfas.end()) {
            cur = nfas.at(tmpstr);  // 使用命名规则的 NFA
        }
        // 处理字符集 []
        else if (tmpstr == "0x00") {
            nfa charClassNFA;
            ++k;
            while (s[k] != ']' && k < s.size()) {
                if (s[k + 1] == '-' && s[k + 2] != ']') {  // 检查范围符号，例如 [0-9]
                    char start = s[k];
                    char end = s[k + 2];
                    for (char c = start; c <= end; ++c) {
                        string str;
                        str+=c;
                        nfa charNFA(str);  // 为范围内每个字符创建一个单独的 NFA
                        charClassNFA.alternative(charNFA);  // 将字符 NFA 添加到字符集 NFA
                    }
                    k += 3;  // 跳过 "x-y"
                }else if(s[k]=='^'){
                    for(auto&i:visiableChar){
                        if(i==s[k+1])continue;
                        string str;
                        str+=i;
                        nfa charNFA(str);  // 为范围内每个字符创建一个单独的 NFA
                        charClassNFA.alternative(charNFA);  // 将字符 NFA 添加到字符集 NFA
                    }
                    string temp = "\\";
                    temp += 't';
                    charClassNFA.alternative(nfa(temp));
                    temp = "\\";
                    temp += 'n';
                    charClassNFA.alternative(nfa(temp));
                    temp = "\\";
                    temp += 'r';
                    charClassNFA.alternative(nfa(temp));
                    k++;//跳过{
                }else if (s[k] == '\\' && (s[k + 1] == 't' || s[k + 1] == 'r' || s[k + 1] == 'n')) {
                    std::string temp;

                    // 构造字面量的转义序列，例如 "\\n"
                    temp = "\\";
                    temp += s[k + 1];

                    nfa charNFA(temp);                 // 为字面量 "\n"、"\t"、"\r" 创建 NFA
                    charClassNFA.alternative(charNFA); // 添加到字符集 NFA

                    k += 2; // 跳过这两个字符
                }

                else {
                    nfa charNFA(std::string(1,s[k]));  // 为单个字符创建一个 NFA
                    charClassNFA.alternative(charNFA);  // 添加到字符集 NFA
                    ++k;
                }
            }

            ++k;  // 跳过 ']'
            cur = charClassNFA;  // 将字符集 NFA 赋值给当前 NFA
        }
        // 非字符集或命名规则的情况
        else if(tmpstr=="^"&&s[k-2]!='\\'){
            nfa charClassNFA;
            for(auto&i:visiableChar){
                if(i==s[k+1])continue;
                string str;
                str+=i;
                nfa charNFA(str);  // 为范围内每个字符创建一个单独的 NFA
                charClassNFA.alternative(charNFA);  // 将字符 NFA 添加到字符集 NFA
            }
            k++;//跳过{
            cur=charClassNFA;
        }
        else {
            cur = nfa(tmpstr);
        }

        if (lbracket)
            cur = toNFA(id, s, k, nfas);

        unary_op(cur, s, k);

        while (k < s.size() && s[k] != '|' && s[k] != ')'&&s[k]!='[') {
            tmpstr = getToken_RE(id, s, k, lbracket);
            nfa tmp_nfa;

            if (nfas.find(tmpstr) != nfas.end()) {
                tmp_nfa = nfas.at(tmpstr);
            } else {
                tmp_nfa = nfa(tmpstr);
            }

            if (lbracket)
                tmp_nfa = toNFA(id, s, k, nfas);

            unary_op(tmp_nfa, s, k);

            cur.concatenation(tmp_nfa);
        }

        if (k == s.size() || s[k] == ')') {
            ++k;
            if (exist) {
                result.alternative(cur);
                return result;
            }
            return cur;
        }
        if(s[k]=='[')continue;
        ++k;
        result.alternative(cur);
        exist = true;
    }
    return result;
}

void e_closure(const nfa &a, int k, set<int> &target){
     auto it=a.v[k].find(epsilon);
     target.insert(k);

     if (it==a.v[k].end()) return;
     for (auto &i:it->second){
         if (target.count(i)==0){
             e_closure(a,i,target);
         }
     }
}

void e_closure(const nfa &a, const set<int> &kset, set<int> &target){
    for (auto &k:kset){
        e_closure(a,k,target);
    }
}

set<int> move(const nfa &a, const set<int> &kset, const string &str){
    set<int> target;
    for (auto &i:kset){
        auto it=a.v[i].find(str);
        if (it!=a.v[i].end()){
            target.insert(it->second.begin(),it->second.end());
        }
    }

    return target;
}

set<string> del_epsilon(const set<string> &s){
    set<string> e=s;
    e.erase(epsilon);
    return e;
}

dfa toDFA(const nfa &a){
    set<int> tmp_set;
    e_closure(a,0,tmp_set);                                         //初始化DFA首个结点
    dfa result(tmp_set, del_epsilon(a.edges), a.size);

    for (int i=0; i<result.size; ++i){
        for (auto &str:result.edges){
            tmp_set.clear();
            e_closure(a,move(a,result.state_Set[i],str),tmp_set);   //计算当前DFA节点的epsilon闭包
            if (tmp_set.size()==0) continue;
            result.append(tmp_set,a.size);
            result.insert(i,str,tmp_set);
        }
    }

    return result;
}

string resultCode;

// 生成词法分析器代码并返回为字符串
/*
string generateLexerCode(const map<string, sdfa> &sdfas) {
    ostringstream codeStream;
    size_t count=0;
    vector<int>finals;
    codeStream << "#include <iostream>" << endl;
    codeStream << "#include <string>" << endl;
    codeStream << "using namespace std;" << endl << endl;
    codeStream << "int main() {" << endl;
    codeStream << "    string input;" << endl;
    codeStream << "    cin >> input;" << endl;
    codeStream << "    int currentState = 0;" << endl;
    codeStream << "    int length = input.length();" << endl;
    codeStream << "    for (int i = 0; i < length; i++) {" << endl;
    codeStream << "        char c = input[i];" << endl;
    codeStream << "        switch (currentState) {" << endl;

    for (const auto &[name, automaton] : sdfas) {

        for (int state = 0; state < automaton.size; ++state) {
            codeStream << "            case " << state+count << ":" << endl;
            codeStream << "                switch (c) {" << endl;

            // Write each transition for this state
            for (const auto &[symbol, nextState] : automaton.v[state]) {
                codeStream << "                    case '" << symbol << "':" << endl;
                codeStream << "                        currentState = " << nextState+count << ";" << endl;
                codeStream << "                        break;" << endl;
            }

            codeStream << "                    default:" << endl;
            codeStream << "                        cout << \"Error: Invalid input character '\" << c << \"'\" << endl;" << endl;
            codeStream << "                        continue;" << endl;
            codeStream << "                }" << endl;
            codeStream << "                break;" << endl;
        }
        for(auto &accept:automaton.accept){
            finals.push_back(accept+count+1);
        }
        count+=automaton.v.size();
    }

    codeStream << "        }" << endl;
    codeStream << "    }" << endl;

    codeStream << "    switch (currentState) {" << endl;
    int i=0;
    for (const auto &[name, automaton] : sdfas) {
        for (const int state : automaton.accept) {
            codeStream << "        case " << finals[i++]-1 << ":" << endl;
            codeStream << "            cout << \"Accepted\" << endl;" << endl;
            codeStream << "            return 0;" << endl;
        }
    }

    codeStream << "        default:" << endl;
    codeStream << "            cout << \"Not Accepted\" << endl;" << endl;
    codeStream << "    }" << endl;
    codeStream << "    return 0;" << endl;
    codeStream << "}" << endl;

    resultCode = codeStream.str();
    return resultCode;
}

*/



string generateLexerCode(const map<string, sdfa> &sdfas, const set<string> &names) {
    ostringstream codeStream;
    size_t stateOffset = 0;
    unordered_map<int, string> finalStateNames; // 记录接受状态和对应DFA名称

    codeStream << "#include <iostream>" << endl;
    codeStream << "#include <string>" << endl;
    codeStream << "#include <unordered_map>" << endl;
    codeStream << "#include <unordered_set>" << endl;
    codeStream << "#include <fstream>" << endl;
    codeStream << "#include <sstream>" << endl;
    codeStream << "using namespace std;" << endl << endl;

    codeStream << "unordered_map<int, string> finalStatements;" << endl;
    codeStream << "int main(int argc, char* argv[]) {" << endl;
    codeStream << "    if (argc < 2) {\n";
    codeStream << "\t\tcerr << \"请提供一个 .tny 文件作为输入.\" << endl;";
    codeStream << "\t\treturn 1;" << endl;
    codeStream << "\t}" << endl;
    codeStream << "\tifstream inputFile(argv[1]);" << endl;
    codeStream << "\tif (!inputFile.is_open()) {" << endl;
    codeStream << "    \tcerr << \"无法打开文件: \" << argv[1] << endl;" << endl;
    codeStream << "\t\treturn 1;" << endl;
    codeStream << "\t}" << endl;
    codeStream << "    stringstream filecontent;" << endl;
    codeStream << "    filecontent << inputFile.rdbuf();" << endl;
    codeStream << "    string input;" << endl;
    codeStream << "    input = filecontent.str();" << endl;
    codeStream << "    unordered_set<int> currentStates = {0};" << endl;  // 改为集合
    codeStream << "    int tokenStart = 0;" << endl;
    codeStream << "    int length = input.length();" << endl;
    codeStream << "\t\tint linepos = 0;" << endl;
    codeStream << "    for (int i = 0; i < length; i++) {" << endl;
    codeStream << "        char c = input[i];" << endl;
    codeStream << "\t\tstring resultToken;" << endl;
    codeStream << "\t\tunordered_set<int> nextStates;" << endl;
    codeStream << "        for (int state : currentStates) {" << endl;
    codeStream << "            switch (state) {" << endl;

    // 生成DFA的每个状态的switch-case
    for (const auto &[name, automaton] : sdfas) {
        for (int state = 0; state < automaton.size; ++state) {
            codeStream << "                case " << state + stateOffset << ":" << endl;
            codeStream << "                    switch (c) {" << endl;

            for (const auto &[symbol, nextStates] : automaton.v[state]) {
                if (symbol.size() == 1) {  // 单字符情况
                    if (symbol == "\\" || symbol == "\'") {
                        codeStream << "                        case '\\" << symbol << "':" << endl;
                    } else {
                        codeStream << "                        case '" << symbol << "':" << endl;
                    }
                } else {  // 特殊字符集处理
                    codeStream << "                        case '" << symbol << "':" << endl;
                }

                for (int nextState : nextStates) {
                    codeStream << "                            nextStates.insert(" << nextState + stateOffset << ");" << endl;
                }
                codeStream << "                            break;" << endl;
            }

            codeStream << "                        default:" << endl;
            codeStream << "                            cout << \"Error: Invalid input character '\" << c << \"'\" << endl;" << endl;
            codeStream << "                            return -1;" << endl;
            codeStream << "                    }" << endl;
            codeStream << "                    break;" << endl;
        }

        // 记录接受状态和对应DFA名称
        for (int accept : automaton.accept) {
            finalStateNames[accept + stateOffset] = name;
        }
        stateOffset += automaton.size;
    }

    codeStream << "            }" << endl;
    codeStream << "        }" << endl;

    // 更新 currentStates 为 nextStates
    codeStream << "        currentStates = move(nextStates);" << endl;

    // 初始化 finalStatements 的内容
    for (const auto &[state, dfaName] : finalStateNames) {
        codeStream << "\t\tfinalStatements[" << state << "] = \"" << dfaName << "\";" << endl;
    }

    // 检查是否进入接受状态，识别 token
    codeStream << "        for (int state : currentStates) {" << endl;
    codeStream << "            if (finalStatements.count(state) > 0) {" << endl;
    codeStream << "                cout << \"Token: '\" << input.substr(tokenStart, i - tokenStart + 1) << \"' : '\" << finalStatements[state] << \"'\" << endl;" << endl;
    codeStream << "                tokenStart = i + 1;" << endl;
    codeStream << "                currentStates = {0};" << endl;  // 重置 currentStates
    codeStream << "                break;" << endl;
    codeStream << "            }" << endl;
    codeStream << "        }" << endl;
    codeStream << "    }" << endl;

    // 检查最后的状态是否是接受状态
    codeStream << "    bool accepted = false;" << endl;
    codeStream << "    for (int state : currentStates) {" << endl;
    codeStream << "        if (finalStatements.count(state) > 0) {" << endl;
    codeStream << "            accepted = true;" << endl;
    codeStream << "            break;" << endl;
    codeStream << "        }" << endl;
    codeStream << "    }" << endl;
    codeStream << "    if (!accepted) {" << endl;
    codeStream << "        cout << \"Not Accepted\" << endl;" << endl;
    codeStream << "    }" << endl;

    codeStream << "    return 0;" << endl;
    codeStream << "}" << endl;

    return codeStream.str();
}
