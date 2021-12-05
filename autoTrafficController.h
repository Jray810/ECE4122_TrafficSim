/**
 * Autonomous Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Autonomous Traffic Controller
 * 
 * Revision History:
 *      01DEC2021  R-12-01: Document Created, initial coding
 *      04DEC2021  R-12-04: Re-write following TrafficController
 *                          class update.
 * 
 **/

#ifndef AUTOTRAFFICCONTROLLER_H
#define AUTOTRAFFICCONTROLLER_H

#include "trafficController.h"

class AutoTrafficController: public TrafficController
{
public:
    // Constructors
    AutoTrafficController(Intersection* theIntersection):TrafficController(theIntersection){}

    // Member Functions
    void setPodEntry(Pod* thePod, unsigned long int desiredEntry)
    {
        std::cout << "Entered set pod entry\n";
        std::cout << "Desired: " << desiredEntry << "  Current: " << globalTime << std::endl;
        thePod->setTarget(desiredEntry, globalTime);
        if (worldQueue.empty())
        {
            worldQueue.push_back(thePod);
        }
        else
        {
            for (int i=0; i<worldQueue.size(); ++i)
            {
                if (desiredEntry < worldQueue[i]->getEntry())
                {
                    std::deque<Pod*>::iterator it = worldQueue.begin() + i;
                    worldQueue.insert(it, thePod);
                    std::cout << "Exited set pod entry\n";
                    return;
                }
            }
            worldQueue.push_back(thePod);
        }
        std::cout << "Exited set pod entry\n";
    }

    void schedulePod(Vehicle* entryVehicle)
    {
        // Give vehicle control to traffic controller
        entryVehicle->setTrafficControl(true);
        
        // Create a pod
        std::string lane_id = entryVehicle->getSource()->nodeID + "-" + entryVehicle->getDestination()->nodeID;
        Lane* desiredLane = thisIntersection->getLane(lane_id);
        std::string src_node_id = desiredLane->getSource()->nodeID;
        Pod* entryPod = new Pod(entryVehicle, desiredLane, globalTime);

        // Add to controlled pods
        controlledPods.insert({entryPod->getPodID(), entryPod});
        
        // Add pod to lane queue
        std::map<std::string, std::deque<Pod*>>::iterator it = laneQueues.find(src_node_id);
        unsigned long int earliestEntryTime = it->second.size() == 0 ? 0 : it->second.back()->getEntry() + 1;
        unsigned long int timeInIntersection = entryPod->getTimeInIntersection();
        if (entryPod->predictedEntry(globalTime) > earliestEntryTime)
        {
            earliestEntryTime = entryPod->predictedEntry(globalTime);
        }
        it->second.push_back(entryPod);

        // Check if worldQueue is empty
        if (worldQueue.empty())
        {
            setPodEntry(entryPod, earliestEntryTime);
            return;
        }

        // Go through worldQueue and look for an open timeslot
        unsigned long int bannedEntry = 0;
        unsigned long int bannedExit = earliestEntryTime;
        for (int i=0; i<worldQueue.size(); ++i)
        {
            Pod* thisPod = worldQueue[i];
            // Ignore pods that leave the intersection before we can possibly arrive
            if (thisPod->getExit() < bannedExit)
            {
                continue;
            }
            // If sufficient time between ban and this pod
            if (thisPod->getEntry() >= bannedExit + entryPod->getTimeInIntersection())
            {
                setPodEntry(entryPod, bannedExit);
                return;
            }

            // If pod is allowed
            if (entryPod->getLane()->isAllowedLane(thisPod->getLane()->getLaneID()))
            {
                continue;
            }
            else
            {
                // Check if current pod extends banned period
                if (bannedEntry <= thisPod->getEntry() && thisPod->getEntry() <= bannedExit)
                {
                    bannedExit = thisPod->getExit() > bannedExit ? thisPod->getExit() : bannedExit;
                    continue;
                }
                else
                {
                    // Current pod's entry point is greater than bannedExit
                    // Check if sufficient space between bannedExit and current pod entry for our pod
                    if (thisPod->getEntry() - bannedExit >= timeInIntersection)
                    {
                        setPodEntry(entryPod, bannedExit);
                        return;
                    }
                    else
                    {
                        bannedExit = thisPod->getExit() > bannedExit ? thisPod->getExit() : bannedExit;
                        continue;
                    }
                }
            } 
        }

        // Went through worldQueue and didn't find any open slots
        setPodEntry(entryPod, bannedExit);
    }

    void doUpdate()
    {
        std::map<std::string, Pod*>::iterator next_it = controlledPods.begin();
        for (std::map<std::string, Pod*>::iterator it = controlledPods.begin(); it != controlledPods.end(); it = next_it)
        {
            ++next_it;
            std::string podID = it->first;
            Pod* thisPod = it->second;

            // For debugging
            std::cout << podID << " : " << thisPod->getPosition() << std::endl;

            // Check if in slowed region
            if (thisPod->getCountdown() > 0)
            {
                std::cout << "Countdown: " << thisPod->getCountdown() << std::endl;
                thisPod->updatePosition(thisPod->getLane()->getSource()->speedLimit*3/4);
            }
            // Check if in go region before intersection
            else if (thisPod->getPosition() <= thisPod->getLane()->getBeginIntersection())
            {
                // std::cout << "Normal...\n";
                thisPod->updatePosition(thisPod->getLane()->getSource()->speedLimit);
            }
            // Check if in intersection
            else if (thisPod->getPosition() <= thisPod->getLane()->getEndIntersection())
            {
                std::map<std::string, std::deque<Pod*>>::iterator laneIt = laneQueues.find(thisPod->getLane()->getSource()->nodeID);
                Pod* firstPod = laneIt->second.front();
                if (firstPod != NULL && firstPod->getPodID() == thisPod->getPodID())
                {
                    laneIt->second.pop_front();
                }
                thisPod->updatePosition(thisPod->getLane()->getDestination()->speedLimit);
            }
            // Check if pod is beyond intersection but has not left yet
            else if (thisPod->getPosition() <= thisPod->getLane()->getLaneLength())
            {
                Pod* firstPod = worldQueue[0];
                if (firstPod != NULL && firstPod->getPodID() == thisPod->getPodID())
                {
                    worldQueue.pop_front();
                }
                thisPod->updatePosition(thisPod->getLane()->getDestination()->speedLimit);
            }
            // Pod has left intersection control
            else
            {
                controlledPods.erase(it);
                thisPod->getVehicle()->setTrafficControl(false);
                delete thisPod;
            }

            if (controlledPods.empty())
            {
                break;
            }
        }
    }
};

#endif