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
    AutoTrafficController(Intersection* theIntersection, unsigned int tickSpeed):TrafficController(theIntersection, tickSpeed){}

    // Member Functions
    void setPodEntry(Pod* thePod, unsigned long int desiredEntry)
    {
        // std::cout << "Entered set pod entry\n";
        // std::cout << "Desired: " << desiredEntry << "  Current: " << globalTime << std::endl;
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
                    std::vector<Pod*>::iterator it = worldQueue.begin() + i;
                    worldQueue.insert(it, thePod);
                    // std::cout << "Exited set pod entry\n";
                    return;
                }
            }
            worldQueue.push_back(thePod);
        }
        // std::cout << "Exited set pod entry\n";
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
        controlledPods.push_back(entryPod);
        
        // Add pod to lane queue
        std::map<std::string, std::vector<Pod*>>::iterator it = laneQueues.find(src_node_id);
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
        bool popWorld = false;
        bool leaveControl = false;
        std::vector<std::string> popLane;

        // Begin parallel computing
        int i, N, tid;
        N = controlledPods.size();

#pragma omp parallel shared(i, N) private(tid)
        {
            tid = omp_get_thread_num();

#pragma omp for schedule(dynamic)
            for (i=0; i<N; ++i)
            {
                Pod* thisPod = controlledPods[i];

                // For debugging
                // std::cout << thisPod->getPodID() << " : " << thisPod->getPosition() << std::endl;
                // std::cout << thisPod->getLane()->getLaneID() << " : " << thisPod->getPosition() << std::endl;

                // Check if in slowed region
                if (thisPod->getCountdown() > 0)
                {
                    // std::cout << "Countdown: " << thisPod->getCountdown() << std::endl;
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
                    std::map<std::string, std::vector<Pod*>>::iterator laneIt = laneQueues.find(thisPod->getLane()->getSource()->nodeID);
                    if (!laneIt->second.empty() && laneIt->second.front()->getPodID() == thisPod->getPodID())
                    {
                        popLane.push_back(thisPod->getLane()->getSource()->nodeID);
                    }
                    thisPod->updatePosition(thisPod->getLane()->getDestination()->speedLimit);
                }
                // Check if pod is beyond intersection but has not left yet
                else if (thisPod->getPosition() <= thisPod->getLane()->getLaneLength())
                {
                    if (!worldQueue.empty() && worldQueue[0]->getPodID() == thisPod->getPodID())
                    {
                        popWorld = true;
                    }
                    thisPod->updatePosition(thisPod->getLane()->getDestination()->speedLimit);
                }
                // Pod has left intersection control
                else
                {
                    controlledPods[i] = NULL;
                    thisPod->setExitStamp(globalTime);
                    delete thisPod;
                    leaveControl = true;
                }
            }
        }

        if (popWorld)
        {
            worldQueue.erase(worldQueue.begin());
        }

        if (leaveControl)
        {
            for (int i=0; i<controlledPods.size(); ++i)
            {
                if (controlledPods[i] == NULL)
                {
                    controlledPods.erase(controlledPods.begin() + i);
                    i--;
                }
            }
        }
        
        if (!popLane.empty())
        {
            for (int i=0; i<popLane.size(); ++i)
            {
                laneQueues.find(popLane[i])->second.erase(laneQueues.find(popLane[i])->second.begin());
            }
        }
    }
};

#endif