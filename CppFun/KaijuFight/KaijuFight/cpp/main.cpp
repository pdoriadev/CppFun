#include "headers/mainwindow.h"

#include <QApplication>

#include <cstdio>
#include "headers/GameTime.h"


static int processInput()
{
    // adds inputs onto a stack.
    return 0;
}


static int fixedUpdate(const timeData &tData)
{
    // update simulation at fixed time step.
    printf("Current Loop Time: %d", tData.currentGameLoop);
    return 0;
}

static int renderUpdate(MainWindow &window, const timeData &tData)
{
    window.setCommandBoxText(std::to_string(tData.currentGameLoop));
    return 0;
}


static int gameLoop(MainWindow &window)
{
    //////////////////////
    /// \brief
    /// Setup time data.
    ///


    static timeData tData;
    getTime(timeResolution::MICRO, tData.prevGameLoop);
    tData.uSecPerStep = 16666;
    uint64_t lag = 0;

    bool done = false;
    while (!done)
    {
        getTime(timeResolution::MICRO, tData.currentGameLoop);
        tData.elapsed = tData.currentGameLoop - tData.prevGameLoop;
        tData.prevGameLoop = tData.currentGameLoop;
        lag += tData.elapsed;
        // edge case for first frame?? --> elapses = 0.

        // process input
            // for simplicity, processess at every opportunity
        int code = processInput();
        assert(code == 0 && "Process input issue.");

        // update simulation based on how much time has passed.
            // adds sounds that need to be called to a stack.
        while (lag >= tData.uSecPerStep)
        {
            code = fixedUpdate(tData);
            assert(code == 0 && "Process input issue.");

            getTime(timeResolution::MICRO, tData.lastSimulationUpdate);
            lag -= tData.uSecPerStep;
        }

        // always update graphics
            // do i even need to worry about this if QT is handling all the text and images?
        // update audio
            // audio bank. Go through stack to
        //
        renderUpdate(window, tData);
    }

    return 0;
}

int funcTest(int test)
{
    return test;
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    gameLoop(w);

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
    int (*ptr) (int);
    ptr = funcTest;
    printf("%d", ptr(0));

    w.show();
    return a.exec();
}

