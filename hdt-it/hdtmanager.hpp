#ifndef HDTMANAGER_HPP
#define HDTMANAGER_HPP

#include <HDT.hpp>
#include <QObject>

#include "hdtcachedinfo.hpp"
#include "triplecomponentmodel.hpp"
#include "searchresultsmodel.hpp"
#include "predicatestatus.hpp"

class TripleComponentModel;
class SearchResultsModel;
class PredicateStatus;

class HDTManager : public QObject
{
    Q_OBJECT

public:
    explicit HDTManager(QObject *parent = 0);
    ~HDTManager();

private:
    hdt::HDT *hdt;
    hdt::TripleID selectedTriple;
    TripleComponentModel *subjectModel;
    TripleComponentModel *predicateModel;
    TripleComponentModel *objectModel;
    SearchResultsModel *searchResultsModel;
    PredicateStatus *predicateStatus;
    HDTCachedInfo *hdtCachedInfo;

    hdt::TripleID searchPatternID;
    hdt::TripleString searchPatternString;
    unsigned int numResults;

    hdt::IteratorTripleID *iteratorResults;

    StopWatch resultsTime;

public:
    hdt::HDT *getHDT();
    bool hasHDT();

    void openHDTFile(QString file);
    void saveHDTFile(QString file);

    void importRDFFile(QString file, hdt::RDFNotation notation, hdt::HDTSpecification &spec);
    void exportRDFFile(QString file, hdt::RDFNotation notation);

    void closeHDT();

    TripleComponentModel *getSubjectModel();
    TripleComponentModel *getPredicateModel();
    TripleComponentModel *getObjectModel();
    SearchResultsModel *getSearchResultsModel();
    PredicateStatus *getPredicateStatus();
    HDTCachedInfo *getHDTCachedInfo();

    void selectNearestTriple(int subject, int predicate, int object);
    void setSelectedTriple(hdt::TripleID &selected);
    hdt::TripleID getSelectedTriple();
    void clearSelectedTriple();

    hdt::TripleID &getSearchPatternID();
    hdt::TripleString &getSearchPatternString();
    void setSearchPattern(hdt::TripleString &pattern);

    vector<hdt::TripleID> &getTriples();

    unsigned int getNumResults();

    QString getTime();

signals:
    void datasetChanged();
    void searchPatternChanged();
    void numResultsChanged(int numResults);

private slots:
    void updateOnHDTChanged();
    void updateNumResults();

public slots:

};


#endif // HDTMANAGER_HPP



