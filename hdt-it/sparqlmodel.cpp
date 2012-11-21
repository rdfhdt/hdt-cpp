#include "sparqlmodel.hpp"
#include <sstream>

#include <SingleTriple.hpp>

#include <QFont>

SparqlModel::SparqlModel(HDTController *controller) :
    hdtController(controller),
    binding(NULL),
    currentIndex(0),
    numresults(0)
{
}

void SparqlModel::find(unsigned int index) {
    if(currentIndex>index) {
        currentIndex=0;
        binding->goToStart();
        binding->findNext();
    }
    while(currentIndex<index) {
        if(binding->findNext()) {
            currentIndex++;
        } else {
	    numresults = currentIndex+1;
            emit layoutChanged();
            break;
        }
    }
}

SparqlModel::~SparqlModel()
{

}

class SparqlQuery {
public:
        set<string> vars;
        vector<hdt::TripleString> patterns;
};

SparqlQuery parseSparql(string query) {
        struct SparqlQuery output;
        string word;
        unsigned int phase = 0;
        vector<string> pattern;

        stringstream in(query, ios::in);

        while(in >> word) {
                cout << word << endl;
                if(word.size()==0) {
                        continue;
                }
                if(phase==0) {
                        if(word.at(0)=='?') {
                                output.vars.insert(word);
                        } else if(word=="WHERE") {
                                phase++;
                        }
                } else {
                        if(word.at(0)=='.') {
                                if(pattern.size()!=3) {
                                        throw "Pattern should have 3 components";
                                }
                                hdt::TripleString trip(pattern[0], pattern[1], pattern[2]);
                                output.patterns.push_back(trip);
                                pattern.clear();
                        } else {
				if(word.at(0)=='{') {

				} else if(word.at(0)!='.'){
				   cout << "**"<< word << "**" << endl;
				    pattern.push_back(word);
                                }
                        }
                }
        }
        return output;
}

void SparqlModel::setQuery(QString queryTxt)
{
    throw "Not implemented";
#if 0
    SparqlQuery query = parseSparql(string(queryTxt.toAscii()));

    if(binding) {
        delete binding;
    }

    binding = hdtManager->getHDT()->searchJoin(query.patterns, query.vars);
    binding->findNext();
    numresults = binding->estimatedNumResults();
    currentIndex = 0;

    emit layoutChanged();
#endif
}

int SparqlModel::rowCount(const QModelIndex &parent) const {
    return numresults;
}
int SparqlModel::columnCount(const QModelIndex &parent) const {
    if(binding==NULL) {
        return 0;
    }
    return binding->getNumVars();
}
QVariant SparqlModel::data(const QModelIndex &index, int role) const {
    if(binding == NULL) {
        return QVariant();
    }

    switch(role) {
    case Qt::ToolTipRole:
    case Qt::DisplayRole:
    {
        // Compiler complains that by calling findTriple we are modifying internal
        // state, which is illegal due to this function being const. But we need to
        // modify the currentIndex and currentTriple, so we can avoid it.
	SparqlModel *noConstThis = const_cast<SparqlModel *>(this);
        noConstThis->find(index.row());

        return binding->getVar(index.column()).c_str();
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
QVariant SparqlModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(binding==NULL) {
        return QVariant();
    }
    switch(role) {
    case Qt::DisplayRole:
    {
        if(orientation == Qt::Horizontal) {
            return binding->getVarName(section);
        } else {
            return QString::number(section+1);
        }
        break;
    }
    }
    return QVariant();
}
