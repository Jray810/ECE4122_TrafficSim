/**
 * Pod
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Defines pod object that will control vehicles.
 *      Used by traffic controllers to interface with vehicles.
 * 
 * Revision History:
 *      14NOV2021  R-11-14: Document Created, initial coding
 *      30NOV2021  R-11-30: Added intersection square presence detection
 *                          Adjusted update position methodology
 *      04DEC2021  R-12-04: Added target entry handling
 *                          to support autonomous control
 *      06DEC2021  R-12-06: Added destructor
 * 
 **/

#ifndef POD_H
#define POD_H

#include "calc.h"
#include "vehicle.h"
#include <ctime>

class Pod
{
public:
    // Constructors
    Pod(Vehicle* obj, Lane* ln, unsigned long int timeAdded = -1):vehicle(obj), lane(ln)
    {
        timestamp = timeAdded;
        podID = obj->getVehicleID();
        position = 0;
        move = false;
        countdown = -1;
        targetSet = false;
        targetIntersectionEntry = -1;
        targetIntersectionExit = -1;
        timeInIntersection = -1;
        inIntersectionSquare = false;
        positionInQueue = -1;
        obj->setPod(this);
    }

    // Destructors
    ~Pod()
    {
        vehicle->setPod(NULL);
    }

    // Member Functions
    unsigned long int predictedEntry(unsigned long int currentTime)
    {
        double distance = lane->getBeginIntersection();
        double speedLimit = lane->getSource()->speedLimit;
        unsigned long int entryTime = currentTime + (distance / speedLimit);
        return entryTime;
    }

    void updatePosition(int speed, int cntdown = -1)
    {
        if (cntdown != -1)
        {
            countdown = cntdown;
        }
        else if (countdown > 0)
        {
            countdown--;
        }

        move = speed > 0;

        if (move)
        {
            position += vehicle->update(speed);
        }

        inIntersectionSquare = (position > lane->getBeginIntersection() && position <= lane->getEndIntersection()) ? true : false;
    }

    void setTarget(unsigned long int desiredEntry, unsigned long int currentTime)
    {
        unsigned long int timeDiff = desiredEntry - currentTime;
        countdown = 4*timeDiff - 4*(lane->getBeginIntersection()) / (lane->getSource()->speedLimit);
        timeInIntersection = (lane->getEndIntersection() - lane->getBeginIntersection()) / lane->getDestination()->speedLimit;
        targetIntersectionEntry = desiredEntry;
        targetIntersectionExit = desiredEntry + timeInIntersection;
        targetSet = true;
    }
    
    void setPositionInQueue(int pos){
        positionInQueue = pos;
    }

    // Getters
    std::string getPodID(){return podID;}
    unsigned long int getTimestamp(){return timestamp;}
    Vehicle* getVehicle(){return vehicle;}
    Lane* getLane(){return lane;}
    double getPosition(){return position;}
    int getCountdown(){return countdown;}
    bool isTargetSet(){return targetSet;}
    unsigned long int getEntry(){return targetIntersectionEntry;}
    unsigned long int getExit(){return targetIntersectionExit;}
    unsigned long int getTimeInIntersection(){return timeInIntersection;}
    bool getInIntersectionSquare(){return inIntersectionSquare;}
    int getPositionInQueue(){return positionInQueue;}

private:
    std::string podID;
    unsigned long int timestamp;
    Vehicle* vehicle;
    Lane* lane;
    double position;

    // Instructors
    bool move;
    int countdown;

    // Targets
    bool targetSet;
    unsigned long int targetIntersectionEntry;
    unsigned long int targetIntersectionExit;
    unsigned long int timeInIntersection;

    // Status
    bool inIntersectionSquare;
    int positionInQueue;
};

#endif