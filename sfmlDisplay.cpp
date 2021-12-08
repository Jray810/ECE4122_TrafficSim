/**
 * Main
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Main program for using SFML to display the project
 * 
 * Revision History:
 *      05DEC2021  R-12-05: Document Created, initial coding
 *      06DEC2021  R-12-06: Full debugging and successful SFML setup
 *      07DEC2021  R-12-07: Debugging and code cleanup
 * 
 **/

#include "code/debugSetup.h"
#include <SFML/Graphics.hpp>
#include <cmath>

#include "code/intersect4wsl.h"
#include "code/autoTrafficController.h"
#include "code/lightTrafficController.h"
#include "code/stopTrafficController.h"


// Window Dimension Constants
#define WINDOW_XDIM 1920
#define WINDOW_YDIM 1080

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

// Fonts
sf::Font thinFont;
sf::Font regFont;
sf::Font boldFont;

// Global Variables
Intersect4WSL* theIntersection;
TrafficController* theTrafficController;
sf::Texture textureCollection[8];
std::vector<Vehicle*> vehicleCollection;
std::vector<sf::Sprite*> spriteCollection;

/**
 * Coord
 * Description:
 *          Data structure for holding cartesian coordinate information
 * Contains:
 *      double x - X coordinate of an object
 *      double y - Y coordinate of an object
 *      double r - Rotation angle of an object in degrees
 **/
struct Coord
{
    double x;
    double y;
    double r;
};

// Forward Defined Functions
void cleanup();
void vehicleSpawner();
Coord getPos(std::string lane_id, double t, double s = 1);
Coord getPosRight(unsigned int lane, double t, double s = 1);
Coord getPosStraight(unsigned int lane, double t, double s = 1);
Coord getPosLeft(unsigned int lane, double t, double s = 1);

using namespace sf;

/**
 * Main
 * Inputs: Command Line Arguments
 * Description:
 *          Creates SFML Window to display and run simulation
 **/
