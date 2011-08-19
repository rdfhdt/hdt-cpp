#include "hdtit.hpp"
#include "ui_hdtit.h"

#include "hdtspecform.hpp"
#include "ui_hdtspecform.h"

#include "abouthdt.hpp"
#include "hdtsummarygenerator.hpp"

#include <fstream>

HDTit::HDTit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HDTit),
    hdtManager(new HDTManager()),
//    lastDir(QDir::currentPath())
    lastDir("/Users/mck/rdf/hdt/")
{
    ui->setupUi(this);
    //this->setUnifiedTitleAndToolBarOnMac(true);

    ui->matrixView->setManager(hdtManager);
    ui->subjectView->setModel(hdtManager->getSubjectModel());
    ui->predicateView->setModel(hdtManager->getPredicateModel());
    ui->objectView->setModel(hdtManager->getObjectModel());
    ui->resultsTable->setModel(hdtManager->getSearchResultsModel());
    ui->headerView->setModel(hdtManager->getHeaderModel());

    ui->subjectPatternEdit->getSuggestions()->setManager(hdtManager);
    ui->predicatePatternEdit->getSuggestions()->setManager(hdtManager);
    ui->objectPatternEdit->getSuggestions()->setManager(hdtManager);
    ui->subjectPatternEdit->getSuggestions()->setRole(hdt::SUBJECT);
    ui->predicatePatternEdit->getSuggestions()->setRole(hdt::PREDICATE);
    ui->objectPatternEdit->getSuggestions()->setRole(hdt::OBJECT);

    // Context menus
    connect(ui->resultsTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->subjectView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->predicateView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(ui->objectView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    // Camera
    connect(ui->matrixView, SIGNAL(rotationChanged()), this, SLOT(updateViewButtons()));

    // Dataset
    connect(hdtManager, SIGNAL(datasetChanged()), ui->matrixView, SLOT(reloadHDTInfo()));

    // Predicate Status
    connect(ui->predicateCountSlider, SIGNAL(valueChanged(int)), hdtManager->getPredicateStatus(), SLOT(setMinimumPredicateCount(int)));
    connect(hdtManager->getPredicateStatus(), SIGNAL(minimumPredicateCountChanged(int)), ui->predicateCountSlider, SLOT(setValue(int)));
    connect(hdtManager->getPredicateStatus(), SIGNAL(predicatesChanged(unsigned int, unsigned int)), ui->matrixView, SLOT(updateGL()));
    connect(ui->selectAllPredicatesButton, SIGNAL(clicked()), hdtManager->getPredicateStatus(), SLOT(selectAllPredicates()));
    connect(ui->selectNoneButton, SIGNAL(clicked()), hdtManager->getPredicateStatus(), SLOT(selectNonePredicates()));
    connect(hdtManager->getPredicateStatus(), SIGNAL(predicatesChanged(unsigned int,unsigned int)), hdtManager->getPredicateModel(), SLOT(itemsChanged(uint,uint)));
    connect(hdtManager->getPredicateStatus(), SIGNAL(predicateSelected(int)), ui->predicateView, SLOT(selectRow(int)));

    // Search pattern
    connect(hdtManager, SIGNAL(searchPatternChanged()), this, SLOT(refreshSearchPattern()));
    connect(hdtManager, SIGNAL(numResultsChanged(int)), this, SLOT(updateNumResults()));
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
    std::string subject = std::string(ui->subjectPatternEdit->text().toUtf8());
    std::string predicate = std::string(ui->predicatePatternEdit->text().toUtf8());
    std::string object = std::string(ui->objectPatternEdit->text().toUtf8());

    hdt::TripleString ts(subject, predicate, object);
    hdtManager->setSearchPattern(ts);

    this->updateNumResults();

    ui->resultsTable->scrollToTop();

    ui->matrixView->updateGL();

    ui->subjectView->selectRow(hdtManager->getSearchPatternID().getSubject()-1);
    ui->predicateView->selectRow(hdtManager->getSearchPatternID().getPredicate()-1);
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

void HDTit::importRDFFile(QString &file, string &baseUri, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    hdtManager->importRDFFile(file, baseUri, notation, spec);

    hdtChanged(file);
}

void HDTit::hdtChanged(QString &file)
{
    ui->subjectPatternEdit->clear();
    ui->predicatePatternEdit->clear();
    ui->objectPatternEdit->clear();

    ui->predicateCountSlider->setMaximum(hdtManager->getPredicateStatus()->getMaximumPredicateCount()+1);
    ui->predicateCountSlider->setValue(0);
    ui->statusBar->showMessage(file);
    ui->statsLabel->setText(HDTSummaryGenerator::getSummary(hdtManager));
    updateNumResults();

    bool hasDataset = hdtManager->hasHDT();
    ui->actionSaveHDT->setEnabled(hasDataset);
    ui->actionExportRDF->setEnabled(hasDataset);
    ui->subjectPatternEdit->setEnabled(hasDataset);
    ui->predicatePatternEdit->setEnabled(hasDataset);
    ui->objectPatternEdit->setEnabled(hasDataset);

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
        string baseUri = string(hdtSpecForm.getBaseUri().toAscii());
        hdt::RDFNotation notation = hdtSpecForm.getNotation();
        importRDFFile(file, baseUri, notation, spec);
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
    if(!hdtManager->getSearchPatternID().isEmpty()) {
        QMessageBox::warning(0, tr("Export Search Results"), tr("Warning: Only those triples matching the selected search pattern will be exported."));
    }

    QString file = QFileDialog::getSaveFileName(this,tr("Select Output RDF File"), lastDir, tr("RDF Files(*.rdf *.RDF *.n3 *.N3)"), 0, 0 );
    if(!file.isEmpty()) {
        // FIXME: Select notation.
        lastDir = file;
        hdtManager->exportResultsRDFFile(file, hdt::N3);
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
        ui->resultTabs->setCurrentIndex(2);
        ui->matrixView->getCamera().setFrontView();
        updateViewButtons();
    }
}

void HDTit::on_actionLeftView_toggled(bool state)
{
    if(state) {
        ui->resultTabs->setCurrentIndex(2);
        ui->matrixView->getCamera().setLeftView();
        updateViewButtons();
    }
}

void HDTit::on_actionTopView_toggled(bool state)
{
    if(state) {
        ui->resultTabs->setCurrentIndex(2);
        ui->matrixView->getCamera().setTopView();
        updateViewButtons();
    }
}

void HDTit::on_action3Dview_toggled(bool state)
{
    if(state) {
        ui->resultTabs->setCurrentIndex(2);
        ui->matrixView->getCamera().set3DView();
        updateViewButtons();
    }
}

void HDTit::on_actionReset_triggered()
{
    ui->matrixView->getCamera().toDefaultValuesAnimated();
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

void HDTit::setPatternContextSubject()
{
    ui->subjectPatternEdit->setText(lastContextMenuTable->model()->data(lastContextMenuCell).toString());
}

void HDTit::setPatternContextPredicate()
{
    ui->predicatePatternEdit->setText(lastContextMenuTable->model()->data(lastContextMenuCell).toString());
}

void HDTit::setPatternContextObject()
{
    ui->objectPatternEdit->setText(lastContextMenuTable->model()->data(lastContextMenuCell).toString());
}

void HDTit::copyResultTableSelection()
{
    QApplication::clipboard()->setText(hdtManager->getSearchResultsModel()->data(lastContextMenuCell).toString());
}

void HDTit::on_actionAbout_triggered()
{
    Abouthdt aboutForm;
    aboutForm.exec();
}

void HDTit::on_actionFull_Screen_triggered()
{
    static bool maximized=false;
    if(isFullScreen()) {
        if(maximized) {
            showMaximized();
        } else {
            showNormal();
        }
    } else {
        maximized = isMaximized();
        showFullScreen();
    }
}

void HDTit::showContextMenu(QPoint pos)
{
    lastContextMenuTable = qobject_cast<QTableView*>(QObject::sender());
    if(lastContextMenuTable==NULL) {
        return;
    }
    lastContextMenuCell = lastContextMenuTable->indexAt(pos);
    if(!lastContextMenuCell.isValid()){
        return;
    }

    QMenu menu;
#if 0
    menu.addAction(tr("Copy")), this, this, SLOT(copyResultTableSelection());
    menu.addSeparator();
#endif
    menu.addAction(tr("Search as Subject"), this, SLOT(setPatternContextSubject()));
    menu.addAction(tr("Search as Predicate"), this, SLOT(setPatternContextPredicate()));
    menu.addAction(tr("Search as Object"), this, SLOT(setPatternContextObject()));
    menu.exec(lastContextMenuTable->mapToGlobal(pos));
}


void HDTit::on_actionClose_triggered()
{
    hdtManager->closeHDT();
    QString str;
    hdtChanged(str);
}
