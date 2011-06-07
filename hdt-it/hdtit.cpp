#include "hdtit.hpp"
#include "ui_hdtit.h"

#include "hdtspecform.hpp"
#include "ui_hdtspecform.h"

#include <fstream>

HDTit::HDTit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HDTit)
{
    ui->setupUi(this);

    // QString file = tr("/Users/mck/workspace-cpp/hdt-lib/data/nytimes.hdt");
    // openHDTFile(file);
    connect(ui->matrixView,SIGNAL(cameraChanged()), this, SLOT(set3Dview()));
}

HDTit::~HDTit()
{
    delete ui;
}


void HDTit::on_searchPatternButton_clicked()
{
    std::string subject = std::string(ui->subjectPatternEdit->text().toAscii());
    std::string predicate = std::string(ui->predicatePatternEdit->text().toAscii());
    std::string object = std::string(ui->objectPatternEdit->text().toAscii());
    hdt::TripleString ts(subject, predicate, object);
    cout << "Search Pattern: " << ts << endl;
    ui->matrixView->setSearchPattern(ts);
    ui->matrixView->updateGL();
}

void HDTit::openHDTFile(QString &file)
{
    ui->matrixView->openHDTFile(file);
    ui->listView->setModel(ui->matrixView->getPredicateModel());
    ui->predicateCountSlider->setMaximum(ui->matrixView->getMaxPredicateCount());
    ui->statusBar->showMessage(file);
}

void HDTit::importRDFFile(QString file, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    ui->matrixView->importRDFFile(file, notation, spec);
    ui->listView->setModel(ui->matrixView->getPredicateModel());
    ui->predicateCountSlider->setMaximum(ui->matrixView->getMaxPredicateCount());
    ui->statusBar->showMessage(file);
}



void HDTit::on_actionOpenHDT_triggered()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select HDT File"), tr("/Users/mck/workspace-cpp/hdt-lib/data"), tr("HDT Files(*.hdt *.HDT)"), 0, 0 );
    openHDTFile(file);
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
    QString file = QFileDialog::getSaveFileName(this,tr("Select HDT File"), tr("/Users/mck/workspace-cpp/hdt-lib/data"), tr("HDT Files(*.hdt *.HDT)"), 0, 0 );
    if(file!="") {
        ui->matrixView->saveHDTFile(file);
    }
}

void HDTit::on_actionExportRDF_triggered()
{
    QString file = QFileDialog::getSaveFileName(this,tr("Select HDT File"), tr("/Users/mck/workspace-cpp/hdt-lib/data"), tr("HDT Files(*.hdt *.HDT)"), 0, 0 );
    if(file!="") {
        // FIXME: Select notation.
        ui->matrixView->exportRDFFile(file, hdt::N3);
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

void HDTit::on_actionTest_triggered()
{
    QToolTip::showText(QPoint(20,20), "HOLA", ui->matrixView);
}
