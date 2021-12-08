/**
 * Light Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Light Traffic Controller class simulates an intersection with traffic lights.
 *      This class is currently a work in progress.
 * 
 * Revision History:
 *      01DEC2021  R-12-01: Document Created, initial coding
 * 
 **/

#ifndef LIGHTTRAFFICCONTROLLER_H
#define LIGHTTRAFFICCONTROLLER_H

#include "trafficController.h"

/**
 * LightTrafficController Class
 * Description:
 *          Class that simulates a traffic light controlled traffic controller
 **/
class LightTrafficController: public TrafficController
{
public:
    // Constructors
    LightTrafficController(Intersection* theIntersection, unsigned int tickSpeed):TrafficController(theIntersection, tickSpeed), lightPhase(0){}

    // Member Functions
    void schedulePod(Vehicle* entryVehicle);
    void doUpdate();
    void startLightCycle();

    // Thread Functions
    void changingLights();

    // Getters
    int getLightPhase(){return lightPhase;}

public:
    int lightPhase;                             // Identify which Traffic Light Phase it currently is
    std::map<std::string, int> trafficLights;   // All the traffic lights and their states
};

#endif