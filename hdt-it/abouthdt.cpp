#include "abouthdt.hpp"
#include "ui_abouthdt.h"

Abouthdt::Abouthdt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Abouthdt)
{
    ui->setupUi(this);
}

Abouthdt::~Abouthdt()
{
    delete ui;
}
