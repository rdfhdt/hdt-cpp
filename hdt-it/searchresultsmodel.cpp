#include "stringutils.hpp"
#include "searchresultsmodel.hpp"
#include <QFont>

SearchResultsModel::SearchResultsModel(HDTController *view) : hdtController(view), triples(NULL)
{
    this->updateResultListChanged();
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
    // FIXME: QTableView crashes when returning more than 100 Million rows :(
    return numResults > 100000000 ? 100000000 : numResults;
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
	// cerr << "SearchResultsModel.data " << index.row() << "," << index.column() << endl;
        // Compiler complains that by calling findTriple we are modifying internal
        // state, which is illegal due to this function being const. But we need to
        // modify the currentIndex and currentTriple, so we can avoid it.
        SearchResultsModel *noConstThis = const_cast<SearchResultsModel *>(this);
        noConstThis->findTriple(index.row());

        hdt::Dictionary *d = hdtController->getHDT()->getDictionary();

        try {
            switch(index.column()) {
            case 0:
        return stringutils::toQString(d->idToString(currentTriple->getSubject(), hdt::SUBJECT).c_str());
            case 1:
        return stringutils::toQString(d->idToString(currentTriple->getPredicate(), hdt::PREDICATE).c_str());
            case 2:
        return stringutils::toQString(d->idToString(currentTriple->getObject(), hdt::OBJECT).c_str());
            }
        } catch (char *e) {
            cerr << "Error accesing dictionary: " << e << endl;
        } catch (const char *e) {
            cerr << "Error accesing dictionary: " << e << endl;
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

void SearchResultsModel::updateResultListChanged() {
    if(triples!=NULL) {
        delete triples;
        triples = NULL;
    }

    if(hdtController->hasHDT()) {
        triples = hdtController->getHDT()->getTriples()->search(hdtController->getSearchPatternID());
        if(triples->hasNext()) {
            currentTriple = triples->next();
        }
    } else {
        triples = NULL;
        currentTriple = NULL;
        numResults=0;
    }
    currentIndex = 0;
    goingUp = true;
    numResults = hdtController->getNumResults();

    emit layoutChanged();
}

void SearchResultsModel::updateNumResultsChanged()
{
    unsigned int old = numResults;
    numResults = hdtController->getNumResults();

    if(old!=numResults) {
#if 0
    beginInsertRows(QModelIndex(), old+1, numResults);
    endInsertRows();
    //emit dataChanged(createIndex(old+1,0), createIndex(numResults,2));
#else
    emit layoutChanged();
#endif
    }
}


void SearchResultsModel::findTriple(unsigned int index)
{
    if(triples == NULL) {
        return;
    }

    if(triples->canGoTo() && abs((long int)currentIndex-(long int)index)>5) {
	triples->goTo(index);
	goingUp = true;
        currentTriple = triples->next();
        currentIndex = index;
	//cerr << "Jump: " << currentIndex << " => " << *currentTriple << endl;
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

    //cerr << "Access " << currentIndex << " => " << *currentTriple << endl;
}
