#include <QMessageBox>
#include <QtGui>

#include "hdtoperation.hpp"

HDTOperation::HDTOperation(hdt::HDT *hdt) : hdt(hdt), hdtInfo(NULL)
{
}

HDTOperation::HDTOperation(hdt::HDT *hdt, HDTCachedInfo *hdtInfo) : hdt(hdt), hdtInfo(hdtInfo)
{
}

void HDTOperation::execute() {
    try {
        switch(op) {
        case HDT_READ:
            hdt->loadFromHDT((istream &)*in, dynamic_cast<ProgressListener *>(this));
            hdtInfo->loadInfo(dynamic_cast<ProgressListener *>(this));
            break;
        case RDF_READ:{
            hdt::RDFParser *parser = hdt::RDFParser::getParser((istream &)*in, notation);
            hdt->loadFromRDF(*parser, dynamic_cast<ProgressListener *>(this));
            delete parser;
            hdtInfo->loadInfo(dynamic_cast<ProgressListener *>(this));
            break;
            }
        case HDT_WRITE:
            hdt->saveToHDT((ostream &)*out, dynamic_cast<ProgressListener *>(this));
            break;
        case RDF_WRITE:{
            hdt::RDFSerializer *serializer = hdt::RDFSerializer::getSerializer((ostream &)*out, notation);
            hdt->saveToRDF(*serializer, dynamic_cast<ProgressListener *>(this));
            delete serializer;
            break;
            }
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

int HDTOperation::exec()
{
    QProgressDialog dialog;
    dialog.setRange(0,100);

    switch(op) {
    case HDT_READ:
        dialog.setWindowTitle(tr("Loading HDT File"));
        break;
    case RDF_READ:
        dialog.setWindowTitle(tr("Importing RDF File to HDT"));
        break;
    case HDT_WRITE:
        dialog.setWindowTitle(tr("Saving HDT File"));
        break;
    case RDF_WRITE:
        dialog.setWindowTitle(tr("Exporting HDT File to RDF"));
        break;
    }

    QPushButton btn;
    btn.setEnabled(false);
    btn.setText(tr("Cancel"));
    dialog.setCancelButton(&btn);
    dialog.setFixedSize(300,130);

    connect(this, SIGNAL(progressChanged(int)), &dialog, SLOT(setValue(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(messageChanged(QString)), &dialog, SLOT(setLabelText(QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(processFinished()), &dialog, SLOT(close()), Qt::QueuedConnection);

    QtConcurrent::run(this, &HDTOperation::execute);
    return dialog.exec();
}

