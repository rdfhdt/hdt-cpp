#include "headermodel.hpp"
#include <QFont>

HeaderModel::HeaderModel(HDTController *view) : hdtController(view), triples(NULL)
{
    this->updateDatasetChanged();
}

HeaderModel::~HeaderModel()
{
    if(triples!=NULL) {
        delete triples;
        triples = NULL;
    }
}

int HeaderModel::rowCount(const QModelIndex &parent) const
{
    if(hdtController->hasHDT()) {
        return (int)hdtController->getHDT()->getHeader()->getNumberOfElements();
    }
    return 0;
}

int HeaderModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant HeaderModel::data(const QModelIndex &index, int role) const
{
    if(triples == NULL) {
        return QVariant();
    }

    switch(role) {
    case Qt::EditRole:
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    {
        //cout << "HeaderModel.data " << index.row() << "," << index.column() << endl;
        // Compiler complains that by calling findTriple we are modifying internal
        // state, which is illegal due to this function being const. But we need to
        // modify the currentIndex and currentTriple, so we can avoid it.
        HeaderModel *noConstThis = const_cast<HeaderModel *>(this);
        noConstThis->findTriple(index.row());

        switch(index.column()) {
        case 0:
            return currentTriple->getSubject().c_str();
        case 1:
            return currentTriple->getPredicate().c_str();
        case 2:
            return currentTriple->getObject().c_str();
        }

        return QVariant();
    }
    case Qt::FontRole:
    {
        static QFont font;
        font.setPointSize(10);
        return font;
    }

    }
    return QVariant();
}


QVariant HeaderModel::headerData(int section, Qt::Orientation orientation, int role) const
{

    switch(role) {
    case Qt::DisplayRole:
    {
        if(orientation == Qt::Horizontal) {
            switch(section) {
            case 0:
                return tr("Subject");
            case 1:
                return tr("Predicate");
            case 2:
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

void HeaderModel::updateDatasetChanged() {
    if(triples!=NULL) {
        delete triples;
        triples = NULL;
    }

    if(hdtController->hasHDT()) {
        triples = hdtController->getHDT()->getHeader()->search("","","");
        if(triples->hasNext()) {
            currentTriple = triples->next();
        }
    } else {
        triples = NULL;
        currentTriple = NULL;
    }
    currentIndex = 0;

    emit layoutChanged();
}


void HeaderModel::findTriple(unsigned int index)
{
    if(triples == NULL) {
        return;
    }

    if(currentIndex > index) {
        currentIndex = 0;
        triples->goToStart();
        if(triples->hasNext()) {
            currentTriple = triples->next();
        }
    }

    while(currentIndex < index && triples->hasNext()) {
        currentTriple = triples->next();
        currentIndex++;
    }

    //cout << "Access " << currentIndex << " => " << *currentTriple << endl;
}

Qt::ItemFlags HeaderModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable /*| Qt::ItemIsEditable*/;
}

bool HeaderModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {

        HeaderModel *noConstThis = const_cast<HeaderModel *>(this);
        noConstThis->findTriple(index.row());

        string str(value.toString().toLatin1());
        switch(index.column()) {
        case 0:
            currentTriple->setSubject(str);
            break;
        case 1:
            currentTriple->setPredicate(str);
            break;
        case 2:
            currentTriple->setObject(str);
            break;
        }

        emit dataChanged(index, index);
        return true;
    }
    return false;
}






