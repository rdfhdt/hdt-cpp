#ifndef HDTOPERATION_HPP
#define HDTOPERATION_HPP

#include <QObject>
#include <HDTListener.hpp>
#include <HDT.hpp>

#include "hdtcachedinfo.hpp"

class HDTOperation : public QObject, public hdt::ProgressListener {
    Q_OBJECT

private:
    hdt::HDT *hdt;
    HDTCachedInfo *hdtInfo;
    std::ifstream *in;
    std::ofstream *out;
    hdt::RDFNotation notation;
    enum Operation {
        HDT_READ,
        RDF_READ,
        HDT_WRITE,
        RDF_WRITE
    } op;


public:
    HDTOperation(hdt::HDT *hdt);
    HDTOperation(hdt::HDT *hdt, HDTCachedInfo *hdtInfo);
    void saveToRDF(std::ofstream *out, hdt::RDFNotation notation);
    void saveToHDT(std::ofstream *out);
    void loadFromRDF(std::ifstream *in, hdt::RDFNotation notation);
    void loadFromHDT(std::ifstream *in);
    void execute();
    int exec();
    void notifyProgress(float level, const char *section);
signals:
    void progressChanged(int progress);
    void messageChanged(QString message);
    void processFinished();
    void processFailed();
};

#endif // HDTOPERATION_HPP
