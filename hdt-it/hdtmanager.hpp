#ifndef HDTMANAGER_HPP
#define HDTMANAGER_HPP

#include <HDT.hpp>
#include <QObject>

#include <QProgressDialog>

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
    unsigned int maxPredicateCount;
    hdt::TripleID searchPatternID;
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

    void setSelectedTriple(hdt::TripleID &selected);
    hdt::TripleID getSelectedTriple();
    void clearSelectedTriple();

    int getMaxPredicateCount();
    bool isPredicateActive(int i);
    void setPredicateActive(int i, bool b);

    hdt::TripleID &getSearchPattern();
    void setSearchPattern(hdt::TripleString &pattern);

    QString getStatistics();

signals:
    void datasetChanged();
    void predicatesChanged();

private slots:
    void updateOnHDTChanged();

public slots:
    void selectPredicate(unsigned int pred);
    void selectAllPredicates();
    void selectNonePredicates();
    void setMinimumPredicateCount(int count);
};

class Wrapper : public QObject, public hdt::ProgressListener {
    Q_OBJECT

private:
    hdt::HDT *hdt;
    ifstream &in;
    hdt::RDFNotation notation;
    QProgressDialog *dialog;
public:
    Wrapper(hdt::HDT *hdt, std::ifstream &in, hdt::RDFNotation &notation, QProgressDialog *dialog);
    void execute();
    void notifyProgress(float level, const char *section);
};


#endif // HDTMANAGER_HPP



