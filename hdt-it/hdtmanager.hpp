#ifndef HDTMANAGER_HPP
#define HDTMANAGER_HPP

#include <HDT.hpp>
#include <QObject>


#include "triplecomponentmodel.hpp"
#include "searchresultsmodel.hpp"

class TripleComponentModel;
class SearchResultsModel;

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
    vector<bool> activePredicate;
    vector<unsigned int> subjectCount;
    vector<unsigned int> predicateCount;
    vector<unsigned int> objectCount;
    int minPredicateCount;
    int maxPredicateCount;
    hdt::TripleID searchPatternID;
    hdt::TripleString searchPatternString;
    unsigned int numResults;
    vector<hdt::TripleID> triples;

public:
    hdt::HDT *getHDT();

    void openHDTFile(QString file);
    void saveHDTFile(QString file);

    void importRDFFile(QString file, hdt::RDFNotation notation, hdt::HDTSpecification &spec);
    void exportRDFFile(QString file, hdt::RDFNotation notation);

    void closeHDT();

    TripleComponentModel *getSubjectModel();
    TripleComponentModel *getPredicateModel();
    TripleComponentModel *getObjectModel();
    SearchResultsModel *getSearchResultsModel();

    void selectTriple(int subject, int predicate, int object);
    void setSelectedTriple(hdt::TripleID &selected);
    hdt::TripleID getSelectedTriple();
    void clearSelectedTriple();

    unsigned int getMinimumPredicateCount();
    unsigned int getMaximumPredicateCount();

    bool isPredicateActive(int i);
    void setPredicateActive(int i, bool b);

    hdt::TripleID &getSearchPatternID();
    hdt::TripleString &getSearchPatternString();
    void setSearchPattern(hdt::TripleString &pattern);

    vector<hdt::TripleID> &getTriples();

    unsigned int getNumResults();

    QString getStatistics();

signals:
    void datasetChanged();
    void predicatesChanged();
    void minimumPredicateCountChanged(int newval);
    void searchPatternChanged();

private slots:
    void updateOnHDTChanged();

public slots:
    void selectPredicate(int pred);
    void selectAllPredicates();
    void selectNonePredicates();
    void setMinimumPredicateCount(int count);
};


#endif // HDTMANAGER_HPP



