/**
 * Vehicle
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Function implementation for vehicle class
 * 
 * Revision History:
 *      14NOV2021  R-11-14: Document Created, initial coding
 *      14NOV2021  R-11-14: Added entry constructor
 *      30NOV2021  R-11-30: Added traffic controller boolean
 *                          Adjusted position update methodology
 *      06DEC2021  R-12-06: Added flag to check if vehicle has exited
 *                          intersection
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#include "vehicle.h"

// Constructor
Vehicle::Vehicle(std::string id, double mS, double mTS, double a, Node* src, Node* dest)
    :vehicleID(id), maxSpeed(mS), maxTurnSpeed(mTS), acceleration(a), source(src), destination(dest)
    {
        initialSpeed = source->speedLimit;
        currentSpeed = initialSpeed;
        initialAcceleration = 0;
        currentAcceleration = 0;
        currentTurning = false;
        crashed = false;
        underTrafficControl = false;
        pod = NULL;
        exited = false;
    }

/**
 * update
 * Inputs:
 *      double - Speed of the vehicle
 * Outputs:
 *      double - Distance traveled in one tick
 * Description:
 *          Calculates how far the vehicle traveled in one tick
 **/
double Vehicle::update(double speed)
{
    currentSpeed = speed;
    double distTraveled = currentSpeed;
    return distTraveled;
}

/**
 * exit
 * Inputs:
 *      unsigned long int - How long the vehicle was delayed in intersection
 * Outputs: None
 * Description:
 *          Signals exit. Stores the delay time in waitTime
 **/
void Vehicle::exit(unsigned long int wait)
{
    exited = true;
    waitTime = wait;
}