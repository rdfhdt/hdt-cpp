#include "sparqlform.hpp"
#include "ui_sparqlform.h"

#include "sparqlmodel.hpp"
#include "hdtit.hpp"

SparqlForm::SparqlForm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SparqlForm)
{
    ui->setupUi(this);

    HDTit *hdtit = reinterpret_cast<HDTit *>(parent);
    sparqlModel = new SparqlModel(hdtit->getManager());
    ui->sparqlResultTable->setModel(sparqlModel);
}

SparqlForm::~SparqlForm()
{
    delete ui;
}

void SparqlForm::on_pushButton_clicked()
{
    sparqlModel->setQuery(ui->sparqlTextEdit->toPlainText());
    ui->sparqlResultTable->scrollToTop();
}
