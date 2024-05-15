#ifndef MYCHARACTER_H
#define MYCHARACTER_H

#include "myproperty.h"
#include "myobject.h"
#include <QWidget>
#include <QVector>
#include <QString>
#include <string>
#include <QMovie>
#include <QDebug>
#include <QLabel>
#include <QFile>

class myCharacter: public myObject {
    Q_OBJECT

public:
    int id;
    int dir;
    QString name;
    QMovie *nowm;
    QMovie *norm, *normf, *attk, *attkf, *dead, *deadf;
    class myBlock *belong = nullptr;
    Property pro;
    QVector<class myBlock*> atk;
    int cd = 0;

public:
    explicit myCharacter(QWidget *parent = nullptr);
    void play();
    void stay();
    void death();
    void setnowm(QMovie *m);
    void getpro(std::string s);
    void hit(myCharacter *target);
    void behit(int damage, int type);
    int X();
    int Y();
};

#endif // MYCHARACTER_H