#ifndef HDTSUMMARYGENERATOR_HPP
#define HDTSUMMARYGENERATOR_HPP

#include <QObject>
#include <QString>
#include "hdtcontroller.hpp"

class HDTSummaryGenerator : public QObject
{
    Q_OBJECT
public:
    static QString getSummary(HDTController *hdtController);
};

#endif // HDTSUMMARYGENERATOR_HPP
