/**
 * Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Traffic Controller
 * 
 * Revision History:
 *      30NOV2021  R-11-30: Document Created, initial coding
 *      01DEC2021  R-12-01: Convert class to abstract
 * 
 **/

#ifndef TRAFFICCONTROLLER_H
#define TRAFFICCONTROLLER_H

#include "intersection.h"
#include "pod.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <map>

#define UPDATE_GAP_MS 100

class TrafficController
{
public:
    // Constructors
    TrafficController(Intersection* theIntersection):thisIntersection(theIntersection)
    {
        controllerActive = true;
        for (int i=0; i<theIntersection->getNumNodes(); ++i)
        {
            for (int j=0; j<theIntersection->getNumNodes(); ++j)
            {
                if (i == j)
                {
                    continue;
                }
                std::string lane_id = std::to_string(i) + "-" + std::to_string(j);
                std::deque<Pod*> thisLaneQueue;
                laneQueues.insert({lane_id, thisLaneQueue});
            }
        }
    }

    // Destructors

    // Member Functions
    void startController()
    {
        std::thread (&TrafficController::entryCheck, this).detach();
        std::thread (&TrafficController::updatePods, this).detach();
    }

    void stopController()
    {
        controllerActive = false;
    }

    virtual void schedulePod(Vehicle* entryVehicle) = 0;
    virtual void doUpdate() = 0;

    // Thread Functions
    void entryCheck()
    {
        while (controllerActive)
        {
            if (entryQueue.size() > 0)
            {
                schedulePod(entryQueue.front());
                entryQueue.pop();
            }
        }
    }

    void updatePods()
    {
        while (controllerActive)
        {
            doUpdate();

            // Update rate at approx UPDATE_GAP_MS between updates
            std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_GAP_MS));
        }
    }

    // Getters

    // Setters

public:
    std::queue<Vehicle*> entryQueue;

protected:
    bool controllerActive;
    Intersection* thisIntersection;
    std::map<std::string, Pod*> controlledPods;
    std::map<std::string, std::deque<Pod*>> laneQueues;
};

#endif