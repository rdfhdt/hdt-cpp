#include "stringutils.hpp"
#include "regexmodel.hpp"

#include <QFont>
#include <QMessageBox>

class QFont;

#include "../hdt-lib/src/dictionary/LiteralDictionary.hpp"

RegexModel::RegexModel(HDTController *manager) : hdtController(manager), numResults(0), results(NULL)
{
}

RegexModel::~RegexModel()
{
    if(results!=NULL) {
        free(results);
    }
}

void RegexModel::setQuery(QString query)
{
    if(hdtController->hasHDT()) {
        hdt::LiteralDictionary *dict = dynamic_cast<hdt::LiteralDictionary *>(hdtController->getHDT()->getDictionary());
        if(dict==NULL) {
            QMessageBox::warning(NULL, tr("ERROR"), tr("This HDT does not support substring search"));
            return;
        }
        if(query.length()==0) {
            numResults = 0;
        } else {
            QByteArray arr = query.toUtf8();
            numResults = dict->substringToId((uchar *)arr.data(), arr.size(), &results);
        }
        emit layoutChanged();
    }
}

int RegexModel::rowCount(const QModelIndex &parent) const
{
    return (int)numResults;
}

int RegexModel::columnCount(const QModelIndex &parent) const
{
    return 2;
}

QVariant RegexModel::data(const QModelIndex &index, int role) const
{
    if(results==NULL) {
        return QVariant();
    }

    switch(role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    {
        hdt::Dictionary *d = hdtController->getHDT()->getDictionary();

        try {
            switch(index.column()) {
            case 0:
            {
                hdt::Triples *t = hdtController->getHDT()->getTriples();

                hdt::TripleID tripleObject(0, 0, results[index.row()]);
                hdt::IteratorTripleID *it = t->search(tripleObject);

                int nResults = it->estimatedNumResults();
                delete it;

                return nResults;
            }
            case 1: {
                return stringutils::toQString(d->idToString(results[index.row()], hdt::OBJECT).c_str());
            }
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

QVariant RegexModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
    case Qt::DisplayRole:
    {
        if(orientation == Qt::Horizontal) {
            switch(section) {
            case 0:
                return tr("Triples  ");
            case 1:
                return tr("Object Literal");
            }
        } else {
            return QString::number(results[section]);
        }
        break;
    }
    }

    return QVariant();
}

void RegexModel::updateDatasetChanged()
{
    if(results==NULL) {
        free(results);
    }
    numResults = 0;
    results=NULL;

    emit layoutChanged();
}
