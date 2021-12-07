/**
 * Light Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Light Traffic Controller class simulates an intersection with traffic lights.
 *      This class is currently a work in progress.
 * 
 * Revision History:
 *      01DEC2021  R-12-01: Document Created, initial coding
 * 
 **/

#include "lightTrafficController.h"

/**
 * schedulePod
 * Inputs:
 *      Vehicle* - Pointer to the vehicle that is joining traffic
 * Outputs: None
 * Description:
 *          Creates a pod for the vehicle and adds it to the world queue
 **/
void LightTrafficController::schedulePod(Vehicle* entryVehicle)
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
void LightTrafficController::doUpdate()
{
    if (DEBUG) {std::cout << "Entered doUpdate\n";}

    // Prepare post multithreading update flags
    bool popWorld = false;
    bool leaveControl = false;

    for (int i=0; i<controlledPods.size(); ++i)
    {
        Pod* thisPod = controlledPods[i];

        if (DEBUG) {std::cout << thisPod->getPodID() << " : " << thisPod->getLane()->getLaneID() << " : " << thisPod->getPosition() << std::endl;}

        // Movement logic
        std::string lane_id = thisPod->getLane()->getLaneID();
        int mvmtSpeed;
        // Check if already through intersection
        if (thisPod->getPosition() > thisPod->getLane()->getBeginIntersection())
        {
            // Already through
            mvmtSpeed = thisPod->getLane()->getDestination()->speedLimit;
        }
        else
        {
            // Not through yet, check traffic light color
            if (trafficLights.find(lane_id)->second == 0)
            {
                // Green
                mvmtSpeed = thisPod->getLane()->getSource()->speedLimit;
            }
            else
            {
                // Yellow or Red, Check position
                double stopTarget = thisPod->getLane()->getBeginIntersection() - thisPod->getPositionInQueue();
                // std::cout << "Stop Target: " << stopTarget << std::endl;
                mvmtSpeed = thisPod->getPosition() < stopTarget ? thisPod->getLane()->getSource()->speedLimit : 0;
            }
        }
        thisPod->updatePosition(mvmtSpeed);

        // Check if pod has gone through intersection square
        if (thisPod->getPosition() > thisPod->getLane()->getEndIntersection())
        {
            std::map<std::string, std::vector<Pod*>>::iterator laneIt = laneQueues.find(thisPod->getLane()->getLaneID());
            Pod* firstPod = laneIt->second.front();
            if (firstPod != NULL && firstPod->getPodID() == thisPod->getPodID())
            {
                laneIt->second.erase(laneIt->second.begin());
                for (int i=0; i<laneIt->second.size(); ++i)
                {
                    laneIt->second[i]->setPositionInQueue(i);
                }
            }
        }

        // Check if pod has left intersection
        if (thisPod->getPosition() > thisPod->getLane()->getLaneLength())
        {
            controlledPods[i] = NULL;
            thisPod->setExitStamp(globalTime);
            delete thisPod;
            leaveControl = true;
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

    if (DEBUG) {std::cout << "Exited doUpdate\n";}
}

/**
 * startLightCycle
 * Inputs: None
 * Outputs: None
 * Description:
 *          Function that sets up traffic lights and calls thread to start light cycles
 **/
void LightTrafficController::startLightCycle()
{
    // Assuming 4W-SL
    lightPhase = 0;

    // Phase 0 - Green
    trafficLights.insert({"0-3", 7});
    trafficLights.insert({"3-0", 7});
    trafficLights.insert({"1-2", 7});
    trafficLights.insert({"2-1", 7});

    // Phase 1 - Green
    trafficLights.insert({"0-2", 5});
    trafficLights.insert({"2-0", 5});
    trafficLights.insert({"0-1", 5});
    trafficLights.insert({"2-3", 5});

    // Phase 2 - Green
    trafficLights.insert({"0-1", 3});
    trafficLights.insert({"1-0", 3});
    trafficLights.insert({"2-3", 3});
    trafficLights.insert({"3-2", 3});

    // Phase 3 - Green
    trafficLights.insert({"1-3", 1});
    trafficLights.insert({"3-1", 1});
    trafficLights.insert({"1-2", 1});
    trafficLights.insert({"3-0", 1});

    // Start traffic light cycles thread
    std::thread (&LightTrafficController::changingLights, this).detach();
}

/**
 * changingLights
 * Inputs: None
 * Outputs: None
 * Description:
 *          Thread function that controls traffic light cycles
 **/
void LightTrafficController::changingLights()
{
    // Make sure stopController has not been called
    while(controllerActive)
    {
        // Turns Phase
        lightPhase = (lightPhase + 1) % 4;

        // Green for 7 seconds
        for (auto it = trafficLights.begin(); it != trafficLights.end(); ++it)
        {
            it->second = (it->second + 1) % 8;
        }
        if (DEBUG) {std::cout << "Turns GREEN!\n";}
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // Yellow for 3 seconds
        for (auto it = trafficLights.begin(); it != trafficLights.end(); ++it)
        {
            it->second = (it->second + 1) % 8;
        }
        if (DEBUG) {std::cout << "Turns YELLOW!\n";}
        std::this_thread::sleep_for(std::chrono::seconds(2));

        // Straights Phase
        lightPhase = (lightPhase + 1) % 4;

        // Green for 7 seconds
        for (auto it = trafficLights.begin(); it != trafficLights.end(); ++it)
        {
            it->second = (it->second + 1) % 8;
        }
        if (DEBUG) {std::cout << "Straights GREEN!\n";}
        std::this_thread::sleep_for(std::chrono::seconds(7));

        // Yellow for 3 seconds
        for (auto it = trafficLights.begin(); it != trafficLights.end(); ++it)
        {
            it->second = (it->second + 1) % 8;
        }
        if (DEBUG) {std::cout << "Straights YELLOW!\n";}
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}