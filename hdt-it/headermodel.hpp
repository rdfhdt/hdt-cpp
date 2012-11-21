#ifndef HEADERMODEL_HPP
#define HEADERMODEL_HPP

#include <QAbstractTableModel>

#include <SingleTriple.hpp>

#include "hdtcontroller.hpp"

class HDTController;

class HeaderModel : public QAbstractTableModel {
    Q_OBJECT
private:
    HDTController *hdtController;
    hdt::IteratorTripleString *triples;
    hdt::TripleString *currentTriple;
    unsigned int currentIndex;

    void findTriple(unsigned int index);

public:
    HeaderModel(HDTController *view);
    ~HeaderModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role);

    void updateDatasetChanged();
signals:
};


#endif // HEADERMODEL_HPP
