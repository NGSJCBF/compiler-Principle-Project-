#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<algorithm>

mainWindow::mainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mainWindow)
{
    ui->setupUi(this);

    connect(ui->OpenAction,&QAction::triggered,this,&mainWindow::openActionSlot);
    connect(ui->SaveAction,&QAction::triggered,this,&mainWindow::saveActionSlot);

    this->setWindowTitle("Converter");
}

mainWindow::~mainWindow()
{
    delete ui;
}

void mainWindow::openActionSlot()
{
    QString fileName=QFileDialog::getOpenFileName(this, "Open a file",
                                                  QCoreApplication::applicationDirPath(),"*.txt");
    if (fileName.isEmpty()) return;

    QFile fin(fileName);
    if (!fin.open(QIODevice::ReadOnly))
    {
        fileName.clear();
        qDebug()<<"Can't open "<<fileName;
        QMessageBox::warning(this,"Warning","Failed to open it!");
        return;
    }

    QByteArray ba = fin.readAll();
    ui->textEditor->setText(QString(ba));
    fin.close();

}

void mainWindow::saveActionSlot()
{
    QString fileName=QFileDialog::getSaveFileName(this, "Choose a file",
                                                  QCoreApplication::applicationDirPath(), "*.txt");
    if (fileName.isEmpty()) {
        QMessageBox::warning(this,"Warning","Please choose a file!");
        return;
    }

    QFile fout(fileName);
    fout.open(QIODevice::WriteOnly);

}

void mainWindow::outputNFAs(const map<string, nfa> &nfas) {
    ui->nfaTable->clear();  // 清空表格内容和表头
    ui->nfaTable->setRowCount(0);

    QStringList header;
    header << "Rule" << "State";  // 初始化前两列表头
    map<string, int> CF;

    // 禁用更新以提高性能
    ui->nfaTable->setUpdatesEnabled(false);

    // 提前计算最大列数并设置转换符号的列索引
    int nextColumnIndex = 2;  // 从第三列开始分配
    for (const auto &[ruleName, a] : nfas) {
        for (const auto &s : a.edges) {
            if (CF.find(s) == CF.end()) {
                header << QString::fromStdString(s);
                CF[s] = nextColumnIndex++;
            }
        }
    }

    // 设置列数和表头
    ui->nfaTable->setColumnCount(header.size());
    ui->nfaTable->setHorizontalHeaderLabels(header);
    ui->nfaTable->verticalHeader()->setVisible(false);

    // 插入数据到表格中
    int rowCounter = 0;
    int count=0;
    for (const auto &[ruleName, a] : nfas) {
        for (int i = 0; i <= a.size; ++i) {
            // 插入新行
            ui->nfaTable->insertRow(rowCounter);

            // 插入规则名称
            QTableWidgetItem *ruleItem = new QTableWidgetItem(QString::fromStdString(ruleName));
            ui->nfaTable->setItem(rowCounter, 0, ruleItem);

            // 插入状态编号（初始状态带箭头）
            QString start = QString::number(i+count) + (i == 0 ? "\u2190" : "");
            QTableWidgetItem *stateItem = new QTableWidgetItem(start);
            ui->nfaTable->setItem(rowCounter, 1, stateItem);

            // 插入转换信息
            auto transitions = a.v[i];
            for (const auto &[symbol, targets] : transitions) {
                if (CF.find(symbol) != CF.end()) {
                    int colIndex = CF[symbol];
                    QString targetsStr;
                    for (auto it = targets.begin(); it != targets.end(); ++it) {
                        if (it != targets.begin()) targetsStr += ",";
                        targetsStr += QString::number(*it);
                    }
                    QTableWidgetItem *transitionsItem = new QTableWidgetItem(targetsStr);
                    ui->nfaTable->setItem(rowCounter, colIndex, transitionsItem);
                }
            }

            ++rowCounter;
        }
        count+=a.size;
        // 将每个 NFA 的终止状态行背景设置为蓝色
        ui->nfaTable->item(rowCounter - 1, 1)->setBackground(Qt::blue);
    }

    // 重新启用更新
    ui->nfaTable->setUpdatesEnabled(true);
}

