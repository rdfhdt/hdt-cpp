
#include <QFont>

#include "triplecomponentmodel.hpp"

#include "stringutils.hpp"

TripleComponentModel::TripleComponentModel(HDTManager *view, hdt::TripleComponentRole compRole) :
    hdtManager(view), tripleComponentRole(compRole)
{

}

int TripleComponentModel::rowCount(const QModelIndex &parent) const
{
    int numResults = 0;

    Q_UNUSED(parent);
    if(hdtManager->getHDT() != NULL) {
        hdt::Dictionary *dict = hdtManager->getHDT()->getDictionary();
        switch(tripleComponentRole) {
        case hdt::SUBJECT:
            numResults = dict->getNsubjects();
            break;
        case hdt::PREDICATE:
            numResults = dict->getNpredicates();
            break;
        case hdt::OBJECT:
            numResults = dict->getNobjects();
            break;
        }
    }

    // FIXME: QTableView crashes when returning more than 100 Million rows :(
    return numResults > 100000000 ? 100000000 : numResults;
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
        hdt::Dictionary *d = hdtManager->getHDT()->getDictionary();
        try {
        return stringutils::toQString(d->idToString(index.row()+1, tripleComponentRole).c_str());
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
    Q_UNUSED(index);
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
