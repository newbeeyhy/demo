#include "levelselectmenu.h"
#include "ui_levelselectmenu.h"

LevelSelectMenu::LevelSelectMenu(int *p, QDialog *parent): QDialog(parent), ui(new Ui::LevelSelectMenu) {
    level = p;
    ui->setupUi(this);
}

LevelSelectMenu::~LevelSelectMenu() {
    delete ui;
}

void LevelSelectMenu::on_pushButton1_clicked() {
    *level = 1;
    this->accept();
}
