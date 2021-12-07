/**
 * Stop Sign Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Stop Sign Traffic Controller class simulates an intersection where cars enter
 *      and queue up at a stop sign. Cars proceed through the intersection in order of
 *      when they join the traffic control. OpenMP is used within the update function
 *      to speed up updates.
 * 
 * Revision History:
 *      01DEC2021  R-12-01: Document Created, initial coding
 *      04DEC2021  R-12-04: Re-write following TrafficController
 *                          class update.
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#ifndef STOPTRAFFICCONTROLLER_H
#define STOPTRAFFICCONTROLLER_H

#include "trafficController.h"

class StopTrafficController: public TrafficController
{
public:
    // Constructors
    StopTrafficController(Intersection* theIntersection, unsigned int tickSpeed):TrafficController(theIntersection, tickSpeed){}

    // Member Functions
    void schedulePod(Vehicle* entryVehicle);
    void doUpdate();
};

#endif