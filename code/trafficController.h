/**
 * Traffic Controller
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Traffic Controller base class. Holds an entry queue along with an intersection
 *      object. Creates two threads, a thread that checks for new entries and a thread
 *      that performs positional updates. Since the threads share data, mutexes are
 *      used for protection. OpenMP is used within the update thread to speed up updates.
 * 
 * Revision History:
 *      30NOV2021  R-11-30: Document Created, initial coding
 *      01DEC2021  R-12-01: Convert class to abstract
 *      04DEC2021  R-12-04: Added lane queues, world queues, and mutexes
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#ifndef TRAFFICCONTROLLER_H
#define TRAFFICCONTROLLER_H

#include "debugSetup.h"
#include <vector>
#include <queue>
#include <map>
#include <thread>
#include <mutex>
#include <omp.h>

#include "intersection.h"
#include "pod.h"

/** TrafficController Class
 *  Description:
 *          Abstract class that serves as foundation for traffic controllers.
 *          Contains a thread for checking for entries and a thread for performing
 *          updates. A global controllerActive boolean determines whether the
 *          the threads stay alive.
 **/
class TrafficController
{
public:
    // Constructors
    TrafficController(Intersection* theIntersection, unsigned int tickSpeed);

    // Destructors
    ~TrafficController();

    // Member Functions
    void startController();
    void stopController();

    // Virtual Member Functions
    virtual void schedulePod(Vehicle* entryVehicle) = 0;
    virtual void doUpdate() = 0;

    // Thread Functions
    void entryCheck();
    void updatePods();

    // Getters
    bool getControllerActive(){return controllerActive;}
    unsigned long int getGlobalTime(){return globalTime;}
    unsigned int getTickSpeedMicro(){return tickSpeedMicro;}

public:
    std::queue<Vehicle*> entryQueue;        // Queue of Vehicles waiting to be scheduled
    std::mutex protectControlledPods;       // Mutex for preventing data races

protected:
    bool controllerActive;                  // Status of controller
    Intersection* thisIntersection;         // Pointer to Intersection object
    std::vector<Pod*> controlledPods;       // Vector of all pods under controller control
    std::map<std::string, std::vector<Pod*>> laneQueues;        // Mapping of lanes and their queues (in the form of a vector)
    std::vector<Pod*> worldQueue;           // Vector of all pods that have not gone through the intersection yet
    unsigned long int globalTime;           // A way to track time
    unsigned int tickSpeedMicro;            // Update speed (How fast time is going)
};

#endif