#include "hdtsummarygenerator.hpp"

#include <HDTVocabulary.hpp>
#include <QLocale>

#include "stringutils.hpp"

QString HDTSummaryGenerator::getSummary(hdt::HDT *hdt)
{
    if(!hdt) {
        return tr("No open file.");
    }

    hdt::Header &head = hdt->getHeader();
    hdt::Dictionary &dict = hdt->getDictionary();
    hdt::Triples &triples = hdt->getTriples();
    QString output;
    QLocale loc = QLocale::system();

    output.append(tr("<h3>Dataset:</h3>"));

    quint64 originalSize = head.getPropertyLong("<http://purl.org/hdt/dataset>", hdt::HDTVocabulary::ORIGINAL_SIZE.c_str());
    output.append(tr("<b>Original Size</b>: "));
    output.append(stringutils::sizeHuman(originalSize));
    output.append("<br/>");

    quint64 hdtSize = head.getPropertyLong("<http://purl.org/hdt/dataset>", hdt::HDTVocabulary::HDT_SIZE.c_str());
    output.append(tr("<b>HDT Size</b>: "));
    output.append(stringutils::sizeHuman(hdtSize));
    output.append("<br/>");

    output.append(tr("<b>Compression ratio</b>: "));
    output.append(QString::number(hdtSize*100.0/originalSize, 'f', 2));
    output.append("%<br/>");

#if 0
    output.append("<h3>Header:</h3>");
    output.append(QString(tr("<b>Number of triples</b>: %1<br/>").arg(loc.toString(head.getNumberOfElements())));
#endif

    output.append(tr("<h3>Dictionary:</h3>"));
    output.append(QString(tr("<b>Number of entries</b>: %1<br/>")).arg(loc.toString(dict.getNumberOfElements())));
    output.append(QString(tr("<b>Different subjects</b>: %1<br/>")).arg(loc.toString(dict.getNsubjects())));
    output.append(QString(tr("<b>Different predicates</b>: %1<br/>")).arg(loc.toString(dict.getNpredicates())));
    output.append(QString(tr("<b>Different objects</b>: %1<br/>")).arg(loc.toString(dict.getNobjects())));
    output.append(QString(tr("<b>Shared area</b>: %1<br/>")).arg(loc.toString(dict.getSsubobj())));
    QString dictType = dict.getType().c_str();
    dictType.replace("<", "&lt;");
    dictType.replace(">", "&gt;");
    output.append(QString(tr("<b>Type</b>: <small>")).append(dictType).append("</small><br/>"));

    output.append(tr("<b>Dictionary Size</b>: "));
    output.append(stringutils::sizeHuman(dict.size()));
    output.append(tr("<br/>"));

    output.append(tr("<h3>Triples:</h3>"));
    output.append(QString(tr("<b>Number of triples</b>: %1<br/>")).arg(loc.toString(triples.getNumberOfElements())));

    QString triplesType = triples.getType().c_str();
    triplesType.replace("<", "&lt;");
    triplesType.replace(">", "&gt;");
    output.append(QString(tr("<b>Type</b>: <small>")).append(triplesType).append("</small><br/>"));

    output.append(tr("<b>Triples Size</b>: "));
    output.append(stringutils::sizeHuman(triples.size()));
    output.append(tr("<br/>"));

    output.append(tr("<b>Triples Order</b>: "));
    output.append(head.getProperty("<http://purl.org/hdt/triples>","<http://purl.org/HDT/hdt#triplesstreamOrder>").c_str());
    output.append("<br/>");

    return output;
}
