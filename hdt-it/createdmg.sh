#!/bin/bash

SRCAPP="hdt-it.app"
FOLDER="macdmg"
VOL="hdt-it"
NAME="HDT-it!"
VERS="0.9beta"
DMG="$VOL-$VERS.dmg"

FRAMEWORK_BASE="$SRCAPP/Contents/Frameworks/"
REMOVE="QtSql.framework QtXmlPatterns.framework QtDeclarative.framework QtNetwork.framework QtScript.framework QtSvg.framework"

DATASETBASE="$HOME/rdf/dataset/"
DATASETS="geospecies test"
DATASETHDTBASE="$HOME/rdf/hdt/"
DATASETSHDT="aifb.hdt apex.hdt 2blog.hdt 3kbo.hdt aaronland.hdt 3dnews.hdt aisricom.hdt"

# ADD LIBRARIES
echo macdeployqt $SRCAPP
macdeployqt $SRCAPP

# ADD Translation files
cp -Rf ../*.qm $STCAPP/Resources

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
