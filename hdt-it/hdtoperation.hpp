#ifndef HDTOPERATION_HPP
#define HDTOPERATION_HPP

#include <QProgressDialog>
#include <HDTListener.hpp>
#include <HDT.hpp>

#include "hdtcachedinfo.hpp"

class HDTOperation : public QObject, public hdt::ProgressListener {
    Q_OBJECT

private:
    hdt::HDT *hdt;
    HDTCachedInfo *hdtInfo;
    QString fileName;
    hdt::RDFNotation notation;
    bool succeded;
    enum Operation {
        HDT_READ,
        RDF_READ,
        HDT_WRITE,
        RDF_WRITE
    } op;
    QProgressDialog dialog;

public:
    HDTOperation(hdt::HDT *hdt);
    HDTOperation(hdt::HDT *hdt, HDTCachedInfo *hdtInfo);
    void saveToRDF(QString &fileName, hdt::RDFNotation notation);
    void saveToHDT(QString &fileName);
    void loadFromRDF(QString &fileName, hdt::RDFNotation notation);
    void loadFromHDT(QString &fileName);
    void execute();
    int exec();
    void notifyProgress(float level, const char *section);
signals:
    void progressChanged(int progress);
    void messageChanged(QString message);
    void processFinished();
    void processFailed(const char *message);
private slots:
    void processFinishedOK();
    void showError(const char *message);
};

#endif // HDTOPERATION_HPP
