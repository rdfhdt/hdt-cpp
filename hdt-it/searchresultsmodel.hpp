#ifndef SEARCHRESULTSMODEL_HPP
#define SEARCHRESULTSMODEL_HPP

#include <QAbstractTableModel>

#include <SingleTriple.hpp>

#include "hdtcontroller.hpp"

class HDTController;

class SearchResultsModel : public QAbstractTableModel {
    Q_OBJECT
private:
    HDTController *hdtController;
    hdt::IteratorTripleID *triples;
    hdt::TripleID *currentTriple;
    unsigned int currentIndex;
    unsigned int numResults;
    bool goingUp;

    void findTriple(unsigned int index);

public:
    SearchResultsModel(HDTController *view);
    ~SearchResultsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void updateResultListChanged();
    void updateNumResultsChanged();
signals:
};


#endif // SEARCHRESULTSMODEL_HPP
