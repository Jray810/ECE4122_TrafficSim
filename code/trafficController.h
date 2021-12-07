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
 *      04DEC2021  R-12-04: Added lane queues, world queues, and mutexes
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
#include <vector>
#include <mutex>
#include <omp.h>

class TrafficController
{
public:
    // Constructors
    TrafficController(Intersection* theIntersection, unsigned int tickSpeed):thisIntersection(theIntersection), tickSpeedMicro(tickSpeed)
    {
        controllerActive = true;
        globalTime = 0;
        for (int i=0; i<theIntersection->getNumNodes(); ++i)
        {
            std::string src_node_id = std::to_string(i);
            std::vector<Pod*> thisLaneQueue;
            laneQueues.insert({src_node_id, thisLaneQueue});
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
                // Protect shared data
                protectControlledPods.lock();

                // For debugging
                // std::cout << "Entered Scheduler\n";

                // Check vehicle is not being scheduled on top of another vehicle
                std::map<std::string, std::vector<Pod*>>::iterator it = laneQueues.find(entryQueue.front()->getSource()->nodeID);
                if (it->second.empty() || globalTime > it->second.back()->getTimestamp())
                {

                    schedulePod(entryQueue.front());
                    entryQueue.pop();
                }

                // For debugging
                // std::cout << "Exited Scheduler\n";

                // Protect shared data
                protectControlledPods.unlock();
            }
        }
    }

    void updatePods()
    {
        while (controllerActive)
        {
            // Protect shared data
            protectControlledPods.lock();

            // For debugging
            // std::cout << "----\n";
            
            doUpdate();
            globalTime++;
            
            // For debugging
            // std::cout << "----\n\n";

            // Protect shared data
            protectControlledPods.unlock();

            // Update rate at approx UPDATE_GAP_MS between updates
            std::this_thread::sleep_for(std::chrono::microseconds(tickSpeedMicro));
        }
    }

    // Getters

    // Setters

public:
    std::queue<Vehicle*> entryQueue;
    std::mutex protectControlledPods;
    unsigned int tickSpeedMicro;

protected:
    bool controllerActive;
    Intersection* thisIntersection;
    std::vector<Pod*> controlledPods;
    std::map<std::string, std::vector<Pod*>> laneQueues;
    std::vector<Pod*> worldQueue;
    unsigned long int globalTime;
};

#endif