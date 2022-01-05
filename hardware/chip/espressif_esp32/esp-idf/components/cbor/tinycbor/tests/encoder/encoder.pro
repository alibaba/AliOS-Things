SOURCES += tst_encoder.cpp

CONFIG += testcase parallel_test c++11
QT = core testlib

INCLUDEPATH += ../../src
msvc: POST_TARGETDEPS = ../../lib/tinycbor.lib
else: POST_TARGETDEPS += ../../lib/libtinycbor.a
LIBS += $$POST_TARGETDEPS
