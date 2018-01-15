#-------------------------------------------------
#
# Project created by QtCreator 2017-12-15T10:13:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PdfViewer_
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    pdfviewer_mainwindow.cpp



HEADERS += \
    mupdf/fitz/annotation.h \
    mupdf/fitz/archive.h \
    mupdf/fitz/band-writer.h \
    mupdf/fitz/bidi.h \
    mupdf/fitz/bitmap.h \
    mupdf/fitz/buffer.h \
    mupdf/fitz/color-management.h \
    mupdf/fitz/colorspace.h \
    mupdf/fitz/compress.h \
    mupdf/fitz/compressed-buffer.h \
    mupdf/fitz/config.h \
    mupdf/fitz/context.h \
    mupdf/fitz/crypt.h \
    mupdf/fitz/device.h \
    mupdf/fitz/display-list.h \
    mupdf/fitz/document.h \
    mupdf/fitz/filter.h \
    mupdf/fitz/font.h \
    mupdf/fitz/geometry.h \
    mupdf/fitz/getopt.h \
    mupdf/fitz/glyph-cache.h \
    mupdf/fitz/glyph.h \
    mupdf/fitz/hash.h \
    mupdf/fitz/image.h \
    mupdf/fitz/link.h \
    mupdf/fitz/outline.h \
    mupdf/fitz/output-pcl.h \
    mupdf/fitz/output-pclm.h \
    mupdf/fitz/output-png.h \
    mupdf/fitz/output-pnm.h \
    mupdf/fitz/output-ps.h \
    mupdf/fitz/output-psd.h \
    mupdf/fitz/output-pwg.h \
    mupdf/fitz/output-svg.h \
    mupdf/fitz/output-tga.h \
    mupdf/fitz/output.h \
    mupdf/fitz/path.h \
    mupdf/fitz/pixmap.h \
    mupdf/fitz/pool.h \
    mupdf/fitz/separation.h \
    mupdf/fitz/shade.h \
    mupdf/fitz/store.h \
    mupdf/fitz/stream.h \
    mupdf/fitz/string-util.h \
    mupdf/fitz/structured-text.h \
    mupdf/fitz/system.h \
    mupdf/fitz/text.h \
    mupdf/fitz/track-usage.h \
    mupdf/fitz/transition.h \
    mupdf/fitz/tree.h \
    mupdf/fitz/util.h \
    mupdf/fitz/version.h \
    mupdf/fitz/writer.h \
    mupdf/fitz/xml.h \
    mupdf/helpers/mu-office-lib.h \
    mupdf/helpers/mu-threads.h \
    mupdf/pdf/annot.h \
    mupdf/pdf/appearance.h \
    mupdf/pdf/clean.h \
    mupdf/pdf/cmap.h \
    mupdf/pdf/crypt.h \
    mupdf/pdf/document.h \
    mupdf/pdf/event.h \
    mupdf/pdf/field.h \
    mupdf/pdf/font.h \
    mupdf/pdf/interpret.h \
    mupdf/pdf/javascript.h \
    mupdf/pdf/name-table.h \
    mupdf/pdf/object.h \
    mupdf/pdf/output-pdf.h \
    mupdf/pdf/page.h \
    mupdf/pdf/parse.h \
    mupdf/pdf/resource.h \
    mupdf/pdf/widget.h \
    mupdf/pdf/xref.h \
    mupdf/fitz.h \
    mupdf/memento.h \
    mupdf/pdf-tools.h \
    mupdf/pdf.h \
    mupdf/ucdn.h \
    pdfviewer_mainwindow.h

FORMS +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/ -llibmupdf
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/ -llibmupdf
else:unix:!macx: LIBS += -L$$PWD/libs/ -llibmupdf

INCLUDEPATH += $$PWD/libs
DEPENDPATH += $$PWD/libs



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/libs/ -llibthirdparty
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/libs/ -llibthirdparty
else:unix:!macx: LIBS += -L$$PWD/libs/ -llibthirdparty

INCLUDEPATH += $$PWD/mupdf
DEPENDPATH += $$PWD/mupdf

unix:!macx|win32: LIBS += -L$$PWD/libs/ -llibresources

INCLUDEPATH += $$PWD/mupdf
DEPENDPATH += $$PWD/mupdf
