#ifndef ABOUTHDT_HPP
#define ABOUTHDT_HPP

#include <QDialog>

namespace Ui {
    class Abouthdt;
}

class Abouthdt : public QDialog
{
    Q_OBJECT

public:
    explicit Abouthdt(QWidget *parent = 0);
    ~Abouthdt();

private:
    Ui::Abouthdt *ui;
};

#endif // ABOUTHDT_HPP
