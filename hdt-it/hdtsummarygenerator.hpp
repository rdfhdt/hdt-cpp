#ifndef HDTSUMMARYGENERATOR_HPP
#define HDTSUMMARYGENERATOR_HPP

#include <QObject>
#include <QString>
#include "hdtmanager.hpp"

class HDTSummaryGenerator : public QObject
{
    Q_OBJECT
public:
    static QString getSummary(HDTManager *manager);
};

#endif // HDTSUMMARYGENERATOR_HPP
