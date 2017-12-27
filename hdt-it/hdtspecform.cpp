#include <QFileDialog>
#include <QMessageBox>

#include "hdtspecform.hpp"
#include "ui_hdtspecform.h"

#include <QtGui>

HDTSpecForm::HDTSpecForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HDTSpecForm)
{
    ui->setupUi(this);
}

HDTSpecForm::~HDTSpecForm()
{
    delete ui;
}

void HDTSpecForm::on_triplesTypeCombo_currentIndexChanged(int index)
{
    switch(index) {
    case 0:
        // BitmapTriples
        ui->streamXcombo->setEnabled(false);
        ui->streamYcombo->setEnabled(true);
        ui->streamZcombo->setEnabled(true);
        break;
    case 1:
        // TriplesList
        ui->streamXcombo->setEnabled(false);
        ui->streamYcombo->setEnabled(false);
        ui->streamZcombo->setEnabled(false);
        break;
    case 2:
        // PlainTriples
        ui->streamXcombo->setEnabled(true);
        ui->streamYcombo->setEnabled(true);
        ui->streamZcombo->setEnabled(true);
        break;
    case 4:
        // FoQTriples
        ui->streamXcombo->setEnabled(false);
        ui->streamYcombo->setEnabled(false);
        ui->streamZcombo->setEnabled(false);
        break;
    }
    ui->triplesOrderCombo->setEnabled(index!=4);
}

void HDTSpecForm::on_inputFileButton_clicked()
{
    static QString lastDir = QDir::currentPath();
    QString file = QFileDialog::getOpenFileName(this,tr("Select Input RDF File"), lastDir, tr("RDF Files(*)"), 0, 0 );
    if(file!="") {
        lastDir=file;
        if(getBaseUri()=="") {
            ui->baseUriText->setText("<file://"+file+">");
        }

        QString noCompression=file;

        if(file.endsWith(".gz", Qt::CaseInsensitive)) {
            noCompression = file.left(file.length()-3);
        }

        if(noCompression.endsWith(".ttl", Qt::CaseInsensitive)) {
            ui->rdfInputCombo->setCurrentIndex(2);
        } else if(noCompression.endsWith(".nt", Qt::CaseInsensitive)) {
            ui->rdfInputCombo->setCurrentIndex(0);
        } else if(noCompression.endsWith(".n3", Qt::CaseInsensitive)) {
            ui->rdfInputCombo->setCurrentIndex(1);
        } else if(noCompression.endsWith(".rdf", Qt::CaseInsensitive) || noCompression.endsWith(".xml", Qt::CaseInsensitive)) {
            ui->rdfInputCombo->setCurrentIndex(3);
        }
    }
    ui->rdfInputFile->setText(file);
}

QString HDTSpecForm::getFileName()
{
    return ui->rdfInputFile->text();
}

QString HDTSpecForm::getBaseUri()
{
    return ui->baseUriText->text();
}

std::string HDTSpecForm::getStreamType(int index) {
    switch(index) {
    case 0:
        // LogStream
        return hdt::HDTVocabulary::SEQ_TYPE_LOG;
    case 1:
        // IntegerStream
        return hdt::HDTVocabulary::SEQ_TYPE_INT32;
    case 2:
        // HuffmanStream
        return hdt::HDTVocabulary::SEQ_TYPE_HUFFMAN;
    case 3:
        // WaveletStream
        return hdt::HDTVocabulary::SEQ_TYPE_WAVELET;
    }
    return "";
}

void HDTSpecForm::fillHDTSpecification(hdt::HDTSpecification &hdt)
{
    hdt.set("triplesOrder", hdt::getOrderStr((hdt::TripleComponentOrder)(ui->triplesOrderCombo->currentIndex()+1)));

    hdt.set("header.type", hdt::HDTVocabulary::DICTIONARY_TYPE_PLAIN);

    switch(ui->dictionaryTypeCombo->currentIndex()) {
    case 0:
        // FourSectionDictionary
        hdt.set("dictionary.type", hdt::HDTVocabulary::DICTIONARY_TYPE_FOUR);
        break;
    case 1:
        // PlainDictionary
        hdt.set("dictionary.type", hdt::HDTVocabulary::DICTIONARY_TYPE_PLAIN);
        break;
    case 2:
        // LiteralDictionary
        hdt.set("dictionary.type", hdt::HDTVocabulary::DICTIONARY_TYPE_LITERAL);
        break;
    }

    switch(ui->triplesTypeCombo->currentIndex()) {
    case 0:
        // BitmapTriples
        hdt.set("triples.type", hdt::HDTVocabulary::TRIPLES_TYPE_BITMAP);
        break;
    case 1:
        // TriplesList
        hdt.set("triples.type", hdt::HDTVocabulary::TRIPLES_TYPE_TRIPLESLIST);
        break;
    case 2:
        // PlainTriples
        hdt.set("triples.type", hdt::HDTVocabulary::TRIPLES_TYPE_PLAIN);
        break;
    }

    if(ui->streamXcombo->isEnabled()) {
        hdt.set("stream.x", getStreamType(ui->streamXcombo->currentIndex()));
    }

    if(ui->streamYcombo->isEnabled()) {
        hdt.set("stream.y", getStreamType(ui->streamYcombo->currentIndex()));
    }

    if(ui->streamZcombo->isEnabled()) {
        hdt.set("stream.z", getStreamType(ui->streamZcombo->currentIndex()));
    }
}

hdt::RDFNotation HDTSpecForm::getNotation()
{
    switch(ui->rdfInputCombo->currentIndex()) {
    case 0:
        return hdt::NTRIPLES;
    case 1:
        return hdt::N3;
    case 2:
        return hdt::TURTLE;
    case 3:
        return hdt::XML;
    }
    return hdt::NTRIPLES;
}

void HDTSpecForm::accept()
{
    if(getFileName()=="") {
        QMessageBox::warning(NULL, tr("Select a file name."), tr("Please insert a file name or URI."));
    } else {
        done(1);
    }
}
