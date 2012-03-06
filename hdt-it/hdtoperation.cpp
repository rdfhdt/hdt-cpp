#include <QMessageBox>
#include <QtGui>

#include "hdtoperation.hpp"

//#define OPERATION_CANCELABLE

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


HDTOperation::HDTOperation(hdt::HDT *hdt) : hdt(hdt), hdtInfo(NULL), errorMessage(NULL)
{
}

HDTOperation::HDTOperation(hdt::HDT *hdt, HDTCachedInfo *hdtInfo) : hdt(hdt), hdtInfo(hdtInfo), errorMessage(NULL)
{
}

void HDTOperation::execute() {
    try {
        switch(op) {
        case HDT_READ: {
            hdt::IntermediateListener iListener(dynamic_cast<ProgressListener *>(this));

            iListener.setRange(0,70);
            hdt->loadFromHDT(fileName.toAscii(), &iListener );

            iListener.setRange(70, 90);
            string file = (const char*)fileName.append("cache").toAscii();
            hdtInfo->load(file, &iListener);

            iListener.setRange(90, 100);
	    hdt->generateIndex( &iListener );

            break;
        }
        case RDF_READ:{
            hdt::IntermediateListener iListener(dynamic_cast<ProgressListener *>(this));

            iListener.setRange(0,80);
            hdt->loadFromRDF(fileName.toAscii(), baseUri, notation, &iListener);

            iListener.setRange(80, 90);
            hdtInfo->generateInfo(&iListener);

            iListener.setRange(90, 100);
	    hdt->generateIndex( &iListener );

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
}


void HDTOperation::notifyProgress(float level, const char *section) {
#ifdef OPERATION_CANCELABLE
    if(isCancelled) {
        cout << "Throwing exception to cancel" << endl;
        throw (char *)"Cancelled by user";
    }
#endif
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

void HDTOperation::loadFromRDF(QString &fileName, hdt::RDFNotation notation, string &baseUri)
{
    this->op = RDF_READ;
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


int HDTOperation::exec()
{
    dialog.setRange(0,100);
    dialog.setAutoClose(false);
    dialog.setFixedSize(300,130);

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

    connect(this, SIGNAL(progressChanged(int)), &dialog, SLOT(setValue(int)), Qt::QueuedConnection);
    connect(this, SIGNAL(messageChanged(QString)), &dialog, SLOT(setLabelText(QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(processFinished(int)), &dialog, SLOT(done(int)), Qt::QueuedConnection);
    connect(&dialog, SIGNAL(canceled()), this, SLOT(cancel()));

    isCancelled=false;
    QtConcurrent::run(this, &HDTOperation::execute);
    int result = dialog.exec();

    //cout << "Dialog returned" << endl;

    if(errorMessage) {
        result = 1;
        QMessageBox::critical(NULL, "ERROR", QString(errorMessage) );
    }

    QApplication::alert(QApplication::activeWindow());
    return result;
}

void HDTOperation::cancel()
{
    //cout << "Operation cancelled" << endl;
    isCancelled = true;
}



