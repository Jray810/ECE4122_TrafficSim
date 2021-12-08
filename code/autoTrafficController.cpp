/**
 * Autonomous Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Function implementation for AutoTrafficController class
 * 
 * Revision History:
 *      01DEC2021  R-12-01: Document Created, initial coding
 *      04DEC2021  R-12-04: Re-write following TrafficController
 *                          class update.
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#include "autoTrafficController.h"

/**
 * setPodEntry
 * Inputs:
 *      Pod* - Pointer to the pod being scheduled
 *      unsigned long int - The desired entry time of the pod
 * Outputs: None
 * Description:
 *          Sets the entry time of the pod and adjusts
 *          the worldQueue so that it remains ordered in
 *          terms of entry times.
 **/
void AutoTrafficController::setPodEntry(Pod* thePod, unsigned long int desiredEntry)
{
    if (DEBUG) {std::cout << "Entered setPodEntry\n";}

    // Set the pod's entry time
    thePod->setTarget(desiredEntry, globalTime);
    
    // Update the worldQueue to maintain ascending entry times
    if (worldQueue.empty())
    {
        // First pod in world queue
        worldQueue.push_back(thePod);
    }
    else
    {
        // Iterate through world queue until correct spot is found, then insert
        for (int i=0; i<worldQueue.size(); ++i)
        {
            if (desiredEntry < worldQueue[i]->getEntry())
            {
                std::vector<Pod*>::iterator it = worldQueue.begin() + i;
                worldQueue.insert(it, thePod);
                if (DEBUG) {std::cout << "Exited set pod entry\n";}
                return;
            }
        }
        worldQueue.push_back(thePod);
    }

    if (DEBUG) {std::cout << "Exited setPodEntry\n";}
}

/**
 * schedulePod
 * Inputs:
 *      Vehicle* - Pointer to the vehicle that is joining traffic
 * Outputs: None
 * Description:
 *          Creates a pod for the vehicle and attempts to find the earliest
 *          timeslot at which the pod can enter the intersection without causing a collision
 **/
void AutoTrafficController::schedulePod(Vehicle* entryVehicle)
{
    if (DEBUG) {std::cout << "Entered schedulePod\n";}

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
    unsigned long int earliestEntryTime = it->second.empty() ? 0 : it->second.back()->getEntry() + 1;
    if (entryPod->predictedEntry(globalTime) > earliestEntryTime)
    {
        earliestEntryTime = entryPod->predictedEntry(globalTime);
    }
    it->second.push_back(entryPod);

    // Check if worldQueue is empty
    if (worldQueue.empty())
    {
        setPodEntry(entryPod, earliestEntryTime);
        if (DEBUG) {std::cout << "Exited schedulePod\n";}
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
            if (DEBUG) {std::cout << "Exited schedulePod\n";}
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
                if (thisPod->getEntry() - bannedExit >= entryPod->getTimeInIntersection())
                {
                    setPodEntry(entryPod, bannedExit);
                    if (DEBUG) {std::cout << "Exited schedulePod\n";}
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
    if (DEBUG) {std::cout << "Exited schedulePod\n";}
}

/**
 * doUpdate
 * Inputs: None
 * Outputs: None
 * Description:
 *          Loops through all controlled pods and updates their position
 *          Uses OpenMP to speed up the update process
 **/
void AutoTrafficController::doUpdate()
{
    if (DEBUG) {std::cout << "Entered doUpdate\n";}

    // Prepare post multithreading update flags
    bool popWorld = false;
    bool leaveControl = false;
    std::vector<std::string> popLane;

    // Begin parallel computing for update
    int i, N, tid;
    N = controlledPods.size();

#pragma omp parallel shared(i, N) private(tid)
    {
        tid = omp_get_thread_num();

#pragma omp for schedule(dynamic)
        for (i=0; i<N; ++i)
        {
            Pod* thisPod = controlledPods[i];

            if (DEBUG) {std::cout << thisPod->getPodID() << " : " << thisPod->getLane()->getLaneID() << " : " << thisPod->getPosition() << std::endl;}

            // Check if in slowed region
            if (thisPod->getCountdown() > 0 && thisPod->getPosition() <= thisPod->getLane()->getBeginIntersection())
            {
                if (DEBUG) {std::cout << "Countdown: " << thisPod->getCountdown() << std::endl;}
                thisPod->updatePosition(thisPod->getLane()->getSource()->speedLimit*3/4);
            }
            // Check if in go region before intersection
            else if (thisPod->getPosition() <= thisPod->getLane()->getBeginIntersection())
            {
                if (DEBUG) {std::cout << "Normal...\n";}
                thisPod->updatePosition(thisPod->getLane()->getSource()->speedLimit);
            }
            // Check if in intersection
            else if (thisPod->getPosition() <= thisPod->getLane()->getEndIntersection())
            {
                // Check if first in lane queue, if so, signal removal from lane queue
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
                // Check if first in world queue, if so, signal removal from world queue
                if (!worldQueue.empty() && worldQueue[0]->getPodID() == thisPod->getPodID())
                {
                    popWorld = true;
                }
                thisPod->updatePosition(thisPod->getLane()->getDestination()->speedLimit);
            }
            // Pod has left intersection control
            else
            {
                // Get rid of this pod
                controlledPods[i] = NULL;
                thisPod->setExitStamp(globalTime);
                delete thisPod;
                leaveControl = true;
            }
        }
    }

    // Signal received to pop the world queue
    if (popWorld)
    {
        worldQueue.erase(worldQueue.begin());
    }

    // Signal received that a pod left control
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

    // Signal received to pop a lane queue
    if (!popLane.empty())
    {
        for (int i=0; i<popLane.size(); ++i)
        {
            laneQueues.find(popLane[i])->second.erase(laneQueues.find(popLane[i])->second.begin());
        }
    }

    if (DEBUG) {std::cout << "Exited doUpdate\n";}
}