#include <iostream>
#include <fstream>

#include <inttypes.h>

#include <HDTFactory.hpp>
#include <HDTVocabulary.hpp>

#include <QtGui>

#include "hdtmanager.hpp"
#include "hdtoperation.hpp"
#include "hdtcachedinfo.hpp"
#include "stringutils.hpp"

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
    this->maxPredicateCount = hdtInfo->maxPredicateCount;
    this->numResults = hdtInfo->numResults;

    activePredicate.clear();
    activePredicate.resize(hdt->getDictionary().getNpredicates(), true);

    subjectModel->notifyLayoutChanged();
    predicateModel->notifyLayoutChanged();
    objectModel->notifyLayoutChanged();
    searchResultsModel->update();

    emit datasetChanged();
}

void HDTManager::importRDFFile(QString file, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    std::ifstream *in = new std::ifstream(file.toAscii());

    if(in->good()) {
        closeHDT();

        hdt::HDT *hdt = hdt::HDTFactory::createHDT(spec);
        HDTCachedInfo *hdtInfo = new HDTCachedInfo(hdt);
        HDTOperation *hdtop = new HDTOperation(hdt, hdtInfo);
        hdtop->loadFromRDF(in, notation);

        int result = hdtop->exec();

        delete hdtop;
        delete in;

        if(result==0) {
            this->hdt = hdt;
            this->hdtInfo = hdtInfo;

            updateOnHDTChanged();
        }
    } else {
        QMessageBox::critical(0, "File Error", "Could not open file: "+file);
    }
}

void HDTManager::openHDTFile(QString file)
{
    std::ifstream *in = new std::ifstream(file.toAscii(), ios::in | ios::binary );

    if(in->good()) {
        closeHDT();

        hdt::HDT *hdt = hdt::HDTFactory::createDefaultHDT();
        HDTCachedInfo *hdtInfo = new HDTCachedInfo(hdt);
        HDTOperation *hdtop = new HDTOperation(hdt, hdtInfo);
        hdtop->loadFromHDT(in);

        int result = hdtop->exec();

        delete hdtop;
        delete in;

        if(result==0) {
            this->hdt = hdt;
            this->hdtInfo = hdtInfo;

            updateOnHDTChanged();
        }
    } else {
        QMessageBox::critical(0, "File Error", "Could not open file: "+file);
    }
}

void HDTManager::saveHDTFile(QString file)
{
    if(hdt!=NULL) {
        ofstream *out = new ofstream(file.toAscii(), ios::out | ios::binary );
        if(out->good()){
            HDTOperation *hdtop = new HDTOperation(hdt);
            hdtop->saveToHDT(out);
            hdtop->exec();

            delete hdtop;
            delete out;
        } else {
            QMessageBox::critical(0, "File Error", "Could not open file: "+file);
        }
    }
}




void HDTManager::exportRDFFile(QString file, hdt::RDFNotation notation)
{
    if(hdt!=NULL) { 
        // Save HDT
        ofstream *out = new ofstream(file.toAscii());
        if(out->good()){
            HDTOperation *hdtop = new HDTOperation(hdt);
            hdtop->saveToRDF(out, notation);
            hdtop->exec();

            delete hdtop;
            delete out;
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
    for(int i=0;i<activePredicate.size();i++) {
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

void HDTManager::setMinimumPredicateCount(int count)
{
    if(count!=minPredicateCount) {
        minPredicateCount = count;
        for(unsigned int i=0;i<activePredicate.size();i++) {
            activePredicate[i] = hdtInfo->predicateCount[i]>=(unsigned int)count;
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
                resultsTime.reset();
                while(it->hasNext()) {
                    it->next();
                    numResults++;
                }
                resultsTime.stop();
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

QString HDTManager::getStatistics()
{
    if(!hdt) {
        return tr("No open file.");
    }

    hdt::Header &head = hdt->getHeader();
    hdt::Dictionary &dict = hdt->getDictionary();
    hdt::Triples &triples = hdt->getTriples();
    QString output;
    QLocale loc = QLocale::system();

    output.append("<h3>Dataset:</h3>");

    unsigned long long originalSize = head.getPropertyLong("<http://purl.org/hdt/dataset>", hdt::HDTVocabulary::ORIGINAL_SIZE.c_str());
    output.append(tr("<b>Original Size</b>: "));
    output.append(stringutils::sizeHuman(originalSize));
    output.append("<br/>");

    unsigned long long hdtSize = head.getPropertyLong("<http://purl.org/hdt/dataset>", hdt::HDTVocabulary::HDT_SIZE.c_str());
    output.append(tr("<b>HDT Size</b>: "));
    output.append(stringutils::sizeHuman(hdtSize));
    output.append("<br/>");

    output.append(tr("<b>Compression ratio</b>: "));
    output.append(QString::number(hdtSize*100.0/originalSize, 'f', 2));
    output.append("%<br/>");

    output.append("<h3>Header:</h3>");
    output.append(QString("<b>Number of triples</b>: %1<br/>").arg(loc.toString(head.getNumberOfElements())));

    output.append("<h3>Dictionary:</h3>");
    output.append(QString("<b>Number of entries</b>: %1<br/>").arg(loc.toString(dict.getNumberOfElements())));
    output.append(QString("<b>Different subjects</b>: %1<br/>").arg(loc.toString(dict.getNsubjects())));
    output.append(QString("<b>Different predicates</b>: %1<br/>").arg(loc.toString(dict.getNpredicates())));
    output.append(QString("<b>Different objects</b>: %1<br/>").arg(loc.toString(dict.getNobjects())));
    output.append(QString("<b>Shared area</b>: %1<br/>").arg(loc.toString(dict.getSsubobj())));
    QString dictType = dict.getType().c_str();
    dictType.replace("<", "&lt;");
    dictType.replace(">", "&gt;");
    output.append(QString("<b>Type</b>: <small>").append(dictType).append("</small><br/>"));

    output.append(tr("<b>Dictionary Size</b>: "));
    output.append(stringutils::sizeHuman(dict.size()));
    output.append(tr("<br/>"));

    output.append("<h3>Triples:</h3>");
    output.append(QString("<b>Number of triples</b>: %1<br/>").arg(loc.toString(triples.getNumberOfElements())));

    QString triplesType = triples.getType().c_str();
    triplesType.replace("<", "&lt;");
    triplesType.replace(">", "&gt;");
    output.append(QString("<b>Type</b>: <small>").append(triplesType).append("</small><br/>"));

    output.append(tr("<b>Triples Size</b>: "));
    output.append(stringutils::sizeHuman(triples.size()));
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
    vector<hdt::TripleID> &triples = getTriples();
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
    return hdtInfo->triples;
}

unsigned int HDTManager::getNumResults()
{
    return numResults;
}

QString HDTManager::getTime()
{
    if(searchPatternID.isEmpty()) {
        return hdtInfo->resultsTime.getRealStr().c_str();
    }
    return resultsTime.getRealStr().c_str();
}