int main(int argc, char *argv[])
{
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
        // Help Support
        if (typeArg == "--help")
        {
            std::cout << "\nTraffic Control Simulator - Command Line Interface Instructions\n\n"
                      << "Command line input format:\n"
                      << "./trafficSim [-T] [-R] [-P]\n"
                      << "Note that argument [-R] must come after a [-T] argument and a [-P] argument must come after both.\n\n"
                      << "Arguments:\n"
                      << "           [-T]: Traffic Control Simulation Type. Options are:\n"
                      << "                 -A    Autonomous Simulation\n"
                      << "                 -L    Traffic Light Controlled Simulation (Coming Soon!)\n"
                      << "                 -S    Stop Sign Controlled Simulation\n"
                      << "           [-R]: Tick Rate in microseconds. Input a positive integer.\n"
                      << "           [-P]: Spawn Probability as a percent. Input a positive integer between 1 and 100.\n"
                      << "With no arguments, the default operation is autonomous with tick speed 100ms and probability of 25\n\n";
            return 0;
        }
        // Check Number of Characters
        if (typeArg.length() != 2)
        {
            std::cerr << "Invalid Command Line Input!\n"
                      << "Expected arg in form of -C received " << typeArg << " instead.\n"
                      << "Run --help for more info.\n"
                      << "Quitting...\n";
            return 1;
        }
        // Controller Type Flag
        if (typeArg[0] != '-')
        {
            std::cerr << "Invalid Command Line Input!\n"
                      << "Expected arg to start with a '-' received '" << typeArg[0] << "' instead.\n"
                      << "Run --help for more info.\n"
                      << "Quitting...\n";
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
                std::cerr << "Light Controlled Traffic Coming Soon!\n"
                          << "Run --help to see other options.\n"
                          << "Quitting...\n";
                return 1;
                // controllerType = LIGHT; // Coming Soon!
                break;
            case 'S'-'A':
                controllerType = STOP;
                break;
            default:
                std::cerr << "Invalid Command Line Input!\n"
                          << "Received " << typeArg[1] << std::endl
                          << "Run --help for more info.\n"
                          << "Quitting...\n";
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
                          << "Found non-digit" << typeArg[i] << std::endl
                          << "Run --help for more info.\n"
                          << "Quitting...\n";
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
                          << "Found non-digit" << typeArg[i] << std::endl
                          << "Run --help for more info.\n"
                          << "Quitting...\n";
                return 1;
            }
        }
        probability = stoi(typeArg);
        if (probability == 0 || probability > 100)
        {
            std::cerr << "Invalid Command Line Input!\n"
                        << "Invalid probability of " << typeArg << std::endl
                        << "Run --help for more info.\n"
                        << "Quitting...\n";
            return 1;
        }
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
     * SFML Setup
     **/
    // Create and Open a Window
    VideoMode vm(WINDOW_XDIM, WINDOW_YDIM);
    RenderWindow window(vm, "Traffic Simulator", Style::Default);

    // Load Fonts
    thinFont.loadFromFile("fonts/HalisR-Light.otf");
    regFont.loadFromFile("fonts/HalisR-Medium.otf");
    boldFont.loadFromFile("fonts/HalisR-Regular.otf");

    // Create and Initialize Text Objects
    Text titleText("Traffic Simulator", regFont, 100);
    titleText.setFillColor(Color::White);
    titleText.setPosition(50,50);
    Text descText("A Simulation by Marcus Chan and Raymond Jia", thinFont, 20);
    descText.setFillColor(Color::White);
    descText.setPosition(50,200);
    Text exitText("Hold down ESC key to exit simulation", thinFont, 24);
    exitText.setFillColor(Color::White);
    exitText.setPosition(50, 900);
    Text countText("Vehicles Processed: ", thinFont, 24);
    countText.setFillColor(Color::White);
    countText.setPosition(1600, 60);
    Text averageWaitText("Average Wait: ", thinFont, 24);
    averageWaitText.setFillColor(Color::White);
    averageWaitText.setPosition(1600, 90);

    // Create and Initialize Background
    Texture textureBackground;
    std::string intersectionType = controllerType == STOP ? "Stop" : "";
    textureBackground.loadFromFile("graphics/intersection" + intersectionType + ".png");
    Sprite spriteBackground;
    spriteBackground.setTexture(textureBackground);
    spriteBackground.setPosition(0,0);

    // Create and Initialize Vehicle Textures
    for (int i=0; i<7; ++i)
    {
        Texture textureVehicle;
        textureVehicle.loadFromFile("graphics/vehicle" + std::to_string(i) + ".png");
        textureCollection[i] = textureVehicle;
    }
    // Also add a Cow texture
    // (The Cow is Spherical, we can avoid weird sizing issues for now)
    Texture textureCow;
    textureCow.loadFromFile("graphics/cow.png");
    textureCollection[7] = textureCow;

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

    /**
     * Statistics Trackers Setup
     **/
    unsigned long int vehiclesProcessed = 0;
    double averageWait = 0;

    /**
     * Draw Start Screen
     **/
    window.clear();
    window.draw(spriteBackground);
    window.draw(titleText);
    window.display();

    // Enter Display Loop
    while (window.isOpen())
    {
        /**
         * Check if user presses escape key
         */
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            // Close window and quit simulation
            window.close();
            continue;
        }

        // Spawn a Vehicle
        vehicleSpawner();

        /**
         * Draw everything
         **/
        // Clear Window
        window.clear();

        // Draw Background
        window.draw(spriteBackground);

        // Draw Vehicles
        // Protect Shared Data
        theTrafficController->protectControlledPods.lock();
        // Loop through vehicle collection and display or remove
        for (int i=0; i<vehicleCollection.size(); ++i)
        {
            Vehicle* thisVehicle = vehicleCollection[i];
            // Remove vehicle if exited
            if (thisVehicle->isExited())
            {
                vehiclesProcessed++;
                double waitTime = controllerType == STOP ? thisVehicle->getWaitTime()-5 : thisVehicle->getWaitTime();
                averageWait = (averageWait * ((double)vehiclesProcessed-1) + waitTime) / (double) vehiclesProcessed;
                delete thisVehicle;
                delete spriteCollection[i];
                vehicleCollection.erase(vehicleCollection.begin() + i);
                spriteCollection.erase(spriteCollection.begin() + i);
                --i;
                continue;
            }
            // Check if vehicle is not in traffic controller control yet
            Pod* podPtr = (Pod*)(thisVehicle->getPod());
            if (podPtr == NULL)
            {
                continue;
            }
            // Get vehicle coordinates
            Coord pos = getPos(podPtr->getLane()->getLaneID(), podPtr->getPosition());
            // Scale vehicle coordinates
            pos.x = WINDOW_XDIM/2 + pos.x * scale;
            pos.y = WINDOW_YDIM/2 + pos.y * scale;
            // Check if vehicle coordinates outside of screen
            if (pos.x < 0 || pos.x > WINDOW_XDIM || pos.y < 0 || pos.y > WINDOW_YDIM)
            {
                continue;
            }
            // Draw the vehicle
            Sprite* vehicleSprite = spriteCollection[i];
            vehicleSprite->setPosition(pos.x, pos.y);
            vehicleSprite->setRotation(pos.r);
            window.draw(*vehicleSprite);
        }
        // Unlock Shared Data
        theTrafficController->protectControlledPods.unlock();

        // Draw Text
        window.draw(titleText);
        window.draw(descText);
        window.draw(exitText);
        countText.setString("Vehicles Processed: " + std::to_string(vehiclesProcessed));
        window.draw(countText);
        averageWaitText.setString("Average Wait: " + std::to_string(averageWait));
        window.draw(averageWaitText);

        // Display Window
        window.display();

        // Update rate at approx tickSpeed microseconds between updates
        std::this_thread::sleep_for(std::chrono::microseconds(tickSpeed));
    }

    // Perform Cleanup
    cleanup();
    return 0;
}

