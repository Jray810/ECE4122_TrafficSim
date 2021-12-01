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
    void schedulePod(Vehicle* entryVehicle)
    {
        entryVehicle->setTrafficControl(true);
        std::string lane_id = entryVehicle->getSource()->nodeID + "-" + entryVehicle->getDestination()->nodeID;
        Lane* desiredLane = thisIntersection->getLane(lane_id);
        Pod* entryPod = new Pod(entryVehicle, desiredLane);
        controlledPods.insert({entryPod->getPodID(), entryPod});
        std::map<std::string, std::deque<Pod*>>::iterator it = laneQueues.find(lane_id);
        it->second.push_back(entryPod);

        // Give instructions to pod if necessary, to meet specified interesction entry time
    }

    void doUpdate()
    {
        // For debugging
        std::cout << "----\n";

        for (auto it = controlledPods.begin(); it != controlledPods.end(); ++it)
        {
            std::string podID = it->first;
            Pod* thisPod = it->second;
            thisPod->updatePosition(0);

            // For debugging
            std::cout << podID << " : " << thisPod->getPosition() << std::endl;

            // Check if pod has gone through intersection square
            if (thisPod->getPosition() > thisPod->getLane()->getEndIntersection())
            {
                std::map<std::string, std::deque<Pod*>>::iterator laneIt = laneQueues.find(thisPod->getLane()->getLaneID());
                Pod* firstPod = laneIt->second.front();
                if (firstPod != NULL && firstPod->getPodID() == thisPod->getPodID())
                {
                    laneIt->second.pop_front();
                }
            }

            // Check if pod has left intersection
            if (thisPod->getPosition() > thisPod->getLane()->getLaneLength())
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

        // For debugging
        std::cout << "----\n\n";
    }
};

#endif