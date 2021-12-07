/**
 * Main
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Program for testing simulation situations.
 *      No display support.
 * 
 * Revision History:
 *      30NOV2021  R-11-30: Document Created, initial coding
 *      04DEC2021  R-12-04: Added TEST_ADDVEHICLES and TEST_STOPCONTROLLER
 *      06DEC2021  R-12-06: Added TEST_TRAFFICJAM
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#include "debugSetup.h"

#include "intersect4wsl.h"
#include "autoTrafficController.h"
#include "lightTrafficController.h"
#include "stopTrafficController.h"

// Default Speed Limit
#define DEFAULT_SPEED_LIMIT 4

// Tests (Only one of them should be on at a time)
#define TEST_INTERSECTION 0
#define TEST_ADDVEHICLES 0
#define TEST_STOPCONTROLLER 0
#define TEST_TRAFFICJAM 1

// Traffic Controller Type
#define AUTO    0
#define LIGHT   1
#define STOP    2
unsigned int controllerType;

// Default Speed Limit
#define DEFAULT_SPEED_LIMIT 4

// Default Tick Speed in microseconds
#define DEFAULT_TICK_MICROS 100000
unsigned int tickSpeed;

// Vehicle Spawn Chance per Iteration
#define DEFAULT_PROB 25
unsigned int probability;

// Global Variables
Intersect4WSL* theIntersection;
TrafficController* theTrafficController;
std::vector<Vehicle*> vehicleCollection;

// Let's run some tests!
int main(int argc, char *argv[])
{
    // Set debug to true
    DEBUG = true;

    /**
     * Command Line Argument Processing
     **/
    controllerType = AUTO;
    tickSpeed = DEFAULT_TICK_MICROS;
    probability = DEFAULT_PROB;
    // Check for second command line argument (--help or Controller Type)
    if (argc >= 2)
    {
        std::string typeArg = argv[1];
        // Check Number of Characters
        if (typeArg.length() != 2)
        {
            std::cerr << "Invalid Second Arg!\n"
                      << "Received " << typeArg << std::endl;
            return 1;
        }
        // Controller Type Flag
        if (typeArg[0] != '-')
        {
            std::cerr << "Invalid Signature!\n"
                      << "Received" << typeArg[0] << std::endl;
            return 1;
        }
        char c = typeArg[1];
        int i_c = c - 'A';
        // Check Controller Flag Type
        switch (i_c)
        {
            case 'A'-'A':
                controllerType = AUTO;
                break;
            case 'L'-'A':
                controllerType = LIGHT; // Will probably not work properly :(
                break;
            case 'S'-'A':
                controllerType = STOP;
                break;
            default:
                std::cerr << "Invalid Command Line Input!\n"
                          << "Received " << typeArg << std::endl;
                return 1;
                break;
        }
    }
    // Check for third command line argument (Tick Speed)
    if (argc >= 3)
    {
        std::string typeArg = argv[2];
        // Check for positive value
        for (int i=0; i<typeArg.length(); ++i)
        {
            if (!isdigit(typeArg[i]))
            {
                std::cerr << "Invalid Command Line Input!\n"
                          << "Found non-digit" << typeArg[i] << std::endl;
                return 1;
            }
        }
        tickSpeed = stoi(typeArg);
    }
    // Check for fourth command line argument (Spawn Probability)
    if (argc >= 4)
    {
        std::string typeArg = argv[3];
        // Check for positive value
        for (int i=0; i<typeArg.length(); ++i)
        {
            if (!isdigit(typeArg[i]))
            {
                std::cerr << "Invalid Command Line Input!\n"
                          << "Found non-digit" << typeArg[i] << std::endl;
                return 1;
            }
        }
        probability = stoi(typeArg);
    }
    // Check for too many args
    if (argc > 4)
    {
        std::cerr << "Too many arguments!\n"
                    << "Received " << argc << ", support only offer up to 4.\n"
                    << "Run --help for more info.\n"
                    << "Quitting...\n";
        return 1;
    }

    /**
     * Environment Setup
     **/
    srand(time(NULL));
    double speedLimit = DEFAULT_SPEED_LIMIT;
    // For scaling the dimensions of the intersection on a screen
    double scale = controllerType == AUTO ? 40 : 36;
    theIntersection = new Intersect4WSL(speedLimit);
    switch (controllerType)
    {
        case AUTO:
            theTrafficController = new AutoTrafficController(theIntersection, tickSpeed);
            break;
        case LIGHT:
            theTrafficController = new LightTrafficController(theIntersection, tickSpeed);
            break;
        case STOP:
            theTrafficController = new StopTrafficController(theIntersection, tickSpeed);
            break;
        default:
            theTrafficController = new AutoTrafficController(theIntersection, tickSpeed);
            break;
    }
    // Start the Controller
    theTrafficController->startController();
    // Start traffic lights thread if a traffic light controller
    if (controllerType == LIGHT)
    {
        LightTrafficController* control = dynamic_cast<LightTrafficController*>(theTrafficController);
        control->startLightCycle();
    }

    // Intersection Setup Testing
    if (TEST_INTERSECTION)
    {
        std::cout << "Testing Intersection Setup\n";
        std::cout << theIntersection->getIntersectionID() <<  std::endl;
        std::string lane_id;
        for (int i=0; i<4; ++i)
        {
            for (int j=0; j<4; ++j)
            {
                if (i==j) {continue;}

                lane_id = std::to_string(i) + "-" + std::to_string(j);
                std::cout << "Lane: " << lane_id <<  std::endl;

                Lane* thisLane = theIntersection->getLane(lane_id);

                std::string testLane;
                for (int k=0; k<4; ++k)
                {
                    for (int l=0; l<4; ++l)
                    {
                        testLane = std::to_string(k) + "-" + std::to_string(l);
                        if (thisLane->isAllowedLane(testLane))
                        {
                            std::cout << testLane <<  std::endl;
                        }
                    }
                }
            }
        }
    }

    // Test adding vehicles
    if (TEST_ADDVEHICLES)
    {
        std::cout << "Testing Adding Vehicles\n";
        Vehicle* testVehicleA = new Vehicle("Alice", 10, 10, 1, theIntersection->getNode("0"), theIntersection->getNode("2"));
        vehicleCollection.push_back(testVehicleA);
        std::cout << "Vehicle pushed to queue\n";
        theTrafficController->entryQueue.push(testVehicleA);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        Vehicle* testVehicleB = new Vehicle("Bob", 10, 10, 1, theIntersection->getNode("1"), theIntersection->getNode("3"));
        vehicleCollection.push_back(testVehicleB);
        std::cout << "Vehicle pushed to queue\n";
        theTrafficController->entryQueue.push(testVehicleB);
        Vehicle* testVehicleC = new Vehicle("Carol", 10, 10, 1, theIntersection->getNode("1"), theIntersection->getNode("2"));
        vehicleCollection.push_back(testVehicleC);
        std::cout << "Vehicle pushed to queue\n";
        theTrafficController->entryQueue.push(testVehicleC);
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }

    // Test Stop Sign Controlled Traffic Controller
    if (TEST_STOPCONTROLLER)
    {
        std::cout << "Testing Stop Sign Controlled Traffic Controller\n";
        Vehicle* testVehicleA = new Vehicle("Alice", 10, 10, 1, theIntersection->getNode("0"), theIntersection->getNode("2"));
        vehicleCollection.push_back(testVehicleA);
        std::cout << "Vehicle pushed to queue\n";
        theTrafficController->entryQueue.push(testVehicleA);
        Vehicle* testVehicleB = new Vehicle("Bob", 10, 10, 1, theIntersection->getNode("1"), theIntersection->getNode("3"));
        vehicleCollection.push_back(testVehicleB);
        std::cout << "Vehicle pushed to queue\n";
        theTrafficController->entryQueue.push(testVehicleB);
        Vehicle* testVehicleC = new Vehicle("Carol", 10, 10, 1, theIntersection->getNode("1"), theIntersection->getNode("2"));
        vehicleCollection.push_back(testVehicleC);
        std::cout << "Vehicle pushed to queue\n";
        theTrafficController->entryQueue.push(testVehicleC);
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }

    // Test Traffic Jame
    if (TEST_TRAFFICJAM)
    {
        std::cout << "Testing Traffic Jam\n";
        for (int i=0; i<10; ++i)
        {
            int dest = (rand() % 2) + 2;
            int l = rand() % 2;
            Vehicle* testVehicle = new Vehicle(std::to_string(i) + std::to_string(l), 10, 10, 1, theIntersection->getNode(std::to_string(l)), theIntersection->getNode(std::to_string(dest)));    
            std::cout << "Vehicle pushed to queue\n";
            vehicleCollection.push_back(testVehicle);
            theTrafficController->entryQueue.push(testVehicle);
        }
        std::this_thread::sleep_for(std::chrono::seconds(30));
    }

    // Cleanup
    theTrafficController->stopController();
    delete theTrafficController;
    delete theIntersection;
    for (int i=0; i<vehicleCollection.size(); ++i)
    {
        delete vehicleCollection[i];
    }
    vehicleCollection.clear();
    return 0;
}
