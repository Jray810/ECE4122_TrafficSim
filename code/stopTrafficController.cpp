/**
 * Stop Sign Traffic Controller
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

#include "stopTrafficController.h"

/**
 * schedulePod
 * Inputs:
 *      Vehicle* - Pointer to the vehicle that is joining traffic
 * Outputs: None
 * Description:
 *          Creates a pod for the vehicle and adds it to the world queue
 **/
void StopTrafficController::schedulePod(Vehicle* entryVehicle)
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
    it->second.push_back(entryPod);
    for (int i=0; i<it->second.size(); ++i)
    {
        it->second[i]->setPositionInQueue(i);
    }

    // Add pod to world queue
    worldQueue.push_back(entryPod);

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
void StopTrafficController::doUpdate()
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

            // Check if pod has left intersection control
            if (thisPod->getPosition() > thisPod->getLane()->getLaneLength())
            {
                // Get rid of this pod
                controlledPods[i] = NULL;
                thisPod->setExitStamp(globalTime);
                delete thisPod;
                leaveControl = true;
            }
            // Check if pod is beyond intersection but has not left yet
            else if (thisPod->getPosition() > thisPod->getLane()->getEndIntersection())
            {
                thisPod->updatePosition(thisPod->getLane()->getDestination()->speedLimit);
            }
            // Check if pod is in intersection
            else if (thisPod->getPosition() > thisPod->getLane()->getBeginIntersection())
            {
                // Leaving intersection, signal removal from world queue
                if (thisPod->getPosition() + thisPod->getLane()->getDestination()->speedLimit > thisPod->getLane()->getEndIntersection())
                {
                    popWorld = true;
                }
                thisPod->updatePosition(thisPod->getLane()->getDestination()->speedLimit);
            }
            // Check if pod is stopped at intersection
            else if (thisPod->getPosition() == thisPod->getLane()->getBeginIntersection())
            {
                // Check stop timer
                if (thisPod->getCountdown() > 0)
                {                    
                    thisPod->updatePosition(0);
                }
                else if (thisPod->getCountdown() == 0)
                {
                    // Check if first in world queue, if so, go time!
                    if (worldQueue.front()->getPodID() == thisPod->getPodID())
                    {
                        // Signal removal from lane queue
                        std::map<std::string, std::vector<Pod*>>::iterator laneIt = laneQueues.find(thisPod->getLane()->getSource()->nodeID);
                        popLane.push_back(thisPod->getLane()->getSource()->nodeID);
                        thisPod->updatePosition(thisPod->getLane()->getDestination()->speedLimit);                        
                    }
                    else
                    {
                        thisPod->updatePosition(0);
                    }
                }
                else
                {
                    if (DEBUG) {std::cout << "Stop!\n";}
                    thisPod->updatePosition(0,3);
                }
            }
            // Check if pod is right before stop target in queue
            else if (thisPod->getPosition() + thisPod->getLane()->getSource()->speedLimit > thisPod->getLane()->getBeginIntersection() - thisPod->getPositionInQueue())
            {
                thisPod->updatePosition(thisPod->getLane()->getBeginIntersection() - thisPod->getPositionInQueue() - thisPod->getPosition());
            }
            // Pod is approaching intersection
            else
            {
                thisPod->updatePosition(thisPod->getLane()->getSource()->speedLimit);
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
            // Update queue positions
            for (int j=0; j<laneQueues.find(popLane[i])->second.size(); ++j)
            {
                laneQueues.find(popLane[i])->second[j]->setPositionInQueue(j);
            }
        }
    }

    if (DEBUG) {std::cout << "Exited doUpdate\n";}
}