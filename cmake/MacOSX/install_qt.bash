#!/bin/bash
## 
## 04/16/2009 - Stephan Aiche
##  Copies the Qt libraries into the installation bundle and fixes the 
##  internal associations between the different Qt libraries
##
## Commandline arguments are: 
##  $1 -> QT_LIB_PATH
##  $2 -> target path
##  $3 -> install_name_tool

for lib in QtOpenGL QtCore QtGui QtXml QtSql QtNetwork QtTest
do
    # copy Qt library
    cp -Rf $1/$lib.framework $2

    # fix the id's
    $3 -id ./$lib.framework/Versions/4/$lib \
        $2/$lib.framework/Versions/4/$lib

    # fix reference to core
    $3 -change $1/QtCore.framework/Versions/4/QtCore \
        QtCore.framework/Versions/4/QtCore \
        $2/$lib.framework/Versions/4/$lib
done

#fix reference of QtOpenGL to QtGui
$3 -change $1/QtGui.framework/Versions/4/QtGui \
    QtGui.framework/Versions/4/QtGui \
    $2/QtOpenGL.framework/Versions/4/QtOpenGL

#delete debug stuff
find $2 -name "*debug*" -not -name "*.h" -exec rm -f {} \;
