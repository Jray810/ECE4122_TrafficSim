/**
 * Main
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Main.
 * 
 * Revision History:
 *      30NOV2021  R-11-30: Document Created, initial coding
 *      01DEC2021  R-12-01: Added Command Line Argument Detection
 * 
 **/

#include "intersect4wsl.h"
#include "autoTrafficController.h"
#include "lightTrafficController.h"
#include <iostream>
#include <thread>

#define DEFAULT_SPEED_LIMIT 10

#define AUTO    0
#define LIGHT   1
#define STOP    2

unsigned int controllerType;

using namespace std;

int main(int argc, char *argv[])
{
    // Command Line Argument Detection
    if (argc == 1)
    {
        controllerType = 0;
    }
    if (argc == 2)
    {
        string typeArg = argv[1];
        switch (typeArg)
        {
            case "-A":
                controllerType = AUTO;
                break;
            case "-L":
                controllerType = LIGHT;
                break;
            case "-S":
                controllerType = STOP;
                break;
            default:
                cerr << "Something went wrong\n";
                return 1;
                break;
        }
    }

    // Environment Setup Section




    // World Setup Section
    Intersect4WSL* theIntersection = new Intersect4WSL(DEFAULT_SPEED_LIMIT);
    TrafficController* theTrafficController;
    switch (controllerType)
    {
        case AUTO:
            theTrafficController = new AutoTrafficController(theIntersection);
            break;
        case LIGHT:
            theTrafficController = new LightTrafficController(theIntersection);
            break;
        case STOP:
        default:
            theTrafficController = new AutoTrafficController(theIntersection);
            break;
    }

    // Start Operations
    theTrafficController->startController();
    if (controllerType == LIGHT)
    {
        LightTrafficController* control = dynamic_cast<LightTrafficController*>(theTrafficController);
        control->startLightCycle();
    }

    // End Operations

    // Cleanup
    theTrafficController->stopController();
    delete theTrafficController;
    delete theIntersection;
    return 0;
}
