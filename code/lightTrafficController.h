/**
 * Light-Controlled Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Light-Controlled Traffic Controller
 * 
 * Revision History:
 *      01DEC2021  R-12-01: Document Created, initial coding
 * 
 **/

#ifndef LIGHTTRAFFICCONTROLLER_H
#define LIGHTTRAFFICCONTROLLER_H

#include "trafficController.h"

class LightTrafficController: public TrafficController
{
public:
    // Constructors
    LightTrafficController(Intersection* theIntersection, unsigned int tickSpeed):TrafficController(theIntersection, tickSpeed){}

    // Member Functions
    void schedulePod(Vehicle* entryVehicle)
    {
        entryVehicle->setTrafficControl(true);
        std::string lane_id = entryVehicle->getSource()->nodeID + "-" + entryVehicle->getDestination()->nodeID;
        Lane* desiredLane = thisIntersection->getLane(lane_id);
        Pod* entryPod = new Pod(entryVehicle, desiredLane);
        controlledPods.push_back(entryPod);
        std::map<std::string, std::vector<Pod*>>::iterator it = laneQueues.find(lane_id);
        it->second.push_back(entryPod);
        for (int i=0; i<it->second.size(); ++i)
        {
            it->second[i]->setPositionInQueue(i);
        }
    }

    void doUpdate()
    {
        bool popWorld = false;
        bool leaveControl = false;

        // For debugging
        // std::cout << "----\n";

        for (int i=0; i<controlledPods.size(); ++i)
        {
            Pod* thisPod = controlledPods[i];

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

            // For debugging
            // std::cout << thisPod->getPodID() << " : " << thisPod->getPosition() << std::endl;

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

        // For debugging
        // std::cout << "----\n\n";
    }

    void startLightCycle()
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

        std::thread (&LightTrafficController::changingLights, this).detach();
    }

    // Thread Functions
    void changingLights()
    {
        while(controllerActive)
        {
            // Turns Phase
            lightPhase = (lightPhase + 1) % 4;

            // Green for 7 seconds
            for (auto it = trafficLights.begin(); it != trafficLights.end(); ++it)
            {
                it->second = (it->second + 1) % 8;
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));

            // Yellow for 3 seconds
            for (auto it = trafficLights.begin(); it != trafficLights.end(); ++it)
            {
                it->second = (it->second + 1) % 8;
            }
            std::this_thread::sleep_for(std::chrono::seconds(2));

            // Straights Phase
            lightPhase = (lightPhase + 1) % 4;

            // Green for 7 seconds
            for (auto it = trafficLights.begin(); it != trafficLights.end(); ++it)
            {
                it->second = (it->second + 1) % 8;
            }
            std::this_thread::sleep_for(std::chrono::seconds(7));

            // Yellow for 3 seconds
            for (auto it = trafficLights.begin(); it != trafficLights.end(); ++it)
            {
                it->second = (it->second + 1) % 8;
            }
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }

public:
    int lightPhase;
    std::map<std::string, int> trafficLights;
};

#endif