#include "hdtsummarygenerator.hpp"

#include <hdtmanager.hpp>

#include <HDTVocabulary.hpp>
#include <QLocale>
#include <QDateTime>

#include "stringutils.hpp"

QString HDTSummaryGenerator::getSummary(HDTManager *hdtManager)
{
    hdt::HDT *hdt = hdtManager->getHDT();
    if(!hdt) {
        return tr("No open file.");
    }

    hdt::Header &head = hdt->getHeader();
    hdt::Dictionary &dict = hdt->getDictionary();
    hdt::Triples &triples = hdt->getTriples();
    QString output;
    QLocale loc = QLocale::system();

    output.append(tr("<h3>Dataset:</h3>"));
    QString fileName = hdtManager->getFileName();
    output.append(tr("<b>File name</b>: <small>"));
    output.append(fileName);
    output.append("</small><br/>");

    QString baseUri = stringutils::asRich(head.getSubject(hdt::HDTVocabulary::RDF_TYPE.c_str(), hdt::HDTVocabulary::HDT_DATASET.c_str()).c_str());
    output.append(tr("<b>Dataset base URI</b>: <small>"));
    output.append(baseUri);
    output.append("</small><br/>");

    quint64 originalSize = head.getPropertyLong("_:statistics", hdt::HDTVocabulary::ORIGINAL_SIZE.c_str());
    output.append(tr("<b>Original Size</b>: "));
    output.append(stringutils::sizeHuman(originalSize));
    output.append("<br/>");

    quint64 hdtSize = head.getPropertyLong("_:statistics", hdt::HDTVocabulary::HDT_SIZE.c_str());
    output.append(tr("<b>HDT Size</b>: "));
    output.append(stringutils::sizeHuman(hdtSize));
    output.append("<br/>");

    output.append(tr("<b>Compression ratio</b>: "));
    output.append(QString::number(hdtSize*100.0/originalSize, 'f', 2));
    output.append("%<br/>");

    string issued = head.getProperty("_:publicationInformation", hdt::HDTVocabulary::DUBLIN_CORE_ISSUED.c_str());
    if(issued!="") {
        QString qissued =QString(issued.c_str()).replace("\"", "");
        int idx;
        if((idx = qissued.lastIndexOf("+"))!=-1) {
            qissued = qissued.left(idx);
        }
        QDateTime date = QDateTime::fromString(qissued, Qt::ISODate);
        if(date.isValid()) {
            output.append(QString(tr("<b>Issued</b>: %1<br/>")).arg(loc.toString(date.date())));
        }
    }

#if 0
    output.append(tr("<h3>Header:</h3>"));
    output.append(QString(tr("<b>Number of triples</b>: %1<br/>").arg(loc.toString(head.getNumberOfElements())));
#endif

    output.append(tr("<h3>Dictionary:</h3>"));
    output.append(QString(tr("<b>Number of entries</b>: %1<br/>")).arg(loc.toString(dict.getNumberOfElements())));
    output.append(QString(tr("<b>Different subjects</b>: %1<br/>")).arg(loc.toString(dict.getNsubjects())));
    output.append(QString(tr("<b>Different predicates</b>: %1<br/>")).arg(loc.toString(dict.getNpredicates())));
    output.append(QString(tr("<b>Different objects</b>: %1<br/>")).arg(loc.toString(dict.getNobjects())));
    output.append(QString(tr("<b>Shared area</b>: %1<br/>")).arg(loc.toString(dict.getSsubobj())));
    QString dictType = stringutils::asRich(dict.getType().c_str());
    output.append(QString(tr("<b>Type</b>: <small>")).append(dictType).append("</small><br/>"));

    output.append(tr("<b>Dictionary Size</b>: "));
    output.append(stringutils::sizeHuman(dict.size()));
    output.append(tr("<br/>"));

    output.append(tr("<h3>Triples:</h3>"));
    output.append(QString(tr("<b>Number of triples</b>: %1<br/>")).arg(loc.toString(triples.getNumberOfElements())));

    QString triplesType = stringutils::asRich(triples.getType().c_str());
    output.append(QString(tr("<b>Type</b>: <small>")).append(triplesType).append("</small><br/>"));

    output.append(tr("<b>Triples Size</b>: "));
    output.append(stringutils::sizeHuman(triples.size()));
    output.append(tr("<br/>"));

    output.append(tr("<b>Triples Order</b>: "));
    output.append(head.getProperty("_:triples", hdt::HDTVocabulary::TRIPLES_ORDER.c_str()).c_str());
    output.append("<br/>");

    return output;
}
