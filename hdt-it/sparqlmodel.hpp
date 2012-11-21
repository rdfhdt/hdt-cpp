#ifndef JOINMODEL_HPP
#define JOINMODEL_HPP

#include <QAbstractTableModel>

#include <SingleTriple.hpp>

#include "hdtcontroller.hpp"

class SparqlModel : public QAbstractTableModel {
   Q_OBJECT
private:
   HDTController *hdtController;

   hdt::VarBindingString *binding;
   unsigned int currentIndex;
   unsigned int numresults;
   void find(unsigned int index);
public:
   SparqlModel(HDTController *controller);
   virtual ~SparqlModel();

   void setQuery(QString query);

   int rowCount(const QModelIndex &parent = QModelIndex()) const ;
   int columnCount(const QModelIndex &parent = QModelIndex()) const;
   QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
   QVariant headerData(int section, Qt::Orientation orientation, int role) const;
signals:
};

#endif // JOINMODEL_HPP
