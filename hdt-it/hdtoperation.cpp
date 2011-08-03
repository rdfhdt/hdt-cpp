#include <QMessageBox>
#include <QtGui>

#include "hdtoperation.hpp"

HDTOperation::HDTOperation(hdt::HDT *hdt) : hdt(hdt), hdtInfo(NULL), succeded(false)
{
}

HDTOperation::HDTOperation(hdt::HDT *hdt, HDTCachedInfo *hdtInfo) : hdt(hdt), hdtInfo(hdtInfo), succeded(false)
{
}

void HDTOperation::execute() {
    try {
        switch(op) {
        case HDT_READ: {
            hdt::IntermediateListener iListener(dynamic_cast<ProgressListener *>(this));

            iListener.setRange(0,70);
            hdt->loadFromHDT(fileName.toAscii(), &iListener );

            iListener.setRange(70, 100);
            hdtInfo->loadInfo(&iListener);

            break;
        }
        case RDF_READ:{
            hdt::IntermediateListener iListener(dynamic_cast<ProgressListener *>(this));

            iListener.setRange(0,90);
            hdt::RDFParser *parser = hdt::RDFParser::getParser(fileName.toAscii(), notation);
            hdt->loadFromRDF(*parser, &iListener);
            delete parser;

            iListener.setRange(90, 100);
            hdtInfo->loadInfo(&iListener);

            break;
            }
        case HDT_WRITE:
            hdt->saveToHDT(fileName.toAscii(), dynamic_cast<ProgressListener *>(this));
            break;
        case RDF_WRITE:{
            hdt::RDFSerializer *serializer = hdt::RDFSerializer::getSerializer(fileName.toAscii(), notation);
            hdt->saveToRDF(*serializer, dynamic_cast<ProgressListener *>(this));
            delete serializer;
            break;
            }
        }
        emit processFinished();
    } catch (char* err) {
        emit processFailed(err);
    } catch (const char* err) {
        emit processFailed(err);
    }
}

void HDTOperation::notifyProgress(float level, const char *section) {
    emit progressChanged((int)level);
    emit messageChanged(QString(section));
}

void HDTOperation::saveToRDF(QString &fileName, hdt::RDFNotation notation)
{
    this->op = RDF_WRITE;
    this->fileName = fileName;
    this->notation = notation;
}

void HDTOperation::saveToHDT(QString &fileName)
{
    this->op = HDT_WRITE;
    this->fileName = fileName;
}

void HDTOperation::loadFromRDF(QString &fileName, hdt::RDFNotation notation)
{
    this->op = RDF_READ;
    this->fileName = fileName;
    this->notation = notation;
}

void HDTOperation::loadFromHDT(QString &fileName)
{
    this->op = HDT_READ;
    this->fileName = fileName;
}

int HDTOperation::exec()
{
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
    connect(this, SIGNAL(processFinished()), this, SLOT(processFinishedOK()), Qt::QueuedConnection);
    connect(this, SIGNAL(processFailed(const char *)), this, SLOT(showError(const char*)), Qt::QueuedConnection);

    QtConcurrent::run(this, &HDTOperation::execute);
    int result = dialog.exec();
    return result || !succeded;
}

void HDTOperation::processFinishedOK()
{
    succeded = true;
    dialog.close();
}

void HDTOperation::showError(const char *message)
{
    succeded = false;
    QMessageBox::critical(NULL, "ERROR", QString(message) );
    dialog.close();
}

