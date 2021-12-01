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
 * 
 **/

#include <iostream>
#include "intersect4wsl.h"

#define DEFAULT_SPEED_LIMIT 40

using namespace std;

int main(int argc, char *argv[])
{
    // Environment Setup Section


    // World Setup Section
    Intersect4WSL* theIntersection = new Intersect4WSL(DEFAULT_SPEED_LIMIT);


    // Start Operations


    // End Operations

    // Cleanup
    delete theIntersection;
    return 0;
}
