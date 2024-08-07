#include "mymonster.h"
#include "mytower.h"
#include "myblock.h"
#include <QRandomGenerator>

myTower::myTower(int xx, int yy, const QString &data, QWidget *parent): myCharacter(parent) { //构造函数
    //读取防御塔文件
    QFile file(data);
    file.open(QFile::ReadOnly);
    //载入图像资源
    name = QString::fromUtf8(file.readLine()).chopped(2);
    norm = new QMovie(QString::fromUtf8(file.readLine()).chopped(2));
    normf = new QMovie(QString::fromUtf8(file.readLine()).chopped(2));
    attk = new QMovie(QString::fromUtf8(file.readLine()).chopped(2));
    attkf = new QMovie(QString::fromUtf8(file.readLine()).chopped(2));
    this->setnowm(norm);
    //读取防御塔数值
    std::string s = file.readLine().toStdString();
    getpro(s);
    s = file.readLine().toStdString();
    size_t n = s.length(), i = 0;
    int a[2];
    while (s[i] >= '0' && s[i] <= '9' && i < n) {
        type = type * 10 + s[i] - '0';
        i++;
    }
    //读取并设置初始位置和大小
    s = file.readLine().toStdString();
    n = s.length();
    i = 0;
    for (int j = 0; j < 2; j++) {
        int x = 0;
        while (s[i] >= '0' && s[i] <= '9' && i < n) {
            x = x * 10 + s[i] - '0';
            i++;
        }
        i++;
        a[j] = x;
    }
    this->setGeometry(xx, yy, a[0], a[1]);
    this->setScaledContents(true);
    this->show();
    //读取防御塔攻击范围
    s = file.readLine().toStdString();
    n = s.length(); i = 0;
    for (int j = 0; ; j++) {
        int x = 0, f = 1;
        if (s[i] == '-') {
            f = -1;
            i++;
        }
        while (s[i] >= '0' && s[i] <= '9' && i < n) {
            x = x * 10 + s[i] - '0';
            i++;
        }
        a[j % 2] = x * f;
        if (j % 2 == 1) {
            area.push_back(std::make_pair(a[0], a[1]));
        }
        if (s[i] == ';' || i >= n) break;
        i++;
    }
    this->blood = new myBlood(this, parent);
}

void myTower::bar(myMonster *p) {
    cap += p->pro.WEI;
    bared.push_back(p);
}

int myTower::dis(int x, int y) {
    return abs(x - this->X()) * (x - this->X()) + (y - this->Y()) * (y - this->Y());
}


void myTower::update(int type) {
    if (type == 1) { //狂暴
        pro.PATK *= 2;
        pro.MATK *= 2;
        pro.TATK *= 2;
        pro.ATKF *= 2;
        pro.HP *= 2;
        pro.maxHP *= 2;
        attk->setSpeed(attk->speed() / 2);
        attkf->setSpeed(attkf->speed() / 2);
    }
    if (type == 2) { //冰冻
        bd = true;
    }
    if (type == 3) { //放血
        fx = true;
    }
    if (type == 4) { //群攻
        qg = true;
    }
    if (type == 5) { //击晕
        jy = true;
    }
    if (type == 6) { //鹰眼
        yy = true;
    }
    if (type == 7) { //快攻
        pro.ATKF /= 2;
        attk->setSpeed(attk->speed() * 2);
        attkf->setSpeed(attkf->speed() * 2);
    }
}

void myTower::act() { //防御塔活动逻辑
    if (alive == false || beset == false) return;
    this->raise();
    this->blood->raise();
    if (xr) {
        xr--;
        if (xr == 0) {
            pro.PATK *= 2;
            pro.MATK *= 2;
            pro.TATK *= 2;
        }
    }
    //更新所在地块
    if (belong == nullptr) {
        int x = this->X() / 100, y = this->Y() / 100;
        belong = isin->block[y * 15 + x];
        isin->block[y * 15 + x]->tower = this;
    }
    //优先攻击阻挡单位
    while (!bared.empty() && bared.front()->alive == false) {
        cap -= bared.front()->pro.WEI;
        bared.pop_front();
    }
    myMonster *itk[2] = {nullptr, nullptr};
    if (!bared.empty()) {
        itk[0] = bared.front();
        if (qg) {
            if (bared.size() > 1) {
                itk[1] = bared[1];
            }
        }
    }
    if (bared.empty() || (qg && (itk[0] == nullptr || itk[1] == nullptr))){
        //检测攻击范围内的单位, 选取单位
        int x = this->X() / 100, y = this->Y() / 100, mn = 0x7fffffff;
        size_t m = area.size();
        for (size_t j = 0; j < m; j++) {
            int dx = area[j].first, dy = area[j].second;
            if (x + dx >= 0 && x + dx < 15 && y + dy >= 0 && y + dy < 9) {
                myBlock *u = isin->block[(y + dy) * 15 + (x + dx)];
                QMap<int, myMonster*>::const_iterator it = u->monster.constBegin();
                for (; it != u->monster.constEnd(); it++) {
                    if ((*it)->yn == true && this->type == 1) { //怪物隐匿
                        continue;
                    } 
                    if ((*it)->dis() < mn) {
                        mn = (*it)->dis();
                        if (qg) {
                            if (itk[0] != nullptr) {
                                itk[1] = itk[0];
                            }
                        }
                        itk[0] = *it;
                    }
                }
            }
        }
    }
    for (int i = 0; i < (qg?2:1); i++) {
        if (itk[i] != nullptr) {
            if (itk[i]->X() - this->X() < 0) dir = -1;
            else if (itk[i]->X() - this->X() > 0) dir = 1;
            else dir = 0;
            if (dir == 1 || dir == 0) this->setnowm(attk);
            if (dir == -1) this->setnowm(attkf);
            hit(itk[i]);
            if (cd == 0) {
                if (bd) {
                    if (itk[i]->bd == 0) {
                        itk[i]->pro.SPD /= 2;
                        itk[i]->pro.ATKF *= 2;
                        itk[i]->attk->setSpeed(attk->speed() / 2);
                        itk[i]->attkf->setSpeed(attkf->speed() / 2);
                        itk[i]->norm->setSpeed(norm->speed() / 2);
                        itk[i]->normf->setSpeed(normf->speed() / 2);
                    }
                    itk[i]->bd = 20;
                }
                if (fx) {
                    itk[i]->lx = 50;
                }
                if (jy) {
                    int r = QRandomGenerator::global()->bounded(100);
                    if (r < 20) {
                        if (itk[i]->xy == 0) itk[i]->stay();
                        itk[i]->xy = 50;
                    }
                }
            }
            while (!bared.empty() && bared.front()->alive == false) {
                cap -= bared.front()->pro.WEI;
                bared.pop_front();
            }
        }
        else {
            if (dir == 1 || dir == 0) this->setnowm(norm);
            if (dir == -1) this->setnowm(normf);
            cd = 0;
        }
    }
}


myTower::~myTower() {

}