/**
 * cleanup
 * Inputs: None
 * Outputs: None
 * Description:
 *          Stops traffic controller threads.
 *          Frees all allocated memory.
 **/
void cleanup()
{
    theTrafficController->stopController();
    delete theTrafficController;
    delete theIntersection;
    for (int i=0; i<vehicleCollection.size(); ++i)
    {
        delete vehicleCollection[i];
        delete spriteCollection[i];
    }
    vehicleCollection.clear();
    spriteCollection.clear();
}

/**
 * vehicleSpawner
 * Inputs: None
 * Outputs: None
 * Description:
 *          Creates and allocates memory for Vehicle and Sprite objects
 *          Pushes created vehicles to the Traffic Controller's Entry Queue
 **/
void vehicleSpawner()
{
    // Spawn with given probability
    if (rand() % 100 > probability)
    {
        return;
    }

    // Create random vehicle ID
    std::string randomizedID = "";
    for (int i=0; i<10; ++i)
    {
        randomizedID += std::to_string(rand() % 10);
    }

    // Choose random source and destination
    unsigned int src = rand() % theIntersection->getNumNodes();
    unsigned int dest = rand() % theIntersection->getNumNodes();
    if (src == dest)
    {
        return;
    }

    // Create vehicle, allocate memory
    Vehicle* newVehicle = new Vehicle(randomizedID, 10, 10, 1, theIntersection->getNode(std::to_string(src)), theIntersection->getNode(std::to_string(dest)));
    vehicleCollection.push_back(newVehicle);
    // Choose a sprite (Just Cows for now)
    unsigned int carType = 7;
    // Create sprite, allocate memory
    Sprite* spriteVehicle = new Sprite;
    spriteVehicle->setTexture(textureCollection[7]);
    spriteVehicle->setScale(Vector2f(0.5f, 0.5f));
    spriteVehicle->setOrigin(30, 30);
    spriteCollection.push_back(spriteVehicle);

    // Push vehicle to traffic controller's entry queue
    theTrafficController->entryQueue.push(newVehicle);
}

/**
 * getPos
 * Inputs:
 *      std::string lane_id - Lane ID of the vehicle
 *      double t - Linear position of the vehicle
 *      double s (optional) - Scale factor, by default 1 (No Scaling)
 * Outputs:
 *      Coord - Cartesian coordinates and rotation of the vehicle
 * Description:
 *          Takes in vehicle lane and linear position information
 *          and translates it to cartestian coordinates for screen display,
 *          relative to the middle of the intersection. (0,0) is the assumed
 *          position of the center of the intersection. Applied to 4WSL intersection
 *          type only.
 **/
Coord getPos(std::string lane_id, double t, double s)
{
    // Coord for return
    Coord position;
    // Initialize
    position.x = 0;
    position.y = 0;
    position.r = 0;

    // Choose function dependent on lane_id
    // Right turn
    if (lane_id == "0-1" || lane_id == "1-2" || lane_id == "2-3" || lane_id == "3-0")
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosRight(src, t, s);
    }
    // Straight
    else if (lane_id == "0-2" || lane_id == "1-3" || lane_id == "2-0" || lane_id == "3-1")
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosStraight(src, t, s);
    }
    // Left turn
    else if (lane_id == "0-3" || lane_id == "1-0" || lane_id == "2-1" || lane_id == "3-2")
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosLeft(src, t, s);
    }
    // ???? What ungodly lane is this?
    else
    {
        std::cerr << "Something went wrong!\n"
                  << "Error in getPos(std::string, double) in sfmlDisplay.cpp\n";
    }

    return position;
}

/**
 * getPosRight
 * Inputs:
 *      unsigned int lane - Numerical ID of source node
 *      double t - Linear position along lane
 *      double s (optional) - Scale factor, by default 1 (No Scaling)
 * Outputs:
 *      Coord - Cartesian coordinates and rotation of the vehicle
 * Description:
 *          Takes in right turn lane source and linear position information
 *          and translates it to cartestian coordinates for screen display,
 *          relative to the middle of the intersection. (0,0) is the assumed
 *          position of the center of the intersection. Applied to 4WSL intersection
 *          type only.
 **/
