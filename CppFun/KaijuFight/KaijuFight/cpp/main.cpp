#include "headers/mainwindow.h"

#include <QApplication>

#include <cstdio>
#include "headers/GameTime.h"


/////////////////////////////////////////////
/// \brief processInput
/// for simplicity, processess at every opportunity
/// \return
///
static int processInput()
{
    // adds inputs onto a stack.
    return 0;
}

static int fixedUpdate(const timeData &tData)
{
    // update simulation at fixed time step.
    //printf("Last Simulation Update: %u\n", tData.lastSimulationUpdate);
    return 0;
}

static int renderUpdate(MainWindow &window, const timeData &tData)
{
    window.setCommandBoxText(std::to_string(tData.startOfGameLoop));
    return 0;
}

///
/// \brief gameLoop
/// \param window
/// \param application
/// \return
/// It looks like Qt has it's own loop, which makes sense. To make the gameLoop work, I would need it to be running at the same time
///     as Qt's loop. Otherwise, one of these loops is going to be stuck. Or I have one loop go after the other.
///     See QApplication.exec(): https://doc.qt.io/qt-6/qapplication.html#exec
/// Dunno if I want to do that. I want everything to be happening in the gameLoop. Having another concurrent mainLoop
///     is disgusting, harder to debug, etc. Might be able to have Qt's main loop call into a gameUpdate call that is basically everything
///     the game loop would do that frame. Feels bad.
/// I don't like this. Maybe it's easier/not as bad as I think. I'm considering just making this project in C. Before I commit to that,
///     I want to learn more about linux. I'm going to focus on that.
///
///

class QApplication;
static int gameLoop(MainWindow &window, QApplication &application)
{
    //////////////////////
    /// Setup time data.
    static timeData tData;
    tData.uSecPerSimStep = 16666;
    uint64_t lag = 0;
    //////////////////////

    bool done = false;
    while (!done)
    {
        {
            //////////////////
            /// Update Time Data
            static uint64_t now;
            getTime(timeResolution::MICRO, now);
            tData.elapsedSinceLastGameLoop = now - tData.startOfGameLoop;
            tData.startOfGameLoop = now;
            lag += tData.elapsedSinceLastGameLoop;
            //////////////////
        }

        int code = processInput();
        assert(code == 0 && "Process input issue.");

        // update simulation based on how much time has passed.
            // adds sounds that need to be called to a stack.
        while (lag >= tData.uSecPerSimStep)
        {
            code = fixedUpdate(tData);
            assert(code == 0 && "Fixed Update issue.");

            // Update Timing Test Part A
            /*
            uint64_t testVal;
            getTime(timeResolution::MICRO, testVal);
            uint64_t elapsedSinceLastSim = tData.lastSimUpdate - testVal;
            printf("Time since last sim update: %u\n", elapsedSinceLastSim);
            */

            getTime(timeResolution::MICRO, tData.lastSimUpdate);
            lag -= tData.uSecPerSimStep;

            // Update Timing Test Part B
            /*
            static int test = 0;
            test += 1;
            if (test >= 10)
            {
                fflush(stdout);
                done = true;
            }
            */
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
    w.show();
    a.exec();

    gameLoop(w, a);

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

    /*
    int (*ptr) (int);
    ptr = funcTest;
    printf("%d", ptr(0));
    */


    return a.exec();
}

