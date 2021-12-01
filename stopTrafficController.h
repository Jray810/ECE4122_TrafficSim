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
 * 
 **/

#ifndef STOPTRAFFICCONTROLLER_H
#define STOPTRAFFICCONTROLLER_H

#include "trafficController.h"

class StopTrafficController: public TrafficController
{
public:
    // Constructors
    StopTrafficController(Intersection* theIntersection):TrafficController(theIntersection)
    {
        for (int i=0; i<theIntersection->getNumNodes(); ++i)
        {
            std::string node_id = std::to_string(i);
            std::deque<Pod*> thisNodeQueue;
            nodeQueues.insert({node_id, thisNodeQueue});
        }
    }

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

        worldQueue.push_back(entryPod);

        std::string src_node_id = entryPod->getLane()->getSource()->nodeID;
        std::map<std::string, std::deque<Pod*>>::iterator nodeIt = nodeQueues.find(src_node_id);
        nodeIt->second.push_back(entryPod);
        for (int i=0; i<nodeIt->second.size(); ++i)
        {
            nodeIt->second[i]->setPositionInQueue(i);
        }
    }

    void doUpdate()
    {
        // For debugging
        std::cout << "----\n";

        std::map<std::string, Pod*>::iterator next_it = controlledPods.begin();
        for (std::map<std::string, Pod*>::iterator it = controlledPods.begin(); it != controlledPods.end(); it = next_it)
        {
            ++next_it;
            std::string podID = it->first;
            Pod* thisPod = it->second;
            double pos = thisPod->getPosition();

            // Movement Logic
            int mvmtSpeed;
            int countdown = -1;
            if (pos > thisPod->getLane()->getEndIntersection())
            {
                mvmtSpeed = thisPod->getLane()->getDestination()->speedLimit;
            }
            else if (pos > thisPod->getLane()->getBeginIntersection())
            {
                mvmtSpeed = thisPod->getLane()->getDestination()->speedLimit;
                if (pos == thisPod->getLane()->getEndIntersection())
                {
                    worldQueue.pop_front();
                }
            }
            else if (pos == thisPod->getLane()->getBeginIntersection())
            {
                if (thisPod->getCountdown() == 0)
                {
                    // Check if first in world queue
                    if (worldQueue.front()->getPodID() == thisPod->getPodID())
                    {
                        mvmtSpeed = thisPod->getLane()->getDestination()->speedLimit;
                        std::map<std::string, std::deque<Pod*>>::iterator nodeIt = nodeQueues.find(thisPod->getLane()->getSource()->nodeID);
                        nodeIt->second.pop_front();
                        // Update queue positions
                        for (int i=0; i<nodeIt->second.size(); ++i)
                        {
                            nodeIt->second[i]->setPositionInQueue(i);
                        }
                    }
                    else
                    {
                        mvmtSpeed = 0;
                    }
                }
                else if (thisPod->getCountdown() > 0)
                {
                    mvmtSpeed = 0;
                }
                else
                {
                    std::cout << "Stop!\n";
                    mvmtSpeed = 0;
                    countdown = 5;
                }
            }
            else
            {
                double stopTarget = thisPod->getLane()->getBeginIntersection() - thisPod->getPositionInQueue();
                mvmtSpeed = pos < stopTarget ? thisPod->getLane()->getSource()->speedLimit : 0;
            }
            thisPod->updatePosition(mvmtSpeed, countdown);

            // For debugging
            std::cout << podID << " : " << thisPod->getPosition() << std::endl;

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

protected:
    std::deque<Pod*> worldQueue;
    std::map<std::string, std::deque<Pod*>> nodeQueues;
};

#endif