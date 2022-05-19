TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
DESTDIR = ../../bin

CBORDIR = $$PWD/../../src
INCLUDEPATH += $$CBORDIR
SOURCES += json2cbor.c
LIBS += ../../lib/libtinycbor.a

CJSONDIR = .
!exists($$CJSONDIR/cJSON.h): CJSONDIR = $$CBORDIR/cjson
exists($$CJSONDIR/cJSON.h) {
    INCLUDEPATH += $$CJSONDIR
    SOURCES += $$CJSONDIR/cJSON.c
} else {
    message("cJSON not found, not building json2cbor.")
    TEMPLATE = aux
}
