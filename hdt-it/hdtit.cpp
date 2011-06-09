#include "hdtit.hpp"
#include "ui_hdtit.h"

#include "hdtspecform.hpp"
#include "ui_hdtspecform.h"

#include <fstream>

HDTit::HDTit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HDTit),
    hdtManager(new HDTManager())
{
    ui->setupUi(this);

    ui->matrixView->setManager(hdtManager);

    ui->subjectView->setModel(hdtManager->getSubjectModel());
    ui->subjectView->setColumnWidth(0, 300);

    ui->predicateView->setModel(hdtManager->getPredicateModel());

    ui->objectView->setModel(hdtManager->getObjectModel());
    ui->objectView->setColumnWidth(0, 300);

    ui->resultsTable->setModel(hdtManager->getSearchResultsModel());

    connect(ui->matrixView,SIGNAL(cameraChanged()), this, SLOT(set3Dview()));
    connect(hdtManager, SIGNAL(datasetChanged()), ui->matrixView, SLOT(reloadHDTInfo()));

    QString file = tr("/Users/mck/workspace-cpp/hdt-lib/data/nytimes.hdt");
    //openHDTFile(file);
}

HDTit::~HDTit()
{
    delete ui;
    delete hdtManager;
}

void HDTit::updateSearchPattern()
{
    std::string subject = std::string(ui->subjectPatternEdit->text().toAscii());
    std::string predicate = std::string(ui->predicatePatternEdit->text().toAscii());
    std::string object = std::string(ui->objectPatternEdit->text().toAscii());
    hdt::TripleString ts(subject, predicate, object);
    cout << "Search Pattern: " << ts << endl;
    hdtManager->setSearchPattern(ts);
    ui->numResultsLabel->setText(
                QString("%1 results found in %2.").arg(hdtManager->getSearchResultsModel()->getNumResults())
                .arg(hdtManager->getSearchResultsModel()->getTime().c_str())
                );
    //ui->resultsTable->resizeColumnsToContents();
    //ui->resultsTable->resizeRowsToContents();
    ui->matrixView->updateGL();
}

void HDTit::openHDTFile(QString &file)
{
    hdtManager->openHDTFile(file);

    ui->predicateCountSlider->setMaximum(hdtManager->getMaxPredicateCount());
    ui->statusBar->showMessage(file);
    ui->statsLabel->setText(hdtManager->getStatistics());
    ui->numResultsLabel->setText(
                QString("%1 results found in %2.").arg(hdtManager->getSearchResultsModel()->getNumResults())
                .arg(hdtManager->getSearchResultsModel()->getTime().c_str())
                );

    ui->subjectPatternEdit->clear();
    ui->predicatePatternEdit->clear();
    ui->objectPatternEdit->clear();
}

void HDTit::importRDFFile(QString file, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    hdtManager->importRDFFile(file, notation, spec);

    ui->predicateCountSlider->setMaximum(hdtManager->getMaxPredicateCount());
    ui->statusBar->showMessage(file);
    ui->statsLabel->setText(hdtManager->getStatistics());
    ui->numResultsLabel->setText(QString("%1 results found.").arg(hdtManager->getSearchResultsModel()->getNumResults()));
    ui->numResultsLabel->setText(
                QString("%1 results found in %2.").arg(hdtManager->getSearchResultsModel()->getNumResults())
                .arg(hdtManager->getSearchResultsModel()->getTime().c_str())
                );
}



void HDTit::on_actionOpenHDT_triggered()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select HDT File"), tr("/Users/mck/workspace-cpp/hdt-lib/data"), tr("HDT Files(*.hdt *.HDT)"), 0, 0 );
    if(!file.isEmpty()) {
        openHDTFile(file);
    }
}

void HDTit::on_actionImportRDF_triggered()
{
    HDTSpecForm hdtSpecForm;
    int result = hdtSpecForm.exec();
    cout << "HDTForm opened: " << result << endl;
    if(result>0) {
        hdt::HDTSpecification spec;
        hdtSpecForm.fillHDTSpecification(spec);

        QString file = hdtSpecForm.getFileName();

        hdt::RDFNotation notation = hdtSpecForm.getNotation();
        importRDFFile(file, notation, spec);
    }
}

void HDTit::on_actionSaveHDT_triggered()
{
    QString file = QFileDialog::getSaveFileName(this,tr("Select Output HDT File"), tr("/Users/mck/workspace-cpp/hdt-lib/data"), tr("HDT Files(*.hdt *.HDT)"), 0, 0 );
    if(!file.isEmpty()) {
        hdtManager->saveHDTFile(file);
    }
}

void HDTit::on_actionExportRDF_triggered()
{
    QString file = QFileDialog::getSaveFileName(this,tr("Select Output RDF File"), tr("/Users/mck/workspace-cpp/hdt-lib/data"), tr("HDT Files(*.rdf *.RDF *.n3 *.N3)"), 0, 0 );
    if(!file.isEmpty()) {
        // FIXME: Select notation.
        hdtManager->exportRDFFile(file, hdt::N3);
    }
}

void HDTit::on_actionFrontView_toggled(bool state)
{
    if(state) {
        ui->actionLeftView->setChecked(false);
        ui->actionTopView->setChecked(false);
        ui->action3Dview->setChecked(false);

        ui->matrixView->getCamera().setFrontView();
    }
}

void HDTit::on_actionLeftView_toggled(bool state)
{
    if(state) {
        ui->actionFrontView->setChecked(false);
        ui->actionTopView->setChecked(false);
        ui->action3Dview->setChecked(false);

        ui->matrixView->getCamera().setLeftView();
    }
}

void HDTit::on_actionTopView_toggled(bool state)
{
    if(state) {
        ui->actionFrontView->setChecked(false);
        ui->actionLeftView->setChecked(false);
        ui->action3Dview->setChecked(false);

        ui->matrixView->getCamera().setTopView();
    }
}

void HDTit::on_action3Dview_toggled(bool state)
{
    if(state) {
        ui->actionFrontView->setChecked(false);
        ui->actionLeftView->setChecked(false);
        ui->actionTopView->setChecked(false);

        ui->matrixView->getCamera().set3DView();
    }
}

void HDTit::set3Dview()
{
    ui->actionFrontView->setChecked(false);
    ui->actionLeftView->setChecked(false);
    ui->actionTopView->setChecked(false);
    ui->action3Dview->setChecked(false);
}

void HDTit::setPatternSubject(QModelIndex index)
{
    ui->subjectPatternEdit->setText(hdtManager->getSubjectModel()->data(index).toString());
}

void HDTit::setPatternPredicate(QModelIndex index)
{
    ui->predicatePatternEdit->setText(hdtManager->getPredicateModel()->data(index).toString());
}

void HDTit::setPatternObject(QModelIndex index)
{
    ui->objectPatternEdit->setText(hdtManager->getObjectModel()->data(index).toString());
}

void HDTit::setPatternGlobal(QModelIndex index)
{
    cout << "Index: " << index.row() << ", " << index.column() << endl;

    switch(index.column()) {
    case 0:
        ui->subjectPatternEdit->setText(hdtManager->getSearchResultsModel()->data(index).toString());
        break;
    case 1:
        ui->predicatePatternEdit->setText(hdtManager->getSearchResultsModel()->data(index).toString());
        break;
    case 2:
        ui->objectPatternEdit->setText(hdtManager->getSearchResultsModel()->data(index).toString());
        break;
    }
}
