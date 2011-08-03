#ifndef HDTSUMMARYGENERATOR_HPP
#define HDTSUMMARYGENERATOR_HPP

#include <QObject>
#include <QString>
#include <HDT.hpp>

class HDTSummaryGenerator : public QObject
{
    Q_OBJECT
public:

    static QString getSummary(hdt::HDT *hdt);
};

#endif // HDTSUMMARYGENERATOR_HPP
