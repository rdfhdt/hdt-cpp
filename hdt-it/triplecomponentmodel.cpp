#include "triplecomponentmodel.hpp"

#include <QFont>

TripleComponentModel::TripleComponentModel(QObject *parent, HDTManager *view, hdt::TripleComponentRole compRole) :
    hdtManager(view), tripleComponentRole(compRole)
{

}

int TripleComponentModel::rowCount(const QModelIndex &parent) const
{
    if(hdtManager->getHDT() != NULL) {
        hdt::Dictionary &dict = hdtManager->getHDT()->getDictionary();
        switch(tripleComponentRole) {
        case hdt::SUBJECT:
            return dict.getNsubjects();
        case hdt::PREDICATE:
            return dict.getNpredicates();
        case hdt::OBJECT:
            return dict.getNobjects();
        }
    }
    return 0;
}

int TripleComponentModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant TripleComponentModel::data(const QModelIndex &index, int role) const
{
    if(hdtManager->getHDT() == NULL) {
        return QVariant();
    }

    switch(role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    {
        //cout << "Data: " << index.row() << " role: " << role << " type: " << tripleComponentRole << endl;
        hdt::Dictionary &d = hdtManager->getHDT()->getDictionary();
        try {
            return d.idToString(index.row()+1, tripleComponentRole).c_str();
        } catch (char *e) {
            cout << "Error accessing dictionary: " << e << endl;
        } catch (const char *e) {
            cout << "Error accessing dictionary: " << e << endl;
        }
        return QVariant();
    }
    case Qt::FontRole:
    {
        QFont font;
        font.setPointSize(10);
        return font;
    }
    case Qt::CheckStateRole:
        if(tripleComponentRole==hdt::PREDICATE) {
            return hdtManager->getPredicateStatus()->isPredicateActive(index.row()) ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}

bool TripleComponentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(hdtManager->getHDT() == NULL) {
        return false;
    }

    switch(role) {
        case Qt::CheckStateRole:
        if(tripleComponentRole==hdt::PREDICATE) {
            hdtManager->getPredicateStatus()->setPredicateActive(index.row(), value.toBool());
        }
    }
    return true;
}

QVariant TripleComponentModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    switch(role) {
    case Qt::DisplayRole:
    {
        if(orientation == Qt::Horizontal) {
            switch(tripleComponentRole) {
            case hdt::SUBJECT:
                return tr("Subject");
            case hdt::PREDICATE:
                return tr("Predicate");
            case hdt::OBJECT:
                return tr("Object");
            }
        } else {
            return QString::number(section+1);
        }
        break;
    }
    }

    return QVariant();
}

Qt::ItemFlags TripleComponentModel::flags(const QModelIndex &index) const
{
    if(tripleComponentRole == hdt::PREDICATE) {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
    } else {
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    }
}

void TripleComponentModel::itemsChanged(unsigned int ini, unsigned int fin)
{
    QModelIndex first = createIndex(ini, 1);
    QModelIndex last = createIndex(fin, 1);
    emit dataChanged(first, last);
}

void TripleComponentModel::notifyLayoutChanged()
{
    emit layoutChanged();
}
