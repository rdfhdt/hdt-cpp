#ifndef JOINFORM_HPP
#define JOINFORM_HPP

#include <QMainWindow>

#include "sparqlmodel.hpp"

namespace Ui {
    class SparqlForm;
}

class SparqlForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit SparqlForm(QWidget *parent = 0);
    ~SparqlForm();

private slots:
    void on_pushButton_clicked();

private:
    Ui::SparqlForm *ui;
    SparqlModel *sparqlModel;
};

#endif // JOINFORM_HPP
