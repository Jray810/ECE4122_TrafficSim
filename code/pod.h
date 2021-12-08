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
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#ifndef POD_H
#define POD_H

#include "vehicle.h"
#include <ctime>

/**
 * Pod Class
 * Description:
 *          Class that simulates a pod communicating between traffic controller and vehicle
 **/
class Pod
{
public:
    // Constructors
    Pod(Vehicle* obj, Lane* ln, unsigned long int timeAdded = -1);

    // Destructors
    ~Pod();

    // Member Functions
    unsigned long int predictedEntry(unsigned long int currentTime);
    void updatePosition(int speed, int cntdown = -1);
    void setTarget(unsigned long int desiredEntry, unsigned long int currentTime);
    
    // Setters
    void setPositionInQueue(int pos){positionInQueue = pos;}
    void setExitStamp(unsigned long int exit){exitstamp = exit;}

    // Getters
    std::string getPodID(){return podID;}
    unsigned long int getTimestamp(){return timestamp;}
    unsigned long int getExitstamp(){return exitstamp;}
    unsigned long int getWaitTime(){return waitTime;}
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
    std::string podID;              // Unique pod identifier
    unsigned long int timestamp;    // Entry timestamp
    unsigned long int exitstamp;    // Exit timestamp
    unsigned long int waitTime;     // Exit - Entry - Expected delay
    Vehicle* vehicle;               // Pointer to vehicle it controls
    Lane* lane;                     // Pointer to lane it is in
    double position;                // Linear position in lane

    // Instructors
    bool move;                      // Check if pod is moving
    int countdown;                  // Countdown for speed adjustment

    // Targets
    bool targetSet;                             // Check if a target is set
    unsigned long int targetIntersectionEntry;  // Intersection entry target
    unsigned long int targetIntersectionExit;   // Intersection exit target
    unsigned long int timeInIntersection;       // Expected time in intersection

    // Status
    bool inIntersectionSquare;      // Check if pod is in intersection square
    int positionInQueue;            // Position of pod in its lane queue
};

#endif