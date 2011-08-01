#include "searchresultsmodel.hpp"
#include <QFont>

SearchResultsModel::SearchResultsModel(QObject *parent, HDTManager *view) : hdtManager(view), triples(NULL)
{
    this->update();
}

SearchResultsModel::~SearchResultsModel()
{
    if(triples!=NULL) {
        delete triples;
        triples = NULL;
    }
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
    if(triples == NULL) {
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

        try {
            switch(index.column()) {
            case 0:
                return d.idToString(currentTriple->getSubject(), hdt::SUBJECT).c_str();
            case 1:
                return d.idToString(currentTriple->getPredicate(), hdt::PREDICATE).c_str();
            case 2:
                return d.idToString(currentTriple->getObject(), hdt::OBJECT).c_str();
            }
        } catch (char *e) {
            cout << "Error accesing dictionary: " << e << endl;
        } catch (const char *e) {
            cout << "Error accesing dictionary: " << e << endl;
        }
        return QVariant();

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
    }

    return QVariant();
}

void SearchResultsModel::update() {
    if(triples!=NULL) {
        delete triples;
        triples = NULL;
    }

    if(hdtManager->hasHDT()) {
        triples = hdtManager->getHDT()->getTriples().search(hdtManager->getSearchPatternID());
        if(triples->hasNext()) {
            currentTriple = triples->next();
        }
    } else {
        triples = NULL;
        currentTriple = NULL;
    }
    currentIndex = 0;
    goingUp = true;
    numResults = hdtManager->getNumResults();

    emit layoutChanged();
}

void SearchResultsModel::findTriple(unsigned int index)
{
    if(triples == NULL) {
        return;
    }

    while(currentIndex > index && triples->hasPrevious()) {
        if(goingUp) {
            // On direction changes, we need to move one extra position.
            goingUp = false;
            currentIndex++;
        }
        currentTriple = triples->previous();
        currentIndex--;
    }

    while(currentIndex < index && triples->hasNext()) {
        if(!goingUp){
            // On direction changes, we need to move one extra position.
            goingUp = true;
            currentIndex--;
        }
        currentTriple = triples->next();
        currentIndex++;
    }
}





