TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += core
#QT += network

SOURCES += \
    serveur.cpp

win32: LIBS += -lws2_32    # compiler avec bibliotheque winsock


# on impose que l'executable soit dans le repertoire du projet
DESTDIR= $$_PRO_FILE_PWD_

HEADERS += \
    compatible_socket.h \
    cdata.h \
    cdata2.h
