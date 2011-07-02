#include <QMessageBox>

#include "hdtoperation.hpp"

HDTOperation::HDTOperation(hdt::HDT *hdt) : hdt(hdt)
{
}

void HDTOperation::execute() {
    try {
        switch(op) {
        case HDT_READ:
            hdt->loadFromHDT((istream &)*in, dynamic_cast<ProgressListener *>(this));
            break;
        case RDF_READ:
            hdt->loadFromRDF((istream &)*in, notation, dynamic_cast<ProgressListener *>(this));
            break;
        case HDT_WRITE:
            hdt->saveToHDT((ostream &)*out, dynamic_cast<ProgressListener *>(this));
            break;
        case RDF_WRITE:
            hdt->saveToRDF((ostream &)*out, notation, dynamic_cast<ProgressListener *>(this));
            break;
        }
        emit processFinished();
    } catch (char* err) {
        QMessageBox::critical(0, "Error", err);
        emit processFailed();
    } catch (const char* err) {
        QMessageBox::critical(0, "Error", err);
        emit processFailed();
    }
}

void HDTOperation::notifyProgress(float level, const char *section) {
    emit progressChanged((int)level);
    emit messageChanged(QString(section));
}

void HDTOperation::saveToRDF(std::ofstream *out, hdt::RDFNotation notation)
{
    this->op = RDF_WRITE;
    this->out = out;
    this->notation = notation;
}

void HDTOperation::saveToHDT(std::ofstream *out)
{
    this->op = HDT_WRITE;
    this->out = out;
}

void HDTOperation::loadFromRDF(std::ifstream *in, hdt::RDFNotation notation)
{
    this->op = RDF_READ;
    this->in = in;
    this->notation = notation;
}

void HDTOperation::loadFromHDT(std::ifstream *in)
{
    this->op = HDT_READ;
    this->in = in;
}

