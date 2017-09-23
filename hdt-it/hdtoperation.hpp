#ifndef HDTOPERATION_HPP
#define HDTOPERATION_HPP

#include <QProgressDialog>
#include <QMutex>
#include <QMutexLocker>
#include <HDTListener.hpp>
#include <HDT.hpp>
#include <HDTSpecification.hpp>
#include <SingleTriple.hpp>

#include "hdtcachedinfo.hpp"


class HDTOperationDialog : public QProgressDialog {
    Q_OBJECT
public:
    HDTOperationDialog();
    void closeEvent(QCloseEvent *event);
};

class HDTOperation : public QObject, public hdt::ProgressListener {
    Q_OBJECT

private:
    hdt::HDTSpecification spec;
    hdt::HDT *hdt;
    HDTCachedInfo *hdtInfo;
    hdt::IteratorTripleString *iterator;
    unsigned int numResults;
    QString fileName;
    hdt::RDFNotation notation;
    string baseUri;
    char *errorMessage;
    bool isCancelled;
    HDTOperationDialog dialog;

    enum Operation {
        HDT_READ,
        RDF_READ,
        HDT_WRITE,
        RDF_WRITE,
        RESULT_EXPORT
    } op;

public:
    HDTOperation();
    HDTOperation(QString fileName);
    HDTOperation(hdt::HDT *hdt);
    HDTOperation(hdt::HDT *hdt, HDTCachedInfo *hdtInfo);
    void saveToRDF(QString &fileName, hdt::RDFNotation notation);
    void saveToHDT(QString &fileName);
    void loadFromRDF(hdt::HDTSpecification &spec, QString &fileName, hdt::RDFNotation notation, const string &baseUri);
    void loadFromHDT(QString &fileName);
    void exportResults(QString &fileName, hdt::IteratorTripleString *iterator, unsigned int numResults, hdt::RDFNotation notation);
    hdt::HDT *getHDT();
    HDTCachedInfo *getHDTInfo();
    void execute();
    int exec();
    void notifyProgress(float level, const char *section);
    void notifyProgress(float task, float level, const char *section);
signals:
    void progressChanged(int progress);
    void messageChanged(QString message);
    void processFinished(int status);
private slots:
    void cancel();
    void finished();
};

#endif // HDTOPERATION_HPP
