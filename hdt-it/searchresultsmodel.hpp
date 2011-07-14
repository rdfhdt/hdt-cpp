#ifndef SEARCHRESULTSMODEL_HPP
#define SEARCHRESULTSMODEL_HPP

#include <QAbstractTableModel>

#include <SingleTriple.hpp>

#include "hdtmanager.hpp"

class HDTManager;

class SearchResultsModel : public QAbstractTableModel {
    Q_OBJECT
private:
    HDTManager *hdtManager;
    hdt::IteratorTripleID *triples;
    hdt::TripleID currentTriple;
    unsigned int currentIndex;
    unsigned int numResults;

    void resetIterator();
    void findTriple(unsigned int index);

public:
    SearchResultsModel(QObject *parent, HDTManager *view);

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void update();
signals:
};


#endif // SEARCHRESULTSMODEL_HPP
