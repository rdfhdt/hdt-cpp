#include <iostream>
#include <fstream>

#include <HDTFactory.hpp>

#include <QtGui>

#include "hdtmanager.hpp"

HDTManager::HDTManager(QObject *parent) :
    QObject(parent),
    hdt(NULL),
    searchPatternID(0,0,0)
{
    subjectModel = new TripleComponentModel(this, this, hdt::SUBJECT);
    predicateModel = new TripleComponentModel(this, this, hdt::PREDICATE);
    objectModel = new TripleComponentModel(this, this, hdt::OBJECT);
    searchResultsModel = new SearchResultsModel(this, this);
}

HDTManager::~HDTManager() {
    closeHDT();
    delete subjectModel;
    delete predicateModel;
    delete objectModel;
}


void HDTManager::updateOnHDTChanged()
{
    activePredicate.resize(hdt->getDictionary().getNpredicates(), true);

    predicateCount.clear();
    predicateCount.resize(hdt->getDictionary().getNpredicates(), 0);
    maxPredicateCount = 0;

    hdt::Triples &t = hdt->getTriples();
    hdt::TripleID pattern(0,0,0);

    hdt::IteratorTripleID *it = t.search(pattern);
    while(it->hasNext()) {
        hdt::TripleID tid = it->next();

        triples.push_back(tid);
        predicateCount[tid.getPredicate()-1]++;

        if(maxPredicateCount<predicateCount[tid.getPredicate()-1]) {
            maxPredicateCount = predicateCount[tid.getPredicate()-1];
        }
    }
    delete it;

    subjectModel->notifyLayoutChanged();
    predicateModel->notifyLayoutChanged();
    objectModel->notifyLayoutChanged();
    searchResultsModel->update();

    emit datasetChanged();
}



void HDTManager::openHDTFile(QString file)
{
    std::ifstream in(file.toAscii());

    if(in.good()) {
        closeHDT();

        hdt = hdt::HDTFactory::createDefaultHDT();
        hdt->loadFromHDT(in);
        in.close();

        updateOnHDTChanged();
    } else {
        QMessageBox::critical(0, "File Error", "Could not open file: "+file);
    }
}

void HDTManager::saveHDTFile(QString file)
{
    if(hdt!=NULL) {
        ofstream out;

        // Save HDT
        out.open(file.toAscii());
        if(out.good()){
            QProgressDialog *dialog = new QProgressDialog("Saving HDT File...","Cancel", 0, 100);
            dialog->show();

            try {
                hdt->saveToHDT(out);
            } catch (char *e) {
                QMessageBox::critical(0, "HDT Save Error", e);
            }
            out.close();

            dialog->close();
            delete dialog;
        } else {
            QMessageBox::critical(0, "File Error", "Could not open file: "+file);
        }

    }
}

#if 0
class MyNotifier : public hdt::ProgressListener {
    QProgressDialog *dialog;
public:
    MyNotifier(QProgressDialog *dialog) : dialog(dialog) {

    }

    void notifyProgress(float level, const char *section) {
        cout << "Progress: " << level << " " << section << endl;
        dialog->setValue(level);
        dialog->setLabelText(section);
        //QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
    }
};
#endif

Wrapper::Wrapper(hdt::HDT *hdt, std::ifstream &in, hdt::RDFNotation &notation, QProgressDialog *dialog) :
    hdt(hdt),
    in(in),
    notation(notation),
    dialog(dialog) {
}

void Wrapper::execute() {
    hdt->loadFromRDF(in, notation, this);
    in.close();
    dialog->close();
    delete &in;
    delete dialog;
    emit hdtProcessFinished();
    delete this;
}


void Wrapper::notifyProgress(float level, const char *section) {
    cout << "Progress: " << level << " " << section << endl;
    dialog->setValue(level);
    dialog->setLabelText(section);
    //QCoreApplication::processEvents(QEventLoop::AllEvents, 500);
}


void HDTManager::importRDFFile(QString file, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    std::ifstream *in = new std::ifstream(file.toAscii());

    QProgressDialog *dialog = new QProgressDialog("Importing RDF File...","Cancel", 0, 100);
    dialog->show();

    if(in->good()) {
        closeHDT();

        hdt = hdt::HDTFactory::createHDT(spec);

#if 0
        Wrapper *wrap = new Wrapper(hdt, *in, notation, dialog);
        connect(wrap, SIGNAL(hdtProcessFinished()), this, SLOT(updateOnHDTChanged()));
        QFuture<void> future = QtConcurrent::run(wrap, &Wrapper::execute);
        //QtConcurrent::run(hdt, &hdt::HDT::loadFromRDF, in, notation, &notifier);
#else
        hdt->loadFromRDF(*in, notation);
        in->close();

        dialog->close();
        delete dialog;
        delete in;

        updateOnHDTChanged();
#endif
    } else {
        QMessageBox::critical(0, "File Error", "Could not open file: "+file);
    }
}

