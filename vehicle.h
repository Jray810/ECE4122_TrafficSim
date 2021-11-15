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
    Vehicle(std::string id):vehicleID(id){}

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

    double update()
    {
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

        return distTraveled;        
    }

    // Setters
    void setTurning(bool turning)
    {
        currentTurning = turning;
    }

    // Getters
    std::string getVehicleID(){return vehicleID;}
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

protected:
    std::string vehicleID;

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
};

#endif