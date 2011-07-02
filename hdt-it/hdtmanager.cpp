#include <iostream>
#include <fstream>

#include <inttypes.h>

#include <HDTFactory.hpp>
#include <HDTVocabulary.hpp>

#include <QtGui>

#include "hdtmanager.hpp"
#include "hdtoperation.hpp"

HDTManager::HDTManager(QObject *parent) :
    QObject(parent),
    hdt(NULL),
    searchPatternID(0,0,0),
    numResults(0)
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

    triples.clear();
    subjectCount.clear();
    predicateCount.clear();
    objectCount.clear();
    //subjectCount.resize(hdt->getDictionary().getMaxSubjectID(), 0);
    predicateCount.resize(hdt->getDictionary().getMaxPredicateID(), 0);
    //objectCount.resize(hdt->getDictionary().getMaxObjectID(), 0);
    maxPredicateCount = 0;

    hdt::Triples &t = hdt->getTriples();

    unsigned int increment = t.getNumberOfElements()/100000;
    increment = increment < 1 ? 1 : increment;
    numResults = 0;

    hdt::IteratorTripleID *it = t.searchAll();

    while(it->hasNext()) {
        hdt::TripleID *tid = it->next();

        if( (numResults%increment)==0) {
            triples.push_back(*tid);
        }

        //subjectCount[tid->getSubject()-1]++;
        predicateCount[tid->getPredicate()-1]++;
        //objectCount[tid->getObject()-1]++;

        if(maxPredicateCount<predicateCount[tid->getPredicate()-1]) {
            maxPredicateCount = predicateCount[tid->getPredicate()-1];
        }
        numResults++;
    }
    delete it;

    cout << "Rendering: " << triples.size() << " points." << endl;

    subjectModel->notifyLayoutChanged();
    predicateModel->notifyLayoutChanged();
    objectModel->notifyLayoutChanged();
    searchResultsModel->update();

    emit datasetChanged();
}



void HDTManager::openHDTFile(QString file)
{
    std::ifstream *in = new std::ifstream(file.toAscii(), ios::in | ios::binary );

    if(in->good()) {
#if 1
        //closeHDT();

        QProgressDialog dialog;
        dialog.setRange(0,100);
        dialog.setWindowTitle("Loading HDT File");

        QPushButton btn;
        btn.setEnabled(false);
        btn.setText("Cancel");
        dialog.setCancelButton(&btn);
        dialog.resize(250,120);

        hdt::HDT *hdt = hdt::HDTFactory::createDefaultHDT();

        HDTOperation *hdtop = new HDTOperation(hdt);
        hdtop->loadFromHDT(in);

        connect(hdtop, SIGNAL(progressChanged(int)), &dialog, SLOT(setValue(int)), Qt::QueuedConnection);
        connect(hdtop, SIGNAL(messageChanged(QString)), &dialog, SLOT(setLabelText(QString)), Qt::QueuedConnection);
        connect(hdtop, SIGNAL(processFinished()), &dialog, SLOT(close()), Qt::QueuedConnection);

        QtConcurrent::run(hdtop, &HDTOperation::execute);
        int result = dialog.exec();

        delete hdtop;
        delete in;

        if(result==0) {
            this->hdt = hdt;

            updateOnHDTChanged();
        }
#else
        closeHDT();

        hdt = hdt::HDTFactory::createDefaultHDT();
        hdt->loadFromHDT(in);
        in.close();

        updateOnHDTChanged();
#endif
    } else {
        QMessageBox::critical(0, "File Error", "Could not open file: "+file);
    }
}

