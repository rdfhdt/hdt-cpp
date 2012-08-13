#ifndef REGEXMODEL_HPP
#define REGEXMODEL_HPP

#include <QAbstractTableModel>

#include "hdtmanager.hpp"

class HDTManager;

class RegexModel : public QAbstractTableModel
{
    Q_OBJECT

private:
    HDTManager *hdtManager;
    uint32_t numResults;
    uint32_t *results;

public:
    explicit RegexModel(HDTManager *manager);
    ~RegexModel();

    void setQuery(QString query);

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void updateDatasetChanged();
signals:
    
public slots:
    
};

#endif // REGEXMODEL_HPP
