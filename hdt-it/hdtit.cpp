#include "hdtit.hpp"
#include "ui_hdtit.h"

#include "hdtspecform.hpp"
#include "ui_hdtspecform.h"

#include "abouthdt.hpp"

#include <fstream>

HDTit::HDTit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HDTit),
    hdtManager(new HDTManager()),
    lastDir(QDir::currentPath())
{
    ui->setupUi(this);

    ui->matrixView->setManager(hdtManager);

    ui->subjectView->setModel(hdtManager->getSubjectModel());

    ui->predicateView->setModel(hdtManager->getPredicateModel());

    ui->objectView->setModel(hdtManager->getObjectModel());

    ui->resultsTable->setModel(hdtManager->getSearchResultsModel());

    connect(ui->matrixView, SIGNAL(rotationChanged()), this, SLOT(updateViewButtons()));
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

void HDTit::updateNumResults()
{
    ui->numResultsLabel->setText(
                QString(tr("%1 results found in %2."))
                .arg(QLocale::system().toString(hdtManager->getNumResults()))
                .arg(hdtManager->getTime())
                );
}

void HDTit::searchPatternEdited()
{
    std::string subject = std::string(ui->subjectPatternEdit->text().toAscii());
    std::string predicate = std::string(ui->predicatePatternEdit->text().toAscii());
    std::string object = std::string(ui->objectPatternEdit->text().toAscii());

    hdt::TripleString ts(subject, predicate, object);
    cout << "New search Pattern: " << ts << endl;
    hdtManager->setSearchPattern(ts);

    this->updateNumResults();

    ui->matrixView->updateGL();

    ui->subjectView->selectRow(hdtManager->getSearchPatternID().getSubject()-1);
    ui->objectView->selectRow(hdtManager->getSearchPatternID().getObject()-1);
}

void HDTit::refreshSearchPattern()
{
    if(hdtManager->getHDT()==NULL) {
        ui->subjectPatternEdit->clear();
        ui->predicatePatternEdit->clear();
        ui->objectPatternEdit->clear();
    } else {
        hdt::TripleString &ts = hdtManager->getSearchPatternString();
        ui->subjectPatternEdit->setText(ts.getSubject().c_str());
        ui->predicatePatternEdit->setText(ts.getPredicate().c_str());
        ui->objectPatternEdit->setText(ts.getObject().c_str());
    }
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
    ui->predicateCountSlider->setValue(0);
    ui->statusBar->showMessage(file);
    ui->statsLabel->setText(hdtManager->getStatistics());
    updateNumResults();

    bool dataset = hdtManager->getHDT()!=NULL;
    ui->actionSaveHDT->setEnabled(dataset);
    ui->actionExportRDF->setEnabled(dataset);
    ui->subjectPatternEdit->setEnabled(dataset);
    ui->predicatePatternEdit->setEnabled(dataset);
    ui->objectPatternEdit->setEnabled(dataset);

    refreshSearchPattern();
}



void HDTit::on_actionOpenHDT_triggered()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select HDT File"), lastDir , tr("HDT Files(*.hdt *.HDT)"), 0, 0 );
    if(!file.isEmpty()) {
        lastDir = file;
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
    QString file = QFileDialog::getSaveFileName(this,tr("Select Output HDT File"), lastDir, tr("HDT Files(*.hdt *.HDT)"), 0, 0 );
    if(!file.isEmpty()) {
        lastDir = file;
        hdtManager->saveHDTFile(file);
    }
}

void HDTit::on_actionExportRDF_triggered()
{
    QString file = QFileDialog::getSaveFileName(this,tr("Select Output RDF File"), lastDir, tr("RDF Files(*.rdf *.RDF *.n3 *.N3)"), 0, 0 );
    if(!file.isEmpty()) {
        // FIXME: Select notation.
        lastDir = file;
        hdtManager->exportRDFFile(file, hdt::N3);
    }
}

void HDTit::updateViewButtons()
{
    Camera &cam = ui->matrixView->getCamera();
    ui->actionFrontView->setChecked(cam.isFrontView());
    ui->actionLeftView->setChecked(cam.isLeftView());
    ui->actionTopView->setChecked(cam.isTopView());
    ui->action3Dview->setChecked(cam.is3DView());
}

void HDTit::on_actionFrontView_toggled(bool state)
{
    if(state) {
        ui->resultTabs->setCurrentIndex(1);
        ui->matrixView->getCamera().setFrontView();
        updateViewButtons();
    }
}

void HDTit::on_actionLeftView_toggled(bool state)
{
    if(state) {
        ui->resultTabs->setCurrentIndex(1);
        ui->matrixView->getCamera().setLeftView();
        updateViewButtons();
    }
}

void HDTit::on_actionTopView_toggled(bool state)
{
    if(state) {
        ui->resultTabs->setCurrentIndex(1);
        ui->matrixView->getCamera().setTopView();
        updateViewButtons();
    }
}

void HDTit::on_action3Dview_toggled(bool state)
{
    if(state) {
        ui->resultTabs->setCurrentIndex(1);
        ui->matrixView->getCamera().set3DView();
        updateViewButtons();
    }
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




