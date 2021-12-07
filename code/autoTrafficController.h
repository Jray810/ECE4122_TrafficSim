/**
 * Autonomous Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Autonomous Traffic Controller class performs automated scheduling of entering
 *      vehicles so as to avoid collisions when they reach the intersection. It will force
 *      cars to hold a slowed speed approaching the intersection if the normal timeslot is
 *      occupied. OpenMP is used within the update function to speed up updates.
 * 
 * Revision History:
 *      01DEC2021  R-12-01: Document Created, initial coding
 *      04DEC2021  R-12-04: Re-write following TrafficController
 *                          class update.
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#ifndef AUTOTRAFFICCONTROLLER_H
#define AUTOTRAFFICCONTROLLER_H

#include "trafficController.h"

class AutoTrafficController: public TrafficController
{
public:
    // Constructors
    AutoTrafficController(Intersection* theIntersection, unsigned int tickSpeed)
        :TrafficController(theIntersection, tickSpeed){}

    // Member Functions
    void setPodEntry(Pod* thePod, unsigned long int desiredEntry);
    void schedulePod(Vehicle* entryVehicle);
    void doUpdate();
};

#endif