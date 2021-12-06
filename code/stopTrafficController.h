/**
 * Stop Sign-Controlled Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Stop Sign-Controlled Traffic Controller
 * 
 * Revision History:
 *      01DEC2021  R-12-01: Document Created, initial coding
 *      04DEC2021  R-12-04: Re-write following TrafficController
 *                          class update.
 * 
 **/

#ifndef STOPTRAFFICCONTROLLER_H
#define STOPTRAFFICCONTROLLER_H

#include "trafficController.h"

class StopTrafficController: public TrafficController
{
public:
    // Constructors
    StopTrafficController(Intersection* theIntersection):TrafficController(theIntersection){}

    // Member Functions
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
        it->second.push_back(entryPod);
        for (int i=0; i<it->second.size(); ++i)
        {
            it->second[i]->setPositionInQueue(i);
        }

        // Add pod to world queue
        worldQueue.push_back(entryPod);
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

            // Check if pod has left intersection control
            if (thisPod->getPosition() > thisPod->getLane()->getLaneLength())
            {
                controlledPods.erase(it);
                thisPod->getVehicle()->setTrafficControl(false);
                delete thisPod;
            }
            // Check if pod is beyond intersection but has not left yet
            else if (thisPod->getPosition() > thisPod->getLane()->getEndIntersection())
            {
                thisPod->updatePosition(thisPod->getLane()->getDestination()->speedLimit);
            }
            // Check if pod is in intersection
            else if (thisPod->getPosition() > thisPod->getLane()->getBeginIntersection())
            {
                if (thisPod->getPosition() + thisPod->getLane()->getDestination()->speedLimit > thisPod->getLane()->getEndIntersection())
                {
                    worldQueue.pop_front();
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
                    // Check if first in world queue
                    if (worldQueue.front()->getPodID() == thisPod->getPodID())
                    {
                        std::map<std::string, std::deque<Pod*>>::iterator laneIt = laneQueues.find(thisPod->getLane()->getSource()->nodeID);
                        laneIt->second.pop_front();
                        // Update queue positions
                        for (int i=0; i<laneIt->second.size(); ++i)
                        {
                            laneIt->second[i]->setPositionInQueue(i);
                        }
                        thisPod->updatePosition(thisPod->getLane()->getDestination()->speedLimit);                        
                    }
                    else
                    {
                        thisPod->updatePosition(0);
                    }
                }
                else
                {
                    std::cout << "Stop!\n";
                    thisPod->updatePosition(0,3);
                }
            }
            // Check if pod is right before stop target in queue
            else if (thisPod->getPosition() + thisPod->getLane()->getSource()->speedLimit >= thisPod->getLane()->getBeginIntersection() - thisPod->getPositionInQueue())
            {
                thisPod->updatePosition(thisPod->getLane()->getBeginIntersection() - thisPod->getPositionInQueue() - thisPod->getPosition());
            }
            // Pod is approaching intersection
            else
            {
                thisPod->updatePosition(thisPod->getLane()->getSource()->speedLimit);
            }

            if (controlledPods.empty())
            {
                break;
            }
        }
    }
};

#endif