#ifndef TRIPLECOMPONENTMODEL_HPP
#define TRIPLECOMPONENTMODEL_HPP

#include <QAbstractTableModel>

#include <HDTEnums.hpp>

#include "hdtcontroller.hpp"

class HDTController;

class TripleComponentModel : public QAbstractTableModel {
    Q_OBJECT
private:
    HDTController *hdtController;
    hdt::TripleComponentRole tripleComponentRole;
public:
    TripleComponentModel(HDTController *view, hdt::TripleComponentRole compRole);

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

public slots:
    void itemsChanged(unsigned int ini, unsigned int fin);
    void notifyLayoutChanged();
signals:
};

#endif // TRIPLECOMPONENTMODEL_HPP
