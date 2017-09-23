#include "hdtit.hpp"
#include "ui_hdtit.h"

#include "hdtspecform.hpp"
#include "ui_hdtspecform.h"

#include "sparqlform.hpp"
#include "ui_sparqlform.h"

#include "abouthdt.hpp"
#include "hdtsummarygenerator.hpp"

#include <fstream>

HDTit::HDTit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HDTit),
    hdtController(new HDTController()),
    lastDir(QDir::currentPath())
{
    ui->setupUi(this);
    //this->setUnifiedTitleAndToolBarOnMac(true);

    ui->matrixView->setManager(hdtController);
    ui->subjectView->setModel(hdtController->getSubjectModel());
    ui->predicateView->setModel(hdtController->getPredicateModel());
    ui->objectView->setModel(hdtController->getObjectModel());
    ui->resultsTable->setModel(hdtController->getSearchResultsModel());
    ui->headerView->setModel(hdtController->getHeaderModel());
    ui->regexResultsView->setModel(hdtController->getRegexModel());

    ui->subjectPatternEdit->getSuggestions()->setController(hdtController);
    ui->predicatePatternEdit->getSuggestions()->setController(hdtController);
    ui->objectPatternEdit->getSuggestions()->setController(hdtController);
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
    connect(hdtController, SIGNAL(datasetChanged()), ui->matrixView, SLOT(reloadHDTInfo()));

    // Predicate Status
    connect(ui->predicateCountSlider, SIGNAL(valueChanged(int)), hdtController->getPredicateStatus(), SLOT(setMinimumPredicateCount(int)));
    connect(hdtController->getPredicateStatus(), SIGNAL(minimumPredicateCountChanged(int)), ui->predicateCountSlider, SLOT(setValue(int)));
    connect(hdtController->getPredicateStatus(), SIGNAL(predicatesChanged(unsigned int, unsigned int)), ui->matrixView, SLOT(updateGL()));
    connect(ui->selectAllPredicatesButton, SIGNAL(clicked()), hdtController->getPredicateStatus(), SLOT(selectAllPredicates()));
    connect(ui->selectNoneButton, SIGNAL(clicked()), hdtController->getPredicateStatus(), SLOT(selectNonePredicates()));
    connect(hdtController->getPredicateStatus(), SIGNAL(predicatesChanged(unsigned int,unsigned int)), hdtController->getPredicateModel(), SLOT(itemsChanged(uint,uint)));
    connect(hdtController->getPredicateStatus(), SIGNAL(predicateSelected(int)), ui->predicateView, SLOT(selectRow(int)));

    // Search pattern
    connect(hdtController, SIGNAL(searchPatternChanged()), this, SLOT(refreshSearchPattern()));
    connect(hdtController, SIGNAL(numResultsChanged(int)), this, SLOT(updateNumResults()));
}

HDTit::~HDTit()
{
    delete ui;
    delete hdtController;
}

void HDTit::updateNumResults()
{
    ui->numResultsLabel->setText(
                QString(tr("%1 results found."))
                .arg(QLocale::system().toString((quint64)hdtController->getNumResults()))
                );
}

void HDTit::searchPatternEdited()
{
    std::string subject = ui->subjectPatternEdit->text().toUtf8().constData();
    std::string predicate = ui->predicatePatternEdit->text().toUtf8().constData();
    std::string object = ui->objectPatternEdit->text().toUtf8().constData();

    hdt::TripleString ts(subject, predicate, object);
    hdtController->setSearchPattern(ts);

    this->updateNumResults();

    ui->resultsTable->scrollToTop();

    ui->matrixView->updateGL();

    ui->subjectView->selectRow(hdtController->getSearchPatternID().getSubject()-1);
    ui->predicateView->selectRow(hdtController->getSearchPatternID().getPredicate()-1);
    ui->objectView->selectRow(hdtController->getSearchPatternID().getObject()-1);

}

void HDTit::refreshSearchPattern()
{
    if(hdtController->hasHDT()) {
        hdt::TripleString &ts = hdtController->getSearchPatternString();
        ui->subjectPatternEdit->setText(QString::fromUtf8(ts.getSubject().c_str()));
        ui->predicatePatternEdit->setText(QString::fromUtf8(ts.getPredicate().c_str()));
        ui->objectPatternEdit->setText(QString::fromUtf8(ts.getObject().c_str()));
    } else {
        ui->subjectPatternEdit->clear();
        ui->predicatePatternEdit->clear();
        ui->objectPatternEdit->clear();
    }
}


void HDTit::openHDTFile(QString &file)
{
    hdtController->openHDTFile(file);

    hdtChanged(file);
}

void HDTit::importRDFFile(QString &file, const string &baseUri, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    hdtController->importRDFFile(file, baseUri, notation, spec);

    hdtChanged(file);
}