void mainWindow::outputDFAs(const map<string, dfa> &dfas) {
    ui->dfaTable->clear();  // 清空表格内容和表头
    ui->dfaTable->setRowCount(0);

    QStringList header;
    header << "Rule" << "State";  // 初始化前两列表头
    map<string, int> CF;

    // 禁用更新以提高性能
    ui->dfaTable->setUpdatesEnabled(false);

    // 提前计算最大列数并设置转换符号的列索引
    int nextColumnIndex = 2;  // 从第三列开始分配
    for (const auto &[ruleName, a] : dfas) {
        for (const auto &s : a.edges) {
            if (CF.find(s) == CF.end()) {
                header << QString::fromStdString(s);
                CF[s] = nextColumnIndex++;
            }
        }
    }

    // 设置列数和表头
    ui->dfaTable->setColumnCount(header.size());
    ui->dfaTable->setHorizontalHeaderLabels(header);
    ui->dfaTable->verticalHeader()->setVisible(false);

    int count=0;
    for (const auto &[ruleName, a] : dfas) {

        for (int i = 0; i < a.size; ++i) {
            ui->dfaTable->insertRow(ui->dfaTable->rowCount());

            QTableWidgetItem *ruleItem = new QTableWidgetItem(QString::fromStdString(ruleName));
            ui->dfaTable->setItem(ui->dfaTable->rowCount() - 1, 0, ruleItem);

            QString start(a.state_set_str(i).c_str());
            if (i == 0) start.append("\u2190");
            QTableWidgetItem *item = new QTableWidgetItem(start);
            ui->dfaTable->setItem(ui->dfaTable->rowCount() - 1, 1, item);
            if (a.accept.count(i) != 0) item->setBackground(Qt::green);

            auto m = a.v[i];
            for (auto &s : a.edges) {
                QString tmp_str(a.state_set_str(i, s).c_str());
                if (!tmp_str.isEmpty()) {
                    item = new QTableWidgetItem(tmp_str);
                    ui->dfaTable->setItem(ui->dfaTable->rowCount() - 1, CF[s], item);
                }
            }
        }
    }
    // 重新启用更新
    ui->dfaTable->setUpdatesEnabled(true);
}
/*
void mainWindow::outputSDFAs(const map<string, sdfa> &sdfas) {
    ui->sdfaTable->clear();  // 清空表格内容和表头
    ui->sdfaTable->setRowCount(0);

    QStringList header;
    header << "Rule" << "State";  // 初始化前两列表头
    map<string, int> CF;

    // 禁用更新以提高性能
    ui->sdfaTable->setUpdatesEnabled(false);

    // 提前计算最大列数并设置转换符号的列索引
    int nextColumnIndex = 2;  // 从第三列开始分配
    for (const auto &[ruleName, a] : sdfas) {
        for (const auto &s : a.edges) {
            if (CF.find(s) == CF.end()) {
                header << QString::fromStdString(s);
                CF[s] = nextColumnIndex++;
            }
        }
    }

    // 设置列数和表头
    ui->sdfaTable->setColumnCount(header.size());
    ui->sdfaTable->setHorizontalHeaderLabels(header);
    ui->sdfaTable->verticalHeader()->setVisible(false);

    int count=0;
    for (const auto &[ruleName, a] : sdfas) {

        for (int i = 0; i < a.size; ++i) {
            ui->sdfaTable->insertRow(ui->sdfaTable->rowCount());

            QTableWidgetItem *ruleItem = new QTableWidgetItem(QString::fromStdString(ruleName));
            ui->sdfaTable->setItem(ui->sdfaTable->rowCount() - 1, 0, ruleItem);

            QString start(QString::number(i+count));
            if (i == 0) start.append("\u2190");
            QTableWidgetItem *item = new QTableWidgetItem(start);
            ui->sdfaTable->setItem(ui->sdfaTable->rowCount() - 1, 1, item);
            if (a.accept.count(i) != 0) item->setBackground(Qt::green);

            auto m = a.v[i];
            for (auto &s : a.edges) {
                auto it = m.find(s);
                if (it != m.end()) {
                    item = new QTableWidgetItem(QString::number(it->second));
                    ui->sdfaTable->setItem(ui->sdfaTable->rowCount() - 1, CF[s], item);
                }
            }
        }
        count+=a.size;
    }
    // 重新启用更新
    ui->sdfaTable->setUpdatesEnabled(true);
}*/

void mainWindow::outputSDFAs(const map<string, sdfa> &sdfas) {
    ui->sdfaTable->clear();  // Clear table content and headers
    ui->sdfaTable->setRowCount(0);

    QStringList header;
    header << "Rule" << "State";  // Initialize the first two columns
    map<string, int> CF;

    // Disable updates to improve performance
    ui->sdfaTable->setUpdatesEnabled(false);

    // Calculate maximum column count and set index for each transition symbol
    int nextColumnIndex = 2;  // Start from the third column
    for (const auto &[ruleName, a] : sdfas) {
        for (const auto &s : a.edges) {
            if (CF.find(s) == CF.end()) {
                header << QString::fromStdString(s);
                CF[s] = nextColumnIndex++;
            }
        }
    }

    // Set column count and header labels
    ui->sdfaTable->setColumnCount(header.size());
    ui->sdfaTable->setHorizontalHeaderLabels(header);
    ui->sdfaTable->verticalHeader()->setVisible(false);

    int count = 0;
    for (const auto &[ruleName, a] : sdfas) {
        for (int i = 0; i < a.size; ++i) {
            ui->sdfaTable->insertRow(ui->sdfaTable->rowCount());

            QTableWidgetItem *ruleItem = new QTableWidgetItem(QString::fromStdString(ruleName));
            ui->sdfaTable->setItem(ui->sdfaTable->rowCount() - 1, 0, ruleItem);

            QString start(QString::number(i + count));
            if (i == 0) start.append("\u2190");
            QTableWidgetItem *item = new QTableWidgetItem(start);
            ui->sdfaTable->setItem(ui->sdfaTable->rowCount() - 1, 1, item);
            if (a.accept.count(i) != 0) item->setBackground(Qt::green);

            auto m = a.v[i];
            for (auto &s : a.edges) {
                auto it = m.find(s);
                if (it != m.end()) {
                    // Collect all target states in a comma-separated list
                    QString targets;
                    for (const int targetState : it->second) {
                        if (!targets.isEmpty()) targets += ",";
                        targets += QString::number(targetState);
                    }
                    // Set the item with the list of target states
                    item = new QTableWidgetItem(targets);
                    ui->sdfaTable->setItem(ui->sdfaTable->rowCount() - 1, CF[s], item);
                }
            }
        }
        count += a.size;
    }

    // Re-enable updates
    ui->sdfaTable->setUpdatesEnabled(true);
}


