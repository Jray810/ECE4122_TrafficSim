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
 * 
 **/

#ifndef VEHICLE_H
#define VEHICLE_H

#include "lane.h"

#define SLOW_DOWN 0
#define MAINTAIN 1
#define SPEED_UP 2

class Vehicle
{
public:
    // Constructors
    Vehicle(std::string id, double mS, double mTS, double a, Node* src, Node* dest):
        vehicleID(id), maxSpeed(mS), maxTurnSpeed(mTS), acceleration(a), source(src), destination(dest)
        {
            initialSpeed = source->speedLimit;
            currentSpeed = initialSpeed;
            initialAcceleration = 0;
            currentAcceleration = 0;
            currentTurning = false;
            crashed = false;
            underTrafficControl = false;
            pod = NULL;
        }

    // Member Functions
    bool instruct(unsigned int instruction)
    {
        switch (instruction)
        {
            case SLOW_DOWN:
                if (acceleration != -1)
                {
                    acceleration = -1;
                    return true;
                }
                break;
            case MAINTAIN:
                if (acceleration != 0)
                {
                    acceleration = 0;
                    return true;
                }
                break;
            case SPEED_UP:
                if (acceleration != 1)
                {
                    acceleration = 1;
                    return true;
                }
                break;
            default:
                return false;
                break;
        }
    }

    double update(int speed)
    {
        /*
        double distTraveled = 0.5 * currentAcceleration * acceleration + currentSpeed;

        currentSpeed = currentAcceleration * acceleration + currentSpeed;
        if (currentTurning)
        {
            if (currentSpeed > maxTurnSpeed)
            {
                currentSpeed = maxTurnSpeed;
            }
            else if (currentSpeed < maxTurnSpeed * -1)
            {
                currentSpeed = maxTurnSpeed * -1;
            }

        }
        else
        {
            if (currentSpeed > maxSpeed)
            {
                currentSpeed = maxSpeed;
            }
            else if (currentSpeed < maxSpeed * -1)
            {
                currentSpeed = maxSpeed * -1;
            }
        }
        */
        currentSpeed = speed;
        double distTraveled = currentSpeed;

        return distTraveled;
    }

    // Setters
    void setTurning(bool turning)
    {
        currentTurning = turning;
    }

    void setTrafficControl(bool control)
    {
        underTrafficControl = control;
    }

    void setPod(void* ptr)
    {
        pod = ptr;
    }

    // Getters
    std::string getVehicleID(){return vehicleID;}
    void* getPod(){return pod;}
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

protected:
    std::string vehicleID;
    void* pod;

    // Vehicle Properties
    double maxSpeed;
    double maxTurnSpeed;
    double acceleration;

    // Vehicle Initial State
    double initialSpeed;
    double initialAcceleration;
    Node* source;
    Node* destination;

    // Vehicle Current State
    double currentSpeed;
    double currentAcceleration;
    bool currentTurning;
    bool crashed;

    // Vehicle Control
    bool underTrafficControl;
};

#endif