void HDTManager::exportRDFFile(QString file, hdt::RDFNotation notation)
{
    if(hdt!=NULL) {
        ofstream out;

        // Save HDT
        out.open(file.toAscii());
        if(out.good()){
            QProgressDialog *dialog = new QProgressDialog("Exporting RDF File...","Cancel", 0, 100);
            dialog->show();

            try {
                hdt->saveToRDF(out, notation);
            } catch (char *e) {
                QMessageBox::critical(0, "RDF Export Error", e);
            }
            out.close();

            dialog->close();
            delete dialog;
        } else {
            QMessageBox::critical(0, "File Error", "Could not open file: "+file);
        }
    }
}

void HDTManager::closeHDT()
{
    if(hdt!=NULL) {
        activePredicate.clear();
        selectedTriple.clear();
        delete hdt;
        hdt = NULL;

        // FIXME: Connect using signals??
        subjectModel->notifyLayoutChanged();
        predicateModel->notifyLayoutChanged();
        objectModel->notifyLayoutChanged();

        emit datasetChanged();
    }
}


TripleComponentModel * HDTManager::getSubjectModel()
{
    return subjectModel;
}

TripleComponentModel * HDTManager::getPredicateModel()
{
    return predicateModel;
}
TripleComponentModel * HDTManager::getObjectModel()
{
    return objectModel;
}

SearchResultsModel * HDTManager::getSearchResultsModel()
{
    return searchResultsModel;
}

hdt::HDT *HDTManager::getHDT()
{
    return hdt;
}


void HDTManager::selectAllPredicates()
{
//    cout << "selectAllPredicates" << endl;
    for(unsigned int i=0;i<activePredicate.size();i++) {
        activePredicate[i] = true;
    }
    predicateModel->itemsChanged(0, activePredicate.size());
    emit predicatesChanged();
}

void HDTManager::selectNonePredicates()
{
//    cout << "selectNonePredicates" << endl;
    for(unsigned int i=0;i<activePredicate.size();i++) {
        activePredicate[i] = false;
    }
    predicateModel->itemsChanged(0, activePredicate.size());
    emit predicatesChanged();
}

void HDTManager::selectPredicate(unsigned int pred)
{
    cout << "Select predicate: " << pred << endl;
    for(unsigned int i=0;i<activePredicate.size();i++) {
        activePredicate[i] = i==(pred-1);
    }
    predicateModel->itemsChanged(0, activePredicate.size());
    emit predicatesChanged();
}


void HDTManager::setSearchPattern(hdt::TripleString pattern)
{
    if(hdt==NULL) {
        return;
    }

    try {
        searchPatternID = hdt->getDictionary().tripleStringtoTripleID(pattern);
        cout << "Search Pattern ID: " << searchPatternID << endl;
        searchResultsModel->update();
    } catch (char *exception){
        cout << "Exception" << endl;
    }
}

hdt::TripleID & HDTManager::getSearchPattern()
{
    return searchPatternID;
}

void HDTManager::setMinimumPredicateCount(int count)
{
    for(unsigned int i=0;i<activePredicate.size();i++) {
        //cout << "PredicateCount: " << i << " => " << predicateCount[i] << endl;
        activePredicate[i] = predicateCount[i]>=(unsigned int)count;
    }

    predicateModel->itemsChanged(0, activePredicate.size());
    emit predicatesChanged();
}


int HDTManager::getMaxPredicateCount()
{
    return maxPredicateCount-1;
}

bool HDTManager::isPredicateActive(int i)
{
    return activePredicate[i];
}

void HDTManager::setPredicateActive(int i, bool b)
{
    activePredicate[i] = b;
}



QString HDTManager::getStatistics()
{
    if(!hdt) {
        return tr("No open file.");
    }

    hdt::Header &head = hdt->getHeader();
    hdt::Dictionary &dict = hdt->getDictionary();
    hdt::Triples &triples = hdt->getTriples();

    QString output;
    output.append("<h3>Header:</h3>");
    output.append(QString("<b>Number of triples</b>: %1<br/>").arg(head.getNumberOfElements()));

    output.append("<h3>Dictionary:</h3>");
    output.append(QString("<b>Number of entries</b>: %1<br/>").arg(dict.getNumberOfElements()));
    output.append(QString("<b>Different subjects</b>: %1<br/>").arg(dict.getNsubjects()));
    output.append(QString("<b>Different predicates</b>: %1<br/>").arg(dict.getNpredicates()));
    output.append(QString("<b>Different objects</b>: %1<br/>").arg(dict.getNobjects()));
    output.append(QString("<b>Shared area</b>: %1<br/>").arg(dict.getSsubobj()));

    output.append("<h3>Triples:</h3>");
    output.append(QString("<b>Number of triples</b>: %1<br/>").arg(triples.getNumberOfElements()));

    QString type = triples.getType().c_str();
    type.replace("<", "&lt;");
    type.replace(">", "&gt;");
    output.append(QString("<b>Type</b>: <small>").append(type).append("</small><br/>"));

    return output;
}

hdt::TripleID HDTManager::getSelectedTriple()
{
    return selectedTriple;
}

void HDTManager::setSelectedTriple(hdt::TripleID &selected)
{
    selectedTriple = selected;
}

void HDTManager::clearSelectedTriple()
{
    selectedTriple.clear();
}



