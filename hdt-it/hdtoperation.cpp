#include <QMessageBox>
#include <QtGui>

#include <HDTManager.hpp>

#include "hdtoperation.hpp"

#define USE_DIALOG
//#define OPERATION_CANCELABLE

#ifdef __WIN32__
#include <windows.h>
#define sleep(a) Sleep((a)*1000)
#else
#include <iostream>
#include <unistd.h>
#define sleep(a) usleep(a)
#endif


HDTOperationDialog::HDTOperationDialog()
{
}

void HDTOperationDialog::closeEvent(QCloseEvent *event)
{
#ifdef OPERATION_CANCELABLE
    event->accept();
#else
    event->ignore();
#endif
}

HDTOperation::HDTOperation() : hdt(NULL), hdtInfo(NULL), errorMessage(NULL)
{
}

HDTOperation::HDTOperation(QString fileName) : fileName(fileName), hdt(NULL), hdtInfo(NULL), errorMessage(NULL)
{
}

HDTOperation::HDTOperation(hdt::HDT *hdt) : hdt(hdt), hdtInfo(NULL), errorMessage(NULL)
{
}

HDTOperation::HDTOperation(hdt::HDT *hdt, HDTCachedInfo *hdtInfo) : hdt(hdt), hdtInfo(hdtInfo), errorMessage(NULL)
{
}

void HDTOperation::execute() {
    errorMessage=NULL;
    try {
        switch(op) {
        case HDT_READ: {
            hdt::IntermediateListener iListener(dynamic_cast<ProgressListener *>(this));
            iListener.setRange(0,70);

            // TODO: Decompress GZIP here using progress bar.
            // TODO: Detect whether .hdtcache and .hdt.index exist to be more accurate with the progress
#if 1
            hdt = hdt::HDTManager::mapIndexedHDT(fileName.toAscii(), &iListener);
#else            
            hdt = hdt::HDTManager::loadIndexedHDT(fileName.toAscii(), &iListener);
#endif
            iListener.setRange(70, 100);
            hdtInfo = new HDTCachedInfo(hdt);
            if(fileName.endsWith('.gz')){
                fileName.left(fileName.length()-3);
            }
            QString infoFile = fileName + ".hdtcache";
            hdtInfo->load(infoFile, &iListener);

            break;
        }
        case RDF_READ:{
            hdt::IntermediateListener iListener(dynamic_cast<ProgressListener *>(this));

            iListener.setRange(0,80);
            hdt = hdt::HDTManager::generateHDT(fileName.toAscii(), baseUri.c_str(), notation, spec, &iListener);

            iListener.setRange(80, 90);
            hdt = hdt::HDTManager::indexedHDT(hdt);

            iListener.setRange(90, 100);
            hdtInfo = new HDTCachedInfo(hdt);
            hdtInfo->generateGeneralInfo(&iListener);
            hdtInfo->generateMatrix(&iListener);

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
        case RESULT_EXPORT: {
            hdt::RDFSerializer *serializer = hdt::RDFSerializer::getSerializer(fileName.toAscii(), notation);
            serializer->serialize(iterator, dynamic_cast<ProgressListener *>(this), numResults );
            delete serializer;
            delete iterator;
            break;
        }
        }
        emit processFinished(0);
    } catch (char* err) {
        cout << "Error caught: " << err << endl;
        errorMessage = err;
        emit processFinished(1);
    } catch (const char* err) {
        cout << "Error caught: " << err << endl;
        errorMessage = (char *)err;
        emit processFinished(1);
    }
   sleep(1);	// To ensure that dialog receives the signal and closes.
}


void HDTOperation::notifyProgress(float level, const char *section) {
#ifdef OPERATION_CANCELABLE
    if(isCancelled) {
        cout << "Throwing exception to cancel" << endl;
        throw (char *)"Cancelled by user";
    }
#endif
#ifdef USE_DIALOG
    emit progressChanged((int)level);
    emit messageChanged(QString(section));
#endif
}

void HDTOperation::notifyProgress(float task, float level, const char *section)
{
#ifdef OPERATION_CANCELABLE
    if(isCancelled) {
        cout << "Throwing exception to cancel" << endl;
        throw (char *)"Cancelled by user";
    }
#endif
    int levelInt = (int) level;
    if(levelInt<0) {
        levelInt=0;
    }
    if(levelInt>=100) {
        levelInt=99;
    }

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

void HDTOperation::loadFromRDF(hdt::HDTSpecification &spec, QString &fileName, hdt::RDFNotation notation, string &baseUri)
{
    this->op = RDF_READ;
    this->spec = spec;
    this->fileName = fileName;
    this->notation = notation;
    this->baseUri = baseUri;
}

void HDTOperation::loadFromHDT(QString &fileName)
{
    this->op = HDT_READ;
    this->fileName = fileName;
}

void HDTOperation::exportResults(QString &fileName, hdt::IteratorTripleString *iterator, unsigned int numResults, hdt::RDFNotation notation)
{
    this->op = RESULT_EXPORT;
    this->fileName = fileName;
    this->iterator = iterator;
    this->numResults = numResults;
    this->notation = notation;
}

hdt::HDT *HDTOperation::getHDT()
{
    return hdt;
}

HDTCachedInfo *HDTOperation::getHDTInfo()
{
    return hdtInfo;
}


int HDTOperation::exec()
{

#ifdef USE_DIALOG
    dialog.setRange(0,100);
    dialog.setAutoClose(false);
    dialog.setFixedSize(400,130);

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
    case RESULT_EXPORT:
        dialog.setWindowTitle(tr("Exporting results to RDF"));
        break;
    }

#ifndef OPERATION_CANCELABLE
    QPushButton btn;
    btn.setEnabled(false);
    btn.setText(tr("Cancel"));
    dialog.setCancelButton(&btn);
#endif

    QFutureWatcher<void> watcher;

    connect(&watcher, SIGNAL(finished()), this, SLOT(finished()), Qt::QueuedConnection);

    connect(this, SIGNAL(progressChanged(int)), &dialog, SLOT(setValue(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(messageChanged(QString)), &dialog, SLOT(setLabelText(QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(processFinished(int)), &dialog, SLOT(done(int)), Qt::QueuedConnection);

#ifdef OPERATION_CANCELABLE
    connect(&dialog, SIGNAL(canceled()), this, SLOT(cancel()));
#endif

    isCancelled=false;
    QFuture<void> f = QtConcurrent::run(this, &HDTOperation::execute);
    watcher.setFuture(f);

    int result = dialog.exec();

    if(errorMessage) {
        result = 1;
        QMessageBox::critical(NULL, "ERROR", QString(errorMessage) );
    }

    QApplication::alert(QApplication::activeWindow());
    return result;
#else
    this->execute();
    if(errorMessage!=NULL)  {
        return 1;
    } else {
        return 0;
    }
#endif
}

void HDTOperation::cancel()
{
    //cout << "Operation cancelled" << endl;
    isCancelled = true;
}

void HDTOperation::finished()
{
    cout << "Finished! :)" << endl;
    emit processFinished(0);
}



