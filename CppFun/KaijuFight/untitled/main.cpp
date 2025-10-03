#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    //////////////////////////////////////////////
    /// APP FLOW
    /// //////////////////////////////////////////
    /// setup MVP
    ///     window MVP
    ///     Load data (kaiju data) MVP
    ///     Load
    ///     Intro sequence (You are logging into an old console) *P3*
    ///          Booting Kaiju Fight DOS style load screen. '*' filling across screen as it loads
    ///

    ///
    /// Main Menu loop
    ///     Fight MVP
    ///         "FIGHT" command
    ///     View Kaiju Data
    /// Fight
    ///     Fight UI
    ///     Pre-battle
    ///     Fight

    w.show();
    return a.exec();
}
