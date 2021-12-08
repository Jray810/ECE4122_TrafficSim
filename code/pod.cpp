/**
 * Pod
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Function implementation for pod class
 * 
 * Revision History:
 *      14NOV2021  R-11-14: Document Created, initial coding
 *      30NOV2021  R-11-30: Added intersection square presence detection
 *                          Adjusted update position methodology
 *      04DEC2021  R-12-04: Added target entry handling
 *                          to support autonomous control
 *      06DEC2021  R-12-06: Added destructor
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#include "pod.h"

// Constructor
Pod::Pod(Vehicle* obj, Lane* ln, unsigned long int timeAdded)
    :vehicle(obj), lane(ln)
    {
        timestamp = timeAdded;
        podID = obj->getVehicleID();
        position = 0;
        move = false;
        countdown = -1;
        targetSet = false;
        targetIntersectionEntry = -1;
        targetIntersectionExit = -1;
        timeInIntersection = (ln->getEndIntersection() - ln->getBeginIntersection()) / ln->getDestination()->speedLimit;
        inIntersectionSquare = false;
        positionInQueue = -1;
        obj->setPod(this);
    }

// Destructor
Pod::~Pod()
{
    vehicle->setTrafficControl(false);
    vehicle->setPod(NULL);
    waitTime = exitstamp - timestamp - 12;
    vehicle->exit(waitTime);
}

/**
 * predictedEntry
 * Inputs:
 *      unsigned long int - Time at time of calculation
 * Outputs:
 *      unsigned long int - Predicted time of reaching intersection
 * Description:
 *          Calculates the predicted time the vehicle will reach
 *          the intersection given its current state
 **/
unsigned long int Pod::predictedEntry(unsigned long int currentTime)
{
    double distance = lane->getBeginIntersection();
    double speedLimit = lane->getSource()->speedLimit;
    unsigned long int entryTime = currentTime + (distance / speedLimit);
    return entryTime;
}

/**
 * updatePosition
 * Inputs:
 *      int - Desired speed
 *      int - Value for setting the countdown
 * Outputs: None
 * Description:
 *          Updates pod's current position given speed
 **/
void Pod::updatePosition(int speed, int cntdown)
{
    // Check if a countdown value was given
    if (cntdown != -1)
    {
        countdown = cntdown;
    }
    else if (countdown > 0)
    {
        countdown--;
    }

    // Move only if speed is positive
    move = speed > 0;

    // Move!
    if (move)
    {
        position += vehicle->update(speed);
    }

    // Check if in intersection square
    inIntersectionSquare = (position > lane->getBeginIntersection() && position < lane->getEndIntersection()) ? true : false;
}

/**
 * setTarget
 * Inputs:
 *      unsigned long int - Desired time at which pod will reach intersection
 *      unsigned long int - Current time
 * Outputs: None
 * Description:
 *          Set the countdown value for adjusting speed to meet demand
 **/
void Pod::setTarget(unsigned long int desiredEntry, unsigned long int currentTime)
{
    unsigned long int timeDiff = desiredEntry - currentTime;
    countdown = 4*timeDiff - 4*(lane->getBeginIntersection()) / (lane->getSource()->speedLimit);
    targetIntersectionEntry = desiredEntry;
    targetIntersectionExit = desiredEntry + timeInIntersection;
    targetSet = true;
}