void mainWindow::on_convertButton_clicked() {
    QString text = ui->textEditor->toPlainText();
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Warning", "The regular expressions can't be empty!");
        return;
    }
    QStringList lines = text.split("\n");

    trie id;
    map<string, nfa> nfas;   // 存储所有命名规则的 NFA
    map<string, dfa> dfas;
    map<string, sdfa> sdfas;
    set<string> names;

    // 初步生成所有规则的 NFA，并存储在 nfas 中
    for (int i = 0; i < lines.count(); ++i) {
        string ruleName;
        int k=0,it = 0;
        if(lines[0]!='_'){
            it=check(lines[i].toStdString());
            k = get_token(lines[i].toStdString());
            names.insert( lines[i].toStdString().substr(it, k - it));
            ruleName=lines[i].toStdString().substr(it, k - it);
        }else{
            it=check(lines[i].toStdString());
            k = get_token(lines[i].toStdString());
            ruleName = lines[i].toStdString().substr(it+1, k - it);
            id.insert(ruleName);
        }

        if(ruleName=="")
            continue;
        names.insert(ruleName);

        // 忽略 '=' 符号后的空格
        while (lines[i].toStdString()[k] == ' ' || lines[i].toStdString()[k] == '=')
            ++k;

        // 根据初始规则生成 NFA（无替换）
        nfa temp_nfa = toNFA(id, lines[i].toStdString(), k);
        nfas[ruleName] = temp_nfa;

        // // 转换并存储为 DFA 和简化 DFA
        // dfa temp_dfa = toDFA(temp_nfa);
        // dfas[ruleName] = temp_dfa;
        // sdfa temp_sdfa(temp_dfa);
        // temp_sdfa.simplify(temp_dfa);
        // sdfas[ruleName] = temp_sdfa;
    }

    // 第二次遍历：对每条规则生成的 NFA 进行命名规则替换
    for (int i = 0; i < lines.count(); ++i) {
        int k=0,it = 0;
        string ruleName;
        it=check(lines[i].toStdString());
        k = get_token(lines[i].toStdString());
        ruleName = lines[i].toStdString().substr(it, k - it);
        if(ruleName[0]!='_'){
            continue;
        }else{
            ruleName=ruleName.substr(1,ruleName.size());
        }

        if(ruleName=="")
            continue;
        // 忽略 '=' 符号后的空格
        while (lines[i].toStdString()[k] == ' ' || lines[i].toStdString()[k] == '=')
            ++k;

        // 生成 NFA，并替换其中的命名规则
        nfa temp_nfa = toNFA(id, lines[i].toStdString(), k, nfas);
        nfas[ruleName] = temp_nfa;

        // 更新 DFA 和简化 DFA
        dfa temp_dfa = toDFA(temp_nfa);
        dfas[ruleName] = temp_dfa;
        sdfa temp_sdfa(temp_dfa);
        temp_sdfa.simplify(temp_dfa);
        sdfas[ruleName] = temp_sdfa;
    }
    outputNFAs(nfas);
    outputDFAs(dfas);

    //这里有问题
    sdfa tempsdfa;
    for(auto&[name,sdfa]:sdfas){
        tempsdfa.merge(sdfa);
    }
    // tempsdfa.simplify(tempsdfa);sdfa
    map<string, sdfa> sdfass;
    sdfass["all"]=tempsdfa;
    outputSDFAs(sdfass);

    /*
    重新创建一个class用来合并sdfa
    meragesdfa tempMerage;
    for(auto&[name,sdfa]:sdfas){
        tempMerage.merge(sdfa);
    }
    */

    string code=generateLexerCode(sdfas,names);
    QString Code=QString::fromStdString(code);
    ui->Code->setText(Code);
}
