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
    case 3:
        // CompactTriples
        ui->streamXcombo->setEnabled(false);
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
    }
    ui->rdfInputFile->setText(file);
}

QString HDTSpecForm::getFileName()
{
    return ui->rdfInputFile->text();
}

std::string HDTSpecForm::getStreamType(int index) {
    switch(index) {
    case 0:
        // LogStream
        return hdt::HDTVocabulary::STREAM_TYPE_LOG;
    case 1:
        // IntegerStream
        return hdt::HDTVocabulary::STREAM_TYPE_INTEGER;
    case 2:
        // HuffmanStream
        return hdt::HDTVocabulary::STREAM_TYPE_HUFFMAN;
    case 3:
        // WaveletStream
        return hdt::HDTVocabulary::STREAM_TYPE_WAVELET;
    }
    return "";
}

void HDTSpecForm::fillHDTSpecification(hdt::HDTSpecification &hdt)
{
    hdt.set("header.type", hdt::HDTVocabulary::DICTIONARY_TYPE_PLAIN);

    switch(ui->dictionaryTypeCombo->currentIndex()) {
    case 0:
        // PFCDictionary
        hdt.set("dictionary.type", hdt::HDTVocabulary::DICTIONARY_TYPE_PFC);
        break;
    case 1:
        // PlainDictionary
        hdt.set("dictionary.type", hdt::HDTVocabulary::DICTIONARY_TYPE_PLAIN);
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
    case 3:
        // CompactTriples
        hdt.set("triples.type", hdt::HDTVocabulary::TRIPLES_TYPE_COMPACT);
        break;
    case 4:
        // FoQTriples
        hdt.set("triples.type", hdt::HDTVocabulary::TRIPLES_TYPE_FOQ);
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
