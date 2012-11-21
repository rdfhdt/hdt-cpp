

LIBCDS = ../../../../libcds-v1.0.12
HDTLIB = ../../../

INCLUDEPATH += $${HDTLIB}/include/ .

# Using Traditional Makefile
#LIBS += $${LIBCDS}/lib/libcds.a ../hdt-lib/libhdt.a

# Using Qt Projects
win32:LIBS += ../hdt-lib/qmake/win32/libhdt.a $${LIBCDS}/qmake/win32/libcds.a
win32:LIBS += c:/mingw/lib/libraptor2.a c:/mingw/lib/libexpat.a

unix:!macx:LIBS += $${HDTLIB}/qmake/unix/libhdt.a $${LIBCDS}/qmake/unix/libcds.a
macx:LIBS += $${LIBCDS}/qmake/macx/libcds.a $${HDTLIB}/qmake/macx/libhdt.a

PRE_TARGETDEPS += $$LIBS

unix:LIBS += -lraptor2 -lkyotocabinet
