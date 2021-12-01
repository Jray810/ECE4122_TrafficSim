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
    Pod(Vehicle* obj, Lane* ln):vehicle(obj), lane(ln)
    {
        timestamp = time(NULL);
        podID = obj->getVehicleID();
        position = 0;
        updatePrediction();
        targetSet = false;
        targetIntersectionEntry = -1;
        targetIntersectionExit = -1;
        inIntersectionSquare = false;
    }

    // Member Functions
    void updatePosition(int speed, int cntdown = 0)
    {
        move = speed > 0 ? true : false;
        countdown = cntdown != 0 ? cntdown : ((countdown - 1) > 0 ? countdown - 1 : 0);
        if (move)
        {
            position += vehicle->update(speed);
        }
        updatePrediction();
        if (targetSet)
        {
            targetIntersectionEntry -= 1;
            targetIntersectionExit -= 1;
        }
        inIntersectionSquare = (position > lane->getBeginIntersection() && position <= lane->getEndIntersection()) ? true : false;
    }

    void updatePrediction()
    {
        timeToIntersectionEntry = findTimeToPos(0, vehicle->getCurrentSpeed(), position, lane->getBeginIntersection());
        timeInIntersection = findTimeToPos(0, vehicle->getCurrentSpeed(), lane->getBeginIntersection(), lane->getEndIntersection());
        timeToIntersectionExit = timeToIntersectionEntry + timeInIntersection;
    }

    bool setTarget(double desiredEntry, double desiredExit)
    {
        // Code for determining whether desired targets are feasible
        return false;
    }

    double predictedEntry()
    {
        if (targetSet)
        {
            return targetIntersectionEntry;
        }
        else
        {
            return timeToIntersectionEntry;
        }
    }
    
    void setPositionInQueue(int pos){
        positionInQueue = pos;
    }

    // Getters
    std::string getPodID(){return podID;}
    time_t getTimestamp(){return timestamp;}
    Vehicle* getVehicle(){return vehicle;}
    Lane* getLane(){return lane;}
    double getPosition(){return position;}
    double getTimeToIntersectionEntry(){return timeToIntersectionEntry;}
    double getTimeInIntersection(){return timeInIntersection;}
    double getTimeToIntersectionExit(){return timeToIntersectionExit;}
    bool isTargetSet(){return targetSet;}
    double getTargetIntersectionEntry(){return targetIntersectionEntry;}
    double getTargetIntersectionExit(){return targetIntersectionExit;}
    int getPositionInQueue(){return positionInQueue;}

private:
    std::string podID;
    time_t timestamp;
    Vehicle* vehicle;
    Lane* lane;
    double position;

    // Predictors
    double timeToIntersectionEntry;
    double timeInIntersection;
    double timeToIntersectionExit;

    // Instructors
    bool move;
    int countdown;

    // Targets
    bool targetSet;
    double targetIntersectionEntry;
    double targetIntersectionExit;

    // Status
    bool inIntersectionSquare;
    int positionInQueue;
};

#endif