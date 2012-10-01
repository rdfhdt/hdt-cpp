#include <iostream>
#include <fstream>

#include <inttypes.h>

#include <HDTFactory.hpp>
#include <HDTVocabulary.hpp>
#include <SingleTriple.hpp>

#include <QtGui>

#include "hdtmanager.hpp"
#include "hdtoperation.hpp"
#include "hdtcachedinfo.hpp"
#include "stringutils.hpp"
#include "resultcounter.hpp"

#define SHOW_DIALOGS

HDTManager::HDTManager(QObject *parent) :
    QObject(parent),
    hdt(NULL),
    searchPatternID(0,0,0),
    selectedTriple(0,0,0),
    numResults(0),
    iteratorResults(NULL)
{
    subjectModel = new TripleComponentModel(this, hdt::SUBJECT);
    predicateModel = new TripleComponentModel(this, hdt::PREDICATE);
    objectModel = new TripleComponentModel(this, hdt::OBJECT);
    searchResultsModel = new SearchResultsModel(this);
    headerModel = new HeaderModel(this);
    predicateStatus = new PredicateStatus(this);
    regexModel = new RegexModel(this);
}

HDTManager::~HDTManager() {
    closeHDT();
    delete subjectModel;
    delete predicateModel;
    delete objectModel;
    delete searchResultsModel;
    delete headerModel;
    delete predicateStatus;
    delete regexModel;
}

void HDTManager::updateOnHDTChanged()
{
    this->numResults = hdt->getTriples()->getNumberOfElements();

    subjectModel->notifyLayoutChanged();
    predicateModel->notifyLayoutChanged();
    predicateStatus->refreshAll();
    objectModel->notifyLayoutChanged();
    searchResultsModel->updateResultListChanged();
    headerModel->updateDatasetChanged();
    regexModel->updateDatasetChanged();

    emit datasetChanged();
}

void HDTManager::importRDFFile(QString file, string &baseUri, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    closeHDT();

    hdt::HDT *hdt = hdt::HDTFactory::createHDT(spec);
    HDTCachedInfo *hdtInfo = new HDTCachedInfo(hdt);

#ifdef SHOW_DIALOGS
    HDTOperation *hdtop = new HDTOperation(hdt, hdtInfo);
    hdtop->loadFromRDF(file, notation, baseUri);
    int result = hdtop->exec();
    delete hdtop;
#else
    int result=1;
    try {
        hdt::RDFParser *parser = hdt::RDFParser::getParser(fileName.toAscii(), notation);
        hdt->loadFromRDF(*parser, baseUri);
        hdtInfo->loadInfo();
        result=0;
        delete parser;
    } catch (const char *ex) {
        cout << "Error: " << ex;
    } catch (char *ex) {
        cout << "Error: " << ex;
    }
#endif

    if(result==0) {
        this->hdt = hdt;
        this->hdtCachedInfo = hdtInfo;
        this->fileName = file;

        updateOnHDTChanged();
    } else {
        delete hdt;
        delete hdtInfo;
    }
}

void HDTManager::openHDTFile(QString file)
{
    closeHDT();

    HDTOperation *hdtop = new HDTOperation(file);
    hdtop->loadFromHDT(file);
    int result = hdtop->exec();

    if(result==0) {
        this->hdt = hdtop->getHDT();
        this->hdtCachedInfo = hdtop->getHDTInfo();
        this->fileName = file;

        updateOnHDTChanged();
    }

    delete hdtop;
}

void HDTManager::saveHDTFile(QString file)
{
    if(hdt!=NULL) {
#ifdef SHOW_DIALOGS
        HDTOperation *hdtop = new HDTOperation(hdt);
        hdtop->saveToHDT(file);
        hdtop->exec();
        delete hdtop;
#else
        hdt->saveToHDT(file.toAscii());
#endif
    }
}

void HDTManager::exportRDFFile(QString file, hdt::RDFNotation notation)
{
    if(hdt!=NULL) { 
#ifdef SHOW_DIALOGS
        HDTOperation *hdtop = new HDTOperation(hdt);
        hdtop->saveToRDF(file, notation);
        hdtop->exec();
        delete hdtop;
#else
        hdt::RDFSerializer *serializer = hdt::RDFSerializer::getSerializer(file.toAscii(), notation);
        hdt->saveToRDF(*serializer);
#endif
    }
}

