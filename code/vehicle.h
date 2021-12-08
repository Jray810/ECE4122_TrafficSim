/**
 * Vehicle
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Defines base class vehicle object that will carry passengers.
 *      Vehicles will be carried by pods upon entering the jurisdiction
 *      of a traffic controller.
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

#ifndef VEHICLE_H
#define VEHICLE_H

#include "lane.h"

#define SLOW_DOWN 0
#define MAINTAIN 1
#define SPEED_UP 2

/**
 * Vehicle Class
 * Description:
 *          Class that simulates a vehicle. Vehicle is not self aware of position
 **/
class Vehicle
{
public:
    // Constructors
    Vehicle(std::string id, double mS, double mTS, double a, Node* src, Node* dest);

    // Member Functions
    double update(double speed);
    void exit(unsigned long int wait);

    // Setters
    void setTrafficControl(bool control){underTrafficControl = control;}
    void setPod(void* ptr){pod = ptr;}

    // Getters
    std::string getVehicleID(){return vehicleID;}
    void* getPod(){return pod;}
    double getWaitTime(){return waitTime;}
    double getMaxSpeed(){return maxSpeed;}
    double getMaxTurnSpeed(){return maxTurnSpeed;}
    double getAcceleration(){return acceleration;}
    double getInitialSpeed(){return initialSpeed;}
    double getInitialAcceleration(){return initialAcceleration;}
    Node* getSource(){return source;}
    Node* getDestination(){return destination;}
    double getCurrentSpeed(){return currentSpeed;}
    double getCurrentAcceleration(){return currentAcceleration;}
    bool isTurning(){return currentTurning;}
    bool isCrashed(){return crashed;}
    bool isUnderTrafficControl(){return underTrafficControl;}
    bool isExited(){return exited;}

protected:
    std::string vehicleID;          // Unique vehicle identifier
    void* pod;                      // Pointer to pod
    double waitTime;                // Time delayed in intersection

    // Vehicle Properties
    double maxSpeed;                // Maximum speed capability
    double maxTurnSpeed;            // Maximum turning speed capability
    double acceleration;            // Acceleration constant

    // Vehicle Initial State
    double initialSpeed;            // Initial speed upon existence
    double initialAcceleration;     // Initial acceleration upon existence
    Node* source;                   // Source node
    Node* destination;              // Destination node

    // Vehicle Current State
    double currentSpeed;            // Current speed
    double currentAcceleration;     // Current acceleration
    bool currentTurning;            // Whether or not vehicle is turning
    bool crashed;                   // Whether or not vehicle has been involved in a collision

    // Vehicle Control
    bool underTrafficControl;       // Whether or not vehicle is under traffic controller control
    bool exited;                    // Whether or not vehicle has exited traffic controller control
};

#endif