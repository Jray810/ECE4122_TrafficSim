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
#include <mutex>
#include <omp.h>

#define UPDATE_GAP_MS 100

class TrafficController
{
public:
    // Constructors
    TrafficController(Intersection* theIntersection):thisIntersection(theIntersection)
    {
        controllerActive = true;
        globalTime = 0;
        for (int i=0; i<theIntersection->getNumNodes(); ++i)
        {
            std::string src_node_id = std::to_string(i);
            std::deque<Pod*> thisLaneQueue;
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
                // For debugging
                // std::cout << "Entered Scheduler\n";

                // Check vehicle is not being scheduled on top of another vehicle
                std::map<std::string, std::deque<Pod*>>::iterator it = laneQueues.find(entryQueue.front()->getSource()->nodeID);
                // Protect shared data
                protectControlledPods.lock();
                if (it->second.size() == 0 || globalTime > it->second.back()->getTimestamp())
                {
                    schedulePod(entryQueue.front());
                    entryQueue.pop();
                }
                // Protect shared data
                protectControlledPods.unlock();

                // For debugging
                // std::cout << "Exited Scheduler\n";
            }
        }
    }

    void updatePods()
    {
        while (controllerActive)
        {
            // For debugging
            std::cout << "----\n";

            // Protect shared data
            protectControlledPods.lock();
            
            doUpdate();
            globalTime++;

            // Protect shared data
            protectControlledPods.unlock();
            
            // For debugging
            std::cout << "----\n\n";

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
    std::deque<Pod*> worldQueue;
    std::mutex protectControlledPods;
    unsigned long int globalTime;
};

#endif