void HDTManager::exportResultsRDFFile(QString file, hdt::RDFNotation notation)
{
    if(hdt!=NULL) {
#ifdef SHOW_DIALOGS
        HDTOperation *hdtop = new HDTOperation(hdt);
        hdt::IteratorTripleString *it = hdt->search(searchPatternString);
        hdtop->exportResults(file, it, getNumResults(), notation);
        hdtop->exec();
        delete hdtop;
#else
        hdt::RDFSerializer *serializer = hdt::RDFSerializer::getSerializer(file.toAscii(), notation);
        hdt::IteratorTripleString *it = hdt->search(searchPatternString);
        serializer->serialize(iterator, getNumResults());
        delete serializer;
        delete iterator;
        break;
#endif
    }
}


void HDTManager::closeHDT()
{
    if(hdt!=NULL) {
        delete hdt;
        hdt = NULL;

        numResults=0;
        subjectModel->notifyLayoutChanged();
        predicateModel->notifyLayoutChanged();
        predicateStatus->refreshAll();
        objectModel->notifyLayoutChanged();
        searchResultsModel->updateResultListChanged();
        headerModel->updateDatasetChanged();
        regexModel->updateDatasetChanged();

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

HeaderModel * HDTManager::getHeaderModel()
{
    return headerModel;
}

RegexModel *HDTManager::getRegexModel()
{
    return regexModel;
}

PredicateStatus *HDTManager::getPredicateStatus()
{
    return predicateStatus;
}

HDTCachedInfo *HDTManager::getHDTCachedInfo()
{
    return hdtCachedInfo;
}

hdt::HDT *HDTManager::getHDT()
{
    return hdt;
}

bool HDTManager::hasHDT()
{
    return hdt!=NULL;
}

void HDTManager::setSearchPattern(hdt::TripleString &pattern)
{
    if(hdt==NULL) {
        return;
    }

    if(searchPatternString != pattern) {
        try {
            this->searchPatternString = pattern;

            hdt->getDictionary()->tripleStringtoTripleID(pattern, searchPatternID);

            if(!searchPatternID.isEmpty()) {
                if(iteratorResults!=NULL) {
                    delete iteratorResults;
                }
                iteratorResults = hdt->getTriples()->search(searchPatternID);

                numResults=0;
                updateNumResults();
            } else {
                numResults = hdt->getTriples()->getNumberOfElements();
                iteratorResults = NULL;
            }

            predicateStatus->selectPredicate(searchPatternID.getPredicate());
        } catch (char *exception){
            numResults = 0;
            iteratorResults = NULL;
        } catch (const char *exception){
            numResults = 0;
            iteratorResults = NULL;
        }
        searchResultsModel->updateResultListChanged();

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

quint64 inline DIST(quint64 x1, quint64 x2, quint64 y1, quint64 y2) {
        return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

void HDTManager::selectNearestTriple(int subject, int predicate, int object)
{
    vector<hdt::TripleID> &triples = getTriples();
    if(triples.size()==0) {
        this->clearSelectedTriple();
        return;
    }

    hdt::TripleID *best = &triples[0];
    unsigned int bestpos = 0;
    quint64 bestdist = DIST(triples[0].getSubject(), subject, triples[0].getObject(), object);

    for (unsigned int i = 0; i < triples.size(); i ++) {
        if (triples[i].match(searchPatternID) && predicateStatus->isPredicateActive(triples[i].getPredicate()-1)) {
            quint64 dist = DIST(triples[i].getSubject(), subject, triples[i].getObject(), object);

            if (dist < bestdist) {
                best = &triples[i];
                bestdist = dist;
                bestpos = i;
                //printf("1New %u, %u, %u, Dist: %u Pos: %u\n", best->getSubject(), best->getPredicate(), best->getObject(), bestdist, bestpos);
            }
        }
    }
    //printf("Found: %u, %u, %u, Dist: %llu\n", best->getSubject(), best->getPredicate(), best->getObject(), bestdist);

    this->setSelectedTriple( *best );
}

vector<hdt::TripleID> & HDTManager::getTriples()
{
    return hdtCachedInfo->getTriples();
}

unsigned int HDTManager::getNumResults()
{
    return numResults;
}

void HDTManager::numResultsValueChanged(int numResults)
{
    this->numResults = numResults;
    searchResultsModel->updateNumResultsChanged();
    emit numResultsChanged(numResults);
}

void HDTManager:: numResultCountFinished()
{

}

void HDTManager::updateNumResults()
{
    if(iteratorResults==NULL) {
        return;
    }

    if(iteratorResults->numResultEstimation()==hdt::EXACT) {
        // Provided, use.
        numResults = iteratorResults->estimatedNumResults();
    } else {
        // Not provided, count.
        while(iteratorResults->hasNext()) {
            iteratorResults->next();
            numResults++;
        }
    }

    searchResultsModel->updateNumResultsChanged();
    emit numResultsChanged(numResults);
}

QString HDTManager::getFileName()
{
    return fileName;
}




