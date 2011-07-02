#include "hdtit.hpp"
#include "ui_hdtit.h"

#include "hdtspecform.hpp"
#include "ui_hdtspecform.h"

#include "abouthdt.hpp"

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
    connect(hdtManager, SIGNAL(predicatesChanged()), ui->matrixView, SLOT(updateGL()));
    connect(ui->selectAllPredicatesButton, SIGNAL(clicked()), hdtManager, SLOT(selectAllPredicates()));
    connect(ui->selectNoneButton, SIGNAL(clicked()), hdtManager, SLOT(selectNonePredicates()));

    connect(ui->predicateCountSlider, SIGNAL(valueChanged(int)), hdtManager, SLOT(setMinimumPredicateCount(int)));
    connect(hdtManager, SIGNAL(minimumPredicateCountChanged(int)), ui->predicateCountSlider, SLOT(setValue(int)));

    connect(hdtManager, SIGNAL(searchPatternChanged()), this, SLOT(refreshSearchPattern()));
}

HDTit::~HDTit()
{
    delete ui;
    delete hdtManager;
}

void HDTit::searchPatternEdited()
{
    std::string subject = std::string(ui->subjectPatternEdit->text().toAscii());
    std::string predicate = std::string(ui->predicatePatternEdit->text().toAscii());
    std::string object = std::string(ui->objectPatternEdit->text().toAscii());
    hdt::TripleString ts(subject, predicate, object);
    cout << "Search Pattern: " << ts << endl;
    hdtManager->setSearchPattern(ts);
    ui->numResultsLabel->setText(
                QString("%1 results found in %2.").arg(hdtManager->getNumResults())
                .arg(hdtManager->getSearchResultsModel()->getTime().c_str())
                );
    //ui->resultsTable->resizeColumnsToContents();
    //ui->resultsTable->resizeRowsToContents();
    ui->matrixView->updateGL();

    ui->subjectView->selectRow(hdtManager->getSearchPatternID().getSubject()-1);
    ui->objectView->selectRow(hdtManager->getSearchPatternID().getObject()-1);
}

void HDTit::refreshSearchPattern()
{
    hdt::TripleString &ts = hdtManager->getSearchPatternString();
    ui->subjectPatternEdit->setText(ts.getSubject().c_str());
    ui->predicatePatternEdit->setText(ts.getPredicate().c_str());
    ui->objectPatternEdit->setText(ts.getObject().c_str());
}


void HDTit::openHDTFile(QString &file)
{
    hdtManager->openHDTFile(file);

    hdtChanged(file);
}

void HDTit::importRDFFile(QString &file, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    hdtManager->importRDFFile(file, notation, spec);

    hdtChanged(file);
}

void HDTit::hdtChanged(QString &file)
{
    ui->predicateCountSlider->setMaximum(hdtManager->getMaximumPredicateCount()+1);
    ui->statusBar->showMessage(file);
    ui->statsLabel->setText(hdtManager->getStatistics());
    ui->numResultsLabel->setText(
                QString("%1 results found in %2.").arg(hdtManager->getNumResults())
                .arg(hdtManager->getSearchResultsModel()->getTime().c_str())
                );

    ui->subjectPatternEdit->clear();
    ui->predicatePatternEdit->clear();
    ui->objectPatternEdit->clear();

    ui->actionSaveHDT->setEnabled(hdtManager->getHDT()!=NULL);
    ui->actionExportRDF->setEnabled(hdtManager->getHDT()!=NULL);
}



void HDTit::on_actionOpenHDT_triggered()
{
    static QString base = QDir::homePath();
    QString file = QFileDialog::getOpenFileName(this,tr("Select HDT File"), base , tr("HDT Files(*.hdt *.HDT)"), 0, 0 );
    if(!file.isEmpty()) {
        base = file;
        openHDTFile(file);
    }
}

void HDTit::on_actionImportRDF_triggered()
{
    HDTSpecForm hdtSpecForm;
    int result = hdtSpecForm.exec();
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
    QString file = QFileDialog::getSaveFileName(this,tr("Select Output HDT File"), QDir::homePath(), tr("HDT Files(*.hdt *.HDT)"), 0, 0 );
    if(!file.isEmpty()) {
        hdtManager->saveHDTFile(file);
    }
}

void HDTit::on_actionExportRDF_triggered()
{
    QString file = QFileDialog::getSaveFileName(this,tr("Select Output RDF File"), QDir::homePath(), tr("HDT Files(*.rdf *.RDF *.n3 *.N3)"), 0, 0 );
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
    //cout << "Index: " << index.row() << ", " << index.column() << endl;

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


void HDTit::on_actionAbout_triggered()
{
    Abouthdt aboutForm;
    aboutForm.exec();
}