void HDTManager::saveHDTFile(QString file)
{
    if(hdt!=NULL) {
        ofstream out;
        // Save HDT
        out.open(file.toAscii(), ios::out | ios::binary );
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


void HDTManager::importRDFFile(QString file, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    std::ifstream *in = new std::ifstream(file.toAscii());

    if(in->good()) {

#if 1
        closeHDT();

        QProgressDialog dialog;
        dialog.setRange(0,100);
        dialog.setWindowTitle("Importing RDF File to HDT");

        QPushButton btn;
        btn.setEnabled(false);
        btn.setText("Cancel");
        dialog.setCancelButton(&btn);
        dialog.resize(200,120);

        hdt::HDT *hdt = hdt::HDTFactory::createHDT(spec);

        HDTOperation *hdtop = new HDTOperation(hdt);
        hdtop->loadFromRDF(in, notation);

        connect(hdtop, SIGNAL(progressChanged(int)), &dialog, SLOT(setValue(int)), Qt::QueuedConnection);
        connect(hdtop, SIGNAL(messageChanged(QString)), &dialog, SLOT(setLabelText(QString)), Qt::QueuedConnection);
        connect(hdtop, SIGNAL(processFinished()), &dialog, SLOT(close()), Qt::QueuedConnection);

        QtConcurrent::run(hdtop, &HDTOperation::execute);
        int result = dialog.exec();

        delete hdtop;
        delete in;

        if(result==0) {
            this->hdt = hdt;

            updateOnHDTChanged();
        }

#else
        closeHDT();

        hdt = hdt::HDTFactory::createHDT(spec);

        hdt->loadFromRDF(*in, notation);
        in->close();

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
        // Save HDT
        ofstream out(file.toAscii());
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
        numResults = 0;
        delete hdt;
        hdt = NULL;

        // FIXME: Connect using signals??
#if 1
        subjectModel->notifyLayoutChanged();
        predicateModel->notifyLayoutChanged();
        objectModel->notifyLayoutChanged();

        emit datasetChanged();
#endif
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
    minPredicateCount = 0;
    emit minimumPredicateCountChanged(0);
}

void HDTManager::selectNonePredicates()
{
//    cout << "selectNonePredicates" << endl;
    for(unsigned int i=0;i<activePredicate.size();i++) {
        activePredicate[i] = false;
    }
    predicateModel->itemsChanged(0, activePredicate.size());
    emit predicatesChanged();
    minPredicateCount = 0;
    emit minimumPredicateCountChanged(0);
}

void HDTManager::selectPredicate(int pred)
{
    setMinimumPredicateCount(0);
    cout << "Select predicate: " << pred << endl;
    for(unsigned int i=0;i<activePredicate.size();i++) {
        activePredicate[i] = i==(pred-1);
    }
    predicateModel->itemsChanged(0, activePredicate.size());
    emit predicatesChanged();

#if 0
    searchPatternID.setPredicate(pred);

    hdt::TripleString tmp;
    hdt->getDictionary().tripleIDtoTripleString(searchPatternID, tmp);
    setSearchPattern(tmp);
#endif
}

void HDTManager::setSearchPattern(hdt::TripleString &pattern)
{
    if(hdt==NULL) {
        return;
    }

    if(searchPatternString != pattern) {
        try {
            this->searchPatternString = pattern;

            hdt->getDictionary().tripleStringtoTripleID(pattern, searchPatternID);

            if(!searchPatternID.isEmpty()) {
                hdt::IteratorTripleID *it = hdt->getTriples().search(searchPatternID);

                numResults=0;
                while(it->hasNext()) {
                    it->next();
                    numResults++;
                }
            } else {
                numResults = hdt->getTriples().getNumberOfElements();
            }

            searchResultsModel->update();

            if(searchPatternID.getPredicate()!=0) {
                selectPredicate(searchPatternID.getPredicate());
            } else {
                selectAllPredicates();
            }
            minPredicateCount = 0;
            emit minimumPredicateCountChanged(0);
        } catch (char *exception){
            numResults = 0;
            searchResultsModel->update();
            minPredicateCount = 0;
            emit minimumPredicateCountChanged(0);
        }

        emit searchPatternChanged();
    }
}

hdt::TripleID & HDTManager::getSearchPatternID()
{
    return searchPatternID;
}

hdt::TripleString & HDTManager::getSearchPatternString()
{
    return searchPatternString;
}

void HDTManager::setMinimumPredicateCount(int count)
{
    if(count!=minPredicateCount) {
        for(unsigned int i=0;i<activePredicate.size();i++) {
            //cout << "PredicateCount: " << i << " => " << predicateCount[i] << endl;
            activePredicate[i] = predicateCount[i]>=(unsigned int)count;
        }

        predicateModel->itemsChanged(0, activePredicate.size());
        emit predicatesChanged();
        emit minimumPredicateCountChanged(count);
    }
}


unsigned int HDTManager::getMinimumPredicateCount()
{
    return minPredicateCount;
}

unsigned int HDTManager::getMaximumPredicateCount()
{
    return maxPredicateCount;
}

bool HDTManager::isPredicateActive(int i)
{
    return activePredicate[i];
}

void HDTManager::setPredicateActive(int i, bool b)
{
    activePredicate[i] = b;
    minPredicateCount = 0;
    emit minimumPredicateCountChanged(0);
    emit predicatesChanged();
}





std::string calculateSize(unsigned long long size)
{
    static const char     *sizes[]   = { "EB", "PB", "TB", "GB", "MB", "KB", "B" };
    static const unsigned long long exbibytes = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;

    char result[20];
    unsigned long long multiplier = exbibytes;

    if(size==0) {
        strcpy(result, "0");
    } else {
        for (unsigned int i = 0; i < 7; i++, multiplier /= 1024)
        {
            if (size < multiplier)
                continue;
            if (size % multiplier == 0)
                sprintf(result, "%ju %s", (uintmax_t) (size / multiplier), sizes[i]);
            else
                sprintf(result, "%.2f %s", (float) size / multiplier, sizes[i]);
            break;
        }
    }

    string out(result);
    return out;
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

    output.append("<h3>Dataset:</h3>");

    int originalSize = head.getPropertyInt("<http://purl.org/hdt/dataset>", hdt::HDTVocabulary::ORIGINAL_SIZE.c_str());
    output.append(tr("<b>Original Size</b>: "));
    output.append(calculateSize(originalSize).c_str());
    output.append("<br/>");

    int hdtSize = head.getPropertyInt("<http://purl.org/hdt/dataset>", hdt::HDTVocabulary::HDT_SIZE.c_str());
    output.append(tr("<b>HDT Size</b>: "));
    output.append(calculateSize(hdtSize).c_str());
    output.append("<br/>");

    output.append(tr("<b>Compression ratio</b>: "));
    output.append(QString::number(hdtSize*100.0/originalSize, 'f', 2));
    output.append("%<br/>");

    output.append("<h3>Header:</h3>");
    output.append(QString("<b>Number of triples</b>: %1<br/>").arg(head.getNumberOfElements()));

    output.append("<h3>Dictionary:</h3>");
    output.append(QString("<b>Number of entries</b>: %1<br/>").arg(dict.getNumberOfElements()));
    output.append(QString("<b>Different subjects</b>: %1<br/>").arg(dict.getNsubjects()));
    output.append(QString("<b>Different predicates</b>: %1<br/>").arg(dict.getNpredicates()));
    output.append(QString("<b>Different objects</b>: %1<br/>").arg(dict.getNobjects()));
    output.append(QString("<b>Shared area</b>: %1<br/>").arg(dict.getSsubobj()));
    QString dictType = dict.getType().c_str();
    dictType.replace("<", "&lt;");
    dictType.replace(">", "&gt;");
    output.append(QString("<b>Type</b>: <small>").append(dictType).append("</small><br/>"));

    output.append(tr("<b>Dictionary Size</b>: "));
    output.append(calculateSize(dict.size()).c_str());
    output.append(tr("<br/>"));

    output.append("<h3>Triples:</h3>");
    output.append(QString("<b>Number of triples</b>: %1<br/>").arg(triples.getNumberOfElements()));

    QString triplesType = triples.getType().c_str();
    triplesType.replace("<", "&lt;");
    triplesType.replace(">", "&gt;");
    output.append(QString("<b>Type</b>: <small>").append(triplesType).append("</small><br/>"));

    output.append(tr("<b>Triples Size</b>: "));
    output.append(calculateSize(triples.size()).c_str());
    output.append(tr("<br/>"));

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

unsigned long long inline DIST(unsigned long long x1, unsigned long long x2,
                unsigned long long y1, unsigned long long y2) {
        return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

void HDTManager::selectTriple(int subject, int predicate, int object)
{
    if(triples.size()==0) {
        this->clearSelectedTriple();
        return;
    }

    hdt::TripleID *best = &triples[0];
    unsigned int bestpos = 0;
    unsigned long long bestdist = DIST(triples[0].getSubject(), subject, triples[0].getObject(), object);

    for (unsigned int i = 0; i < triples.size(); i ++) {
        if (triples[i].match(searchPatternID) && this->isPredicateActive(triples[i].getPredicate()-1)) {
            unsigned long long dist = DIST(triples[i].getSubject(), subject, triples[i].getObject(), object);

            if (dist < bestdist) {
                best = &triples[i];
                bestdist = dist;
                bestpos = i;
                //                printf("1New %u, %u, %u, Dist: %u Pos: %u\n", best->getSubject(), best->getPredicate(), best->getObject(), bestdist, bestpos);
            }
        }
    }
    //    printf("Found: %u, %u, %u, Dist: %llu\n", best->getSubject(), best->getPredicate(), best->getObject(), bestdist);

    this->setSelectedTriple( *best );
}

vector<hdt::TripleID> & HDTManager::getTriples()
{
    return triples;
}

unsigned int HDTManager::getNumResults()
{
    return numResults;
}



