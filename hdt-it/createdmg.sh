#!/bin/bash

SRCAPP="macx/HDT-it.app"
FOLDER="macdmg"
VOL="HDT-it"
NAME="HDT-it!"
VERS="1.1"
DMG="$VOL-$VERS.dmg"

FRAMEWORK_BASE="$SRCAPP/Contents/Frameworks/"
REMOVE="QtSql.framework QtXmlPatterns.framework QtDeclarative.framework QtNetwork.framework QtScript.framework QtSvg.framework QtGui.framework/Versions/4/QtGui.strip"

DATASETBASE="$HOME/rdf/dataset/"
DATASETS="test.nt"
DATASETHDTBASE="$HOME/hdt/"
DATASETSHDT="3kbo.hdt aaronland.hdt 3dnews.hdt aisricom.hdt"
#DATASETSHDT="aifb.hdt apex.hdt 2blog.hdt 3kbo.hdt aaronland.hdt 3dnews.hdt aisricom.hdt"

mkdir -p $FOLDER
rm -Rf $FOLDER/*

# ADD LIBRARIES
echo macdeployqt $SRCAPP
macdeployqt $SRCAPP

# ADD Translation files
cp -Rf *.qm $SRCAPP/Contents/Resources

# ADD License and documentation
cp -Rf LICENSE $FOLDER

mkdir -p $FOLDER/doc
cp -Rf doc/* $FOLDER/doc

# REMOVE UNNEDED FILES
for i in $REMOVE
do
	echo rm -Rf $FRAMEWORK_BASE$i;
	rm -Rf $FRAMEWORK_BASE$i;
done

echo rm -Rf "$SRCAPP/Contents/PlugIns/*"
rm -Rf "$SRCAPP/Contents/PlugIns/*"

# CREATE TMP FOLDER
echo mkdir -p $FOLDER/datasets
mkdir -p $FOLDER/datasets

echo "Fill moredatasets.txt"
echo "You can download more datasets from http://www.rdfhdt.org/datasets or import your own ones using the 'Import RDF' option!" > $FOLDER/datasets/moredatasets.txt

# COPY APP to folder
echo cp -R $SRCAPP $FOLDER
cp -R $SRCAPP $FOLDER

# COPY DATASETS
for i in $DATASETS
do
	echo cp $DATASETBASE$i $FOLDER/datasets
	cp $DATASETBASE$i $FOLDER/datasets
done

for i in $DATASETSHDT
do
	echo cp $DATASETHDTBASE$i $FOLDER/datasets
	cp $DATASETHDTBASE$i $FOLDER/datasets
done

# CREATE DMG
rm -Rf "$DMG"
echo hdiutil create -srcfolder "$FOLDER" -volname "$VOL" -format UDZO -imagekey zlib-level=9 "$DMG"
hdiutil create -srcfolder "$FOLDER" -volname "$NAME" -format UDZO -imagekey zlib-level=9 "$DMG"
