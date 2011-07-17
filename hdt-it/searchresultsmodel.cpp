#include "searchresultsmodel.hpp"
#include <QFont>

SearchResultsModel::SearchResultsModel(QObject *parent, HDTManager *view) : hdtManager(view), triples(NULL)
{
    this->update();
}

int SearchResultsModel::rowCount(const QModelIndex &parent) const
{
    return numResults;
}

int SearchResultsModel::columnCount(const QModelIndex &parent) const
{
    return 3;
}

QVariant SearchResultsModel::data(const QModelIndex &index, int role) const
{
    if(hdtManager->getHDT() == NULL) {
        return QVariant();
    }

    switch(role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    {
        //cout << "SearchResultsModel.data " << index.row() << "," << index.column() << endl;
        // Compiler complains that by calling findTriple we are modifying internal
        // state, which is illegal due to this function being const. But we need to
        // modify the currentIndex and currentTriple, so we can avoid it.
        SearchResultsModel *noConstThis = const_cast<SearchResultsModel *>(this);
        noConstThis->findTriple(index.row());

        hdt::Dictionary &d = hdtManager->getHDT()->getDictionary();

        switch(index.column()) {
        case 0:
            return d.idToString(currentTriple.getSubject(), hdt::SUBJECT).c_str();
        case 1:
            return d.idToString(currentTriple.getPredicate(), hdt::PREDICATE).c_str();
        case 2:
            return d.idToString(currentTriple.getObject(), hdt::OBJECT).c_str();
        }
        break;
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


QVariant SearchResultsModel::headerData(int section, Qt::Orientation orientation, int role) const
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
    case Qt::SizeHintRole:
    {
    }
    }

    return QVariant();
}

void SearchResultsModel::resetIterator()
{
    if(triples!=NULL) {
        delete triples;
    }

    if(hdtManager->getHDT() != NULL) {
        triples = hdtManager->getHDT()->getTriples().search(hdtManager->getSearchPatternID());
        if(triples->hasNext()) {
            currentTriple = *triples->next();
        }
    } else {
        triples = NULL;
        currentTriple.clear();
    }
    currentIndex = 0;
}

void SearchResultsModel::update() {
    resetIterator();

    numResults = 0;
    currentIndex = 0;
#if 0
    if(triples!=NULL) {
        while(triples->hasNext()) {
            triples->next();
            numResults++;
        }
    }
#endif

    numResults = hdtManager->getNumResults();

    //resetIterator();

    emit layoutChanged();
}

void SearchResultsModel::findTriple(unsigned int index)
{
    if(hdtManager->getHDT() == NULL) {
        return;
    }

    if(index<currentIndex) {
        resetIterator();
    }

    while( currentIndex<index && triples->hasNext()) {
        currentTriple = *triples->next();
        currentIndex++;
    }
}
