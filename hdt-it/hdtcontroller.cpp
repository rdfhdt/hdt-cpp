#include <iostream>
#include <fstream>

#include <inttypes.h>

#include <HDTManager.hpp>
#include <HDTVocabulary.hpp>
#include <SingleTriple.hpp>

#include <QtGui>

#include "hdtcontroller.hpp"
#include "hdtoperation.hpp"
#include "hdtcachedinfo.hpp"
#include "stringutils.hpp"
#include "resultcounter.hpp"

#define SHOW_DIALOGS

HDTController::HDTController(QObject *parent) :
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

HDTController::~HDTController() {
    closeHDT();
    delete subjectModel;
    delete predicateModel;
    delete objectModel;
    delete searchResultsModel;
    delete headerModel;
    delete predicateStatus;
    delete regexModel;
}

void HDTController::updateOnHDTChanged()
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

void HDTController::importRDFFile(QString file, const string &baseUri, hdt::RDFNotation notation, hdt::HDTSpecification &spec)
{
    closeHDT();

    HDTOperation *hdtop = new HDTOperation();
    hdtop->loadFromRDF(spec, file, notation, baseUri);
    int result = hdtop->exec();
    if(result==0) {
        this->hdt = hdtop->getHDT();
        this->hdtCachedInfo = hdtop->getHDTInfo();
        this->fileName = file;

        updateOnHDTChanged();
    }
    delete hdtop;
}

void HDTController::openHDTFile(QString file)
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

void HDTController::saveHDTFile(QString file)
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

void HDTController::exportRDFFile(QString file, hdt::RDFNotation notation)
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

void HDTController::exportResultsRDFFile(QString file, hdt::RDFNotation notation)
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


void HDTController::closeHDT()
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


TripleComponentModel * HDTController::getSubjectModel()
{
    return subjectModel;
}

TripleComponentModel * HDTController::getPredicateModel()
{
    return predicateModel;
}
TripleComponentModel * HDTController::getObjectModel()
{
    return objectModel;
}

SearchResultsModel * HDTController::getSearchResultsModel()
{
    return searchResultsModel;
}

HeaderModel * HDTController::getHeaderModel()
{
    return headerModel;
}

RegexModel *HDTController::getRegexModel()
{
    return regexModel;
}

PredicateStatus *HDTController::getPredicateStatus()
{
    return predicateStatus;
}

HDTCachedInfo *HDTController::getHDTCachedInfo()
{
    return hdtCachedInfo;
}

hdt::HDT *HDTController::getHDT()
{
    return hdt;
}

bool HDTController::hasHDT()
{
    return hdt!=NULL;
}

void HDTController::setSearchPattern(hdt::TripleString &pattern)
{
    if(hdt==NULL) {
        return;
    }

    if(searchPatternString != pattern) {
        try {
            this->searchPatternString = pattern;

            hdt->getDictionary()->tripleStringtoTripleID(pattern, searchPatternID);

            // Reset iterator
            if(iteratorResults!=NULL) {
                delete iteratorResults;
                iteratorResults=NULL;
            }
            numResults=0;

            // Check not found
            if( (searchPatternID.getSubject()==0 && !pattern.getSubject().empty()) ||
                    (searchPatternID.getPredicate()==0 && !pattern.getPredicate().empty()) ||
                    (searchPatternID.getObject()==0 && !pattern.getObject().empty()) ) {

            } else {
                iteratorResults = hdt->getTriples()->search(searchPatternID);
            }
            updateNumResults();

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

hdt::TripleID & HDTController::getSearchPatternID()
{
    return searchPatternID;
}

hdt::TripleString & HDTController::getSearchPatternString()
{
    return searchPatternString;
}

hdt::TripleID HDTController::getSelectedTriple()
{
    return selectedTriple;
}

void HDTController::setSelectedTriple(hdt::TripleID &selected)
{
    selectedTriple = selected;
}

void HDTController::clearSelectedTriple()
{
    selectedTriple.clear();
}

quint64 inline DIST(quint64 x1, quint64 x2, quint64 y1, quint64 y2) {
        return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

void HDTController::selectNearestTriple(int subject, int predicate, int object)
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

vector<hdt::TripleID> & HDTController::getTriples()
{
    return hdtCachedInfo->getTriples();
}

size_t HDTController::getNumResults()
{
    return numResults;
}

void HDTController::numResultsValueChanged(int numResults)
{
    this->numResults = numResults;
    searchResultsModel->updateNumResultsChanged();
    emit numResultsChanged(numResults);
}

void HDTController:: numResultCountFinished()
{

}

void HDTController::updateNumResults()
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

QString HDTController::getFileName()
{
    return fileName;
}




