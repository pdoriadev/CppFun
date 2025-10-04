QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

#* Default rules for deployment.
    # Stack Overflow explanation - https://stackoverflow.com/questions/62044180/rules-for-deployment
    # Default settings QtCreator provides for deploying an app on different platforms.
    # Line 1 - sets the installation path (= target.path) for QNX systems to /tmp/$${TARGET}/bin where $${TARGET} will
        # get replaced by your application's name.
    # Line 2 - sets the installation path to /opt/$${TARGET}/bin but only for UNIX systems excluding Android
        # (that is the unix:!android: expression).
    # Line 3 - Checks if the target.path is not empty.
        # If empty, adds your target (i.e. your application or library) to the install list so that it will be
            # installed when make install is called."
    # Follow-up Questions
        # L3 - What is "make install"?
        # L2 - Android is excluded because it is mobile
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    DesignRef.md \
    MarkdownRef.md \
    TechnicalRef.md
