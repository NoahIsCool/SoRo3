#include "drill.h"
#include "ui_drill.h"

drill::drill(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::drill)
{
    ui->setupUi(this);
    ui->progressBar->setValue(/*i don't Know*/25);// load the inital values here
}

drill::~drill()
{
    delete ui;
}
