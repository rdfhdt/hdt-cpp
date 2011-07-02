#ifndef TRIPLECOMPONENTMODEL_HPP
#define TRIPLECOMPONENTMODEL_HPP

#include <QAbstractTableModel>

#include <HDTEnums.hpp>

#include "hdtmanager.hpp"

class HDTManager;

class TripleComponentModel : public QAbstractTableModel {
    Q_OBJECT
private:
    HDTManager *hdtManager;
    hdt::TripleComponentRole tripleComponentRole;
public:
    TripleComponentModel(QObject *parent, HDTManager *view, hdt::TripleComponentRole compRole);

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void itemsChanged(unsigned int ini, unsigned fin);
    void notifyLayoutChanged();
signals:
};

#endif // TRIPLECOMPONENTMODEL_HPP