void HDTit::hdtChanged(QString &file)
{
    ui->subjectPatternEdit->clear();
    ui->predicatePatternEdit->clear();
    ui->objectPatternEdit->clear();

    ui->predicateCountSlider->setMaximum(hdtController->getPredicateStatus()->getMaximumPredicateCount()+1);
    ui->predicateCountSlider->setValue(0);
    ui->statusBar->showMessage(file);
    ui->statsLabel->setText(HDTSummaryGenerator::getSummary(hdtController));
    updateNumResults();

    bool hasDataset = hdtController->hasHDT();
    ui->actionSaveHDT->setEnabled(hasDataset);
    ui->actionExportRDF->setEnabled(hasDataset);
    ui->subjectPatternEdit->setEnabled(hasDataset);
    ui->predicatePatternEdit->setEnabled(hasDataset);
    ui->objectPatternEdit->setEnabled(hasDataset);

    // Enable/Disable substring search tab.
    bool hdtHasSubstring = false;
    if(hdtController->hasHDT()) {
        hdtHasSubstring = hdtController->getHDT()->getDictionary()->getType()==hdt::HDTVocabulary::DICTIONARY_TYPE_LITERAL;
    }
    ui->tabRegex->setEnabled(hdtHasSubstring);
    if(hdtHasSubstring) {
        if(ui->resultTabs->count()<4) {
            ui->resultTabs->insertTab(3, ui->tabRegex, "Substring Search");
        }
    } else {
        ui->resultTabs->removeTab(3);
    }

    refreshSearchPattern();
}



void HDTit::on_actionOpenHDT_triggered()
{
    QString file = QFileDialog::getOpenFileName(this,tr("Select HDT File"), lastDir , tr("HDT Files(*.hdt *.HDT *.hdt.gz *.HDT.gz)"), 0, 0 );
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
        string baseUri = string(hdtSpecForm.getBaseUri().toLatin1());
        hdt::RDFNotation notation = hdtSpecForm.getNotation();
        importRDFFile(file, baseUri, notation, spec);
    }
}

void HDTit::on_actionSaveHDT_triggered()
{
    QString file = QFileDialog::getSaveFileName(this,tr("Select Output HDT File"), lastDir, tr("HDT Files(*.hdt *.HDT)"), 0, 0 );
    if(!file.isEmpty()) {
        lastDir = file;
        hdtController->saveHDTFile(file);
    }
}

void HDTit::on_actionExportRDF_triggered()
{
    if(!hdtController->getSearchPatternID().isEmpty()) {
        QMessageBox::warning(0, tr("Export Search Results"), tr("Warning: Only those triples matching the selected search pattern will be exported."));
    }

    QString file = lastDir;
    file.replace(".hdt",".nt");

    // TODO: Rename lastDir
    file = QFileDialog::getSaveFileName(this,tr("Select Output RDF File"), file, tr("RDF Files(*.rdf *.RDF *.n3 *.N3 *.nt *.NT *.ttl *.TTL)"), 0, 0 );
    if(!file.isEmpty()) {
        // FIXME: Select notation.
        lastDir = file;
        hdtController->exportResultsRDFFile(file, hdt::NTRIPLES);
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
    ui->subjectPatternEdit->setText(hdtController->getSubjectModel()->data(index).toString());
}

void HDTit::setPatternPredicate(QModelIndex index)
{
    ui->predicatePatternEdit->setText(hdtController->getPredicateModel()->data(index).toString());
}

void HDTit::setPatternObject(QModelIndex index)
{
    ui->objectPatternEdit->setText(hdtController->getObjectModel()->data(index).toString());
}

void HDTit::setPatternGlobal(QModelIndex index)
{
    //cout << "Index: " << index.row() << ", " << index.column() << endl;

    switch(index.column()) {
    case 0:
        ui->subjectPatternEdit->setText(hdtController->getSearchResultsModel()->data(index).toString());
        break;
    case 1:
        ui->predicatePatternEdit->setText(hdtController->getSearchResultsModel()->data(index).toString());
        break;
    case 2:
        ui->objectPatternEdit->setText(hdtController->getSearchResultsModel()->data(index).toString());
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
    QApplication::clipboard()->setText(hdtController->getSearchResultsModel()->data(lastContextMenuCell).toString());
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
    hdtController->closeHDT();
    QString str;
    hdtChanged(str);
}

HDTController * HDTit::getManager()
{
    return hdtController;
}

void HDTit::on_actionSparql_triggered()
{
    SparqlForm *jf = new SparqlForm(this);

    jf->show();
}

void HDTit::on_regexSearchButton_clicked()
{
    this->on_regexEdit_editingFinished();
}

void HDTit::on_regexEdit_editingFinished()
{
    hdtController->getRegexModel()->setQuery(ui->regexEdit->text());
    ui->regexResultsView->scrollToTop();
}

void HDTit::on_regexResultsView_doubleClicked(const QModelIndex &index)
{
    if(index.column()==1) {
        ui->objectPatternEdit->setText(hdtController->getRegexModel()->data(index).toString());
        ui->resultTabs->setCurrentIndex(1);
    }
}