Coord getPosRight(unsigned int lane, double t, double s)
{
    Coord position;

    if (t < 20)
    {
        position.x = -1 * (20-t) * s;
        position.y = -1 * 2;
    }
    else if (t < 30)
    {
        position.x = 2 * sin((t-20)*M_PI/20);
        position.y = -1 * 2 * cos((t-20)*M_PI/20);
    }
    else
    {
        position.x = 2;
        position.y = (t-30) * s;
    }

    double tmp;
    switch(lane)
    {
        case 0:
            position.x -= 3;
            position.y += 3;
            position.r = 0;
            break;
        case 1:
            position.x *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x += 3;
            position.y += 3;
            position.r = -90;
            break;
        case 2:
            position.x *= -1;
            position.y *= -1;
            position.x += 3;
            position.y -= 3;
            position.r = 180;
            break;
        case 3:
            position.y *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x -= 3;
            position.y -= 3;
            position.r = 90;
            break;
        default:
            std::cerr << "something went wrong!\n";
            break;
    }
    
    position.r += 90;

    if (t >= 30)
    {
        position.r += 90;
        if (position.r > 180)
        {
            position.r -= 360;
        }
    }
    else if (t >= 20)
    {
        // Rotation for curve not yet implemented. Math is hard :(
    }

    return position;
}

/**
 * getPosStraight
 * Inputs:
 *      unsigned int lane - Numerical ID of source node
 *      double t - Linear position along lane
 *      double s (optional) - Scale factor, by default 1 (No Scaling)
 * Outputs:
 *      Coord - Cartesian coordinates and rotation of the vehicle
 * Description:
 *          Takes in straight lane source and linear position information
 *          and translates it to cartestian coordinates for screen display,
 *          relative to the middle of the intersection. (0,0) is the assumed
 *          position of the center of the intersection. Applied to 4WSL intersection
 *          type only.
 **/
Coord getPosStraight(unsigned int lane, double t, double s)
{
    Coord position;

    double a;
    if (t < 20)
    {
        a = (t-23)*s;
    }
    else if (t < 30)
    {
        a = (t-20)*3/5 - 3;
    }
    else
    {
        a = (t-27)*s;
    }

    double b = 1;

    switch (lane)
    {
        case 0:
            position.x = a;
            position.y = b;
            position.r = 90;
            break;
        case 2:
            position.x = -1*a;
            position.y = -1*b;
            position.r = -90;
            break;
        case 1:
            position.x = b;
            position.y = -1*a;
            position.r = 0;
            break;
        case 3:
            position.x = -1*b;
            position.y = a;
            position.r = 180;
            break;
        default:
            std::cerr << "something went wrong!\n";
            break;
    }

    return position;
}

/**
 * getPosLeft
 * Inputs:
 *      unsigned int lane - Numerical ID of source node
 *      double t - Linear position along lane
 *      double s (optional) - Scale factor, by default 1 (No Scaling)
 * Outputs:
 *      Coord - Cartesian coordinates and rotation of the vehicle
 * Description:
 *          Takes in left turn lane source and linear position information
 *          and translates it to cartestian coordinates for screen display,
 *          relative to the middle of the intersection. (0,0) is the assumed
 *          position of the center of the intersection. Applied to 4WSL intersection
 *          type only.
 **/
Coord getPosLeft(unsigned int lane, double t, double s)
{
    Coord position;

    if (t < 20)
    {
        position.x = -1 * (20-t) * s;
        position.y = 4;
    }
    else if (t < 30)
    {
        position.x = 4 * sin((t-20)*M_PI/20);
        position.y = 4 * cos((t-20)*M_PI/20);
    }
    else
    {
        position.x = 4;
        position.y = (30-t) * s;
    }

    double tmp;
    switch(lane)
    {
        case 0:
            position.x -= 3;
            position.y -= 3;
            position.r = 90;
            break;
        case 1:
            position.x *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x -= 3;
            position.y += 3;
            position.r = 0;
            break;
        case 2:
            position.x *= -1;
            position.y *= -1;
            position.x += 3;
            position.y += 3;
            position.r = -90;
            break;
        case 3:
            position.y *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x += 3;
            position.y -= 3;
            position.r = 180;
            break;
        default:
            std::cerr << "something went wrong!\n";
            break;
    }

    if (t >= 30)
    {
        position.r -= 90;
        if (position.r < -180)
        {
            position.r += 360;
        }
    }
    else if (t >= 20)
    {
        // Rotation for curve not yet implemented. Math is hard :(
    }

    return position;
}