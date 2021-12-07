/**
 * Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Function implementation for trafficController class
 * 
 * Revision History:
 *      30NOV2021  R-11-30: Document Created, initial coding
 *      01DEC2021  R-12-01: Convert class to abstract
 *      04DEC2021  R-12-04: Added lane queues, world queues, and mutexes
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#include "trafficController.h"

// Constructor
TrafficController::TrafficController(Intersection* theIntersection, unsigned int tickSpeed)
    :thisIntersection(theIntersection), tickSpeedMicro(tickSpeed)
    {
        controllerActive = true;
        globalTime = 0;
        // Initialize mapping of lane queues
        for (int i=0; i<theIntersection->getNumNodes(); ++i)
        {
            std::string src_node_id = std::to_string(i);
            std::vector<Pod*> thisLaneQueue;
            laneQueues.insert({src_node_id, thisLaneQueue});
        }
    }

// Destructor
TrafficController::~TrafficController()
{
    for (int i=0; i<controlledPods.size(); ++i)
    {
        delete controlledPods[i];
    }
    controlledPods.clear();
}

/**
 * startController
 * Inputs: None
 * Outputs: None
 * Description:
 *          Starts entryCheck and updatePods threads
 **/
void TrafficController::startController()
{
    std::thread (&TrafficController::entryCheck, this).detach();
    std::thread (&TrafficController::updatePods, this).detach();
}

/**
 * stopController
 * Inputs: None
 * Outputs: None
 * Description:
 *          Sets controllerActive to false, triggers stoppage of threads
 **/
void TrafficController::stopController()
{
    controllerActive = false;
}

/**
 * entryCheck
 * Inputs: None
 * Outputs: None
 * Description:
 *          Thread function that runs continuously, checks entryQueue
 *          for activity and schedules any new vehicles
 **/
void TrafficController::entryCheck()
{
    // Make sure stopController has not been called
    while (controllerActive)
    {
        // Check if there is something in the queue
        if (!entryQueue.empty())
        {
            // Protect shared data
            protectControlledPods.lock();

            if (DEBUG) {std::cout << "Entered Scheduler\n";}

            // Check vehicle is not being scheduled on top of another vehicle
            std::map<std::string, std::vector<Pod*>>::iterator it = laneQueues.find(entryQueue.front()->getSource()->nodeID);
            if (it->second.empty() || globalTime > it->second.back()->getTimestamp())
            {
                // Safe to schedule, do scheduling
                schedulePod(entryQueue.front());
                entryQueue.pop();
            }

            if (DEBUG) {std::cout << "Exited Scheduler\n";}

            // Protect shared data
            protectControlledPods.unlock();
        }
    }
}

/**
 * updatePods
 * Inputs: None
 * Outputs: None
 * Description:
 *          Thread function that runs continuously, updates
 *          position of all pods in intersection control
 **/
void TrafficController::updatePods()
{
    // Make sure stopController has not been called
    while (controllerActive)
    {
        // Protect shared data
        protectControlledPods.lock();

        if (DEBUG) {std::cout << "----\n";}
        
        // Perform update
        doUpdate();
        // Update global tick clock
        globalTime++;
        
        if (DEBUG) {std::cout << "----\n\n";}

        // Protect shared data
        protectControlledPods.unlock();

        // Update rate at approx tickSpeedMicro microseconds between updates
        std::this_thread::sleep_for(std::chrono::microseconds(tickSpeedMicro));
    }
}