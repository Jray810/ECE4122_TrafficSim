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

#define UPDATE_GAP_MS 500

class TrafficController
{
public:
    // Constructors
    TrafficController(Intersection* theIntersection):thisIntersection(theIntersection)
    {
        controllerActive = true;
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

    void schedulePod(Vehicle* entryVehicle)
    {
        entryVehicle->setTrafficControl(true);
        std::string lane_id = entryVehicle->getSource()->nodeID + "-" + entryVehicle->getDestination()->nodeID;
        Lane* desiredLane = thisIntersection->getLane(lane_id);
        Pod* entryPod = new Pod(entryVehicle, desiredLane);
        controlledPods.insert({entryPod->getPodID(), entryPod});

        // Give instructions to pod if necessary, to meet specified interesction entry time
    }

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
            for (auto it = controlledPods.begin(); it != controlledPods.end(); ++it)
            {
                std::string podID = it->first;
                Pod* thisPod = it->second;
                thisPod->updatePosition();

                // For debugging
                std::cout << podID << " : " << thisPod->getPosition() << std::endl;

                // Check if pod has left intersection
                if (thisPod->getPosition() > thisPod->getLane()->getLaneLength())
                {
                    controlledPods.erase(it);
                    thisPod->getVehicle()->setTrafficControl(false);
                    delete thisPod;
                }
            }

            // Update rate at approx UPDATE_GAP_MS between updates
            std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_GAP_MS));
        }
    }

    // Getters

    // Setters

public:
    std::queue<Vehicle*> entryQueue;

private:
    bool controllerActive;
    Intersection* thisIntersection;
    std::map<std::string, Pod*> controlledPods;
};

#endif