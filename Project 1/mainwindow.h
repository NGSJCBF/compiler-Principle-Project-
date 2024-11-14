#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "trie.h"
#include "NFA.h"
#include "SDFA.h"
#include "converter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class mainWindow; }
QT_END_NAMESPACE

class mainWindow : public QMainWindow
{
    Q_OBJECT

public:
    mainWindow(QWidget *parent = nullptr);
    ~mainWindow();


private slots:
    void openActionSlot();
    void saveActionSlot();

    void on_convertButton_clicked();

private:
    Ui::mainWindow *ui;
    QString filePath;
    void outputSDFAs(const map<string, sdfa> &);
    void outputDFAs(const map<string, dfa>&);
    void outputNFAs(const map<string, nfa>&);
};
#endif // MAINWINDOW_H
