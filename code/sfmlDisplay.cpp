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
 *      05DEC2021  R-11-30: Document Created, initial coding
 * 
 **/

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include "intersect4wsl.h"
#include "autoTrafficController.h"
#include "lightTrafficController.h"
#include "stopTrafficController.h"

// Window Dimension Constants
#define WINDOW_XDIM 1920
#define WINDOW_YDIM 1080

// Controller Type
#define AUTO    0
#define LIGHT   1
#define STOP    2
unsigned int controllerType;

// Default Speed Limit
#define DEFAULT_SPEED_LIMIT 4

// Fonts
sf::Font thinFont;
sf::Font regFont;
sf::Font boldFont;

// Global Variables
Intersect4WSL* theIntersection;
TrafficController* theTrafficController;
std::map<std::string, Vehicle*> VehicleCollection;

struct Coord
{
    double x;
    double y;
    double r;
};

void cleanup();
Coord getPos(std::string lane_id, double t);
Coord getPosRight(unsigned int lane, double t);
Coord getPosStraight(unsigned int lane, double t);
Coord getPosLeft(unsigned int lane, double t);

using namespace sf;

/**
 * Main
 **/
int main(int argc, char *argv[])
{
    // Command Line Argument Detection
    if (argc == 1)
    {
        controllerType = 0;
    }
    if (argc == 2)
    {
        std::string typeArg = argv[1];
        char c = typeArg[1];
        int i_c = c - 'A';
        switch (i_c)
        {
            case 'A'-'A':
                controllerType = AUTO;
                break;
            case 'L'-'A':
                controllerType = LIGHT;
                break;
            case 'S'-'A':
                controllerType = STOP;
                break;
            default:
                std::cerr << "Something went wrong\n";
                return 1;
                break;
        }
    }

    // Create and Open a Window
    VideoMode vm(WINDOW_XDIM, WINDOW_YDIM);
    RenderWindow window(vm, "Traffic Simulator", Style::Default);

    // Load Fonts
    thinFont.loadFromFile("fonts/HalisR-Light.otf");
    regFont.loadFromFile("fonts/HalisR-Medium.otf");
    boldFont.loadFromFile("fonts/HalisR-Regular.otf");

    // Text Objects
    Text titleText("Traffic Simulator", regFont, 100);
    titleText.setFillColor(Color::White);
    FloatRect titleTextRect = titleText.getLocalBounds();
    titleText.setOrigin(titleTextRect.left +
        titleTextRect.width / 2.0f,
        titleTextRect.top +
        titleTextRect.height / 2.0f);
    titleText.setPosition(WINDOW_XDIM / 4.0f, WINDOW_YDIM / 6.0f);

    // Create and Initialize Background Sprite
    Texture textureBackground;
    textureBackground.loadFromFile("graphics/background.png");
    Sprite spriteBackground;
    spriteBackground.setTexture(textureBackground);
    spriteBackground.setPosition(0,0);

    // Create and Initialize Vehicle Sprites
    Texture textureVehicleA;
    textureVehicleA.loadFromFile("graphics/vehicleA.png");
    Sprite spriteVehicleA;
    spriteVehicleA.setTexture(textureVehicleA);
    spriteVehicleA.setPosition(40,40);
    Texture textureVehicleB;
    textureVehicleB.loadFromFile("graphics/vehicleB.png");
    Sprite spriteVehicleB;
    spriteVehicleB.setTexture(textureVehicleB);
    spriteVehicleB.setPosition(160,40);
    Texture textureVehicleC;
    textureVehicleC.loadFromFile("graphics/vehicleC.png");
    Sprite spriteVehicleC;
    spriteVehicleC.setTexture(textureVehicleC);
    spriteVehicleC.setPosition(160,40);
    Texture textureVehicleD;
    textureVehicleD.loadFromFile("graphics/vehicleD.png");
    Sprite spriteVehicleD;
    spriteVehicleD.setTexture(textureVehicleD);
    spriteVehicleD.setPosition(160,40);

    // Adjust Scaling
    spriteVehicleA.setScale(Vector2f(0.3f, 0.3f));
    spriteVehicleB.setScale(Vector2f(0.3f, 0.3f));
    spriteVehicleC.setScale(Vector2f(0.3f, 0.3f));
    spriteVehicleD.setScale(Vector2f(0.3f, 0.3f));

    // Set Origin
    spriteVehicleA.setOrigin(30, 59);
    spriteVehicleB.setOrigin(30, 59);
    spriteVehicleC.setOrigin(30, 59);
    spriteVehicleD.setOrigin(30, 59);

    // Setup Environment
    theIntersection = new Intersect4WSL(DEFAULT_SPEED_LIMIT);
    switch (controllerType)
    {
        case AUTO:
            theTrafficController = new AutoTrafficController(theIntersection);
            break;
        case LIGHT:
            theTrafficController = new LightTrafficController(theIntersection);
            break;
        case STOP:
            theTrafficController = new StopTrafficController(theIntersection);
            break;
        default:
            theTrafficController = new AutoTrafficController(theIntersection);
            break;
    }
    theTrafficController->startController();
    if (controllerType == LIGHT)
    {
        LightTrafficController* control = dynamic_cast<LightTrafficController*>(theTrafficController);
        control->startLightCycle();
    }

    /**
     * Draw Start Screen
     **/
    window.clear();
    window.draw(spriteBackground);
    window.draw(titleText);
    window.display();

    double t = 0;
    double s = 8;

    // Enter Display Loop
    while (window.isOpen())
    {
        /**
         * Check if user presses escape key
         */
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            // Close window and quit game if user presses escape key
            window.close();
        }

        /**
         * Draw everything
         **/
        // Clear Window
        window.clear();
        // Draw Background
        window.draw(spriteBackground);
        Coord posA = getPos("0-1", t);
        Coord posB = getPos("0-2", t);
        Coord posC = getPos("0-3", t);
        Coord posD = getPos("2-0", t);
        spriteVehicleA.setPosition(WINDOW_XDIM/2 + posA.x*s, WINDOW_YDIM/2 + posA.y*s);
        spriteVehicleB.setPosition(WINDOW_XDIM/2 + posB.x*s, WINDOW_YDIM/2 + posB.y*s);
        spriteVehicleC.setPosition(WINDOW_XDIM/2 + posC.x*s, WINDOW_YDIM/2 + posC.y*s);
        spriteVehicleD.setPosition(WINDOW_XDIM/2 + posD.x*s, WINDOW_YDIM/2 + posD.y*s);
        spriteVehicleA.setRotation(posA.r);
        spriteVehicleB.setRotation(posB.r);
        spriteVehicleC.setRotation(posC.r);
        spriteVehicleD.setRotation(posD.r);
        window.draw(spriteVehicleA);
        window.draw(spriteVehicleB);
        window.draw(spriteVehicleC);
        window.draw(spriteVehicleD);

        // Draw Text
        window.draw(titleText);
        // Display Window
        window.display();

        t += 0.1;
        if (t > 100) {t = 0;}
    }

    // Cleanup
    cleanup();
    return 0;
}

void cleanup()
{
    theTrafficController->stopController();
    delete theTrafficController;
    delete theIntersection;
}

void vehicleSpawner()
{
    // Spawn one vehicle
    std::string randomizedID;
    unsigned int src;
    unsigned int dest;
    Vehicle* newVehicle = new Vehicle(randomizedID, 10, 10, 1, theIntersection->getNode(std::to_string(src)), theIntersection->getNode(std::to_string(dest)));
    VehicleCollection.insert({newVehicle->getVehicleID(), newVehicle});
}

// Gives offset from center of intersection
Coord getPos(std::string lane_id, double t)
{
    Coord position;

    if (lane_id == "0-1" || lane_id == "1-2" || lane_id == "2-3" || lane_id == "3-0")
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosRight(src, t);
    }
    else if (lane_id == "0-2" || lane_id == "1-3" || lane_id == "2-0" || lane_id == "3-1")
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosStraight(src, t);
    }
    else
    {
        unsigned int src = lane_id[0]-'0';
        position = getPosLeft(src, t);
    }

    return position;
}

// Right Turn Position Calculator
Coord getPosRight(unsigned int lane, double t)
{
    Coord position;

    if (t < 40)
    {
        position.x = -1 * (40-t);
        position.y = -1 * 7;
    }
    else if (t <= 60)
    {
        position.x = 7 * sin((t-40)*M_PI/40);
        position.y = -1 * 7 * cos((t-40)*M_PI/40);
    }
    else
    {
        position.x = 7;
        position.y = t-60;
    }

    double tmp;
    switch(lane)
    {
        case 0:
            position.x -= 10;
            position.y += 10;
            position.r = 0;
            break;
        case 1:
            position.x *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x += 10;
            position.y += 10;
            position.r = -90;
            break;
        case 2:
            position.x *= -1;
            position.y *= -1;
            position.x += 10;
            position.y -= 10;
            position.r = 180;
            break;
        case 3:
            position.y *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x -= 10;
            position.y -= 10;
            position.r = 90;
            break;
        default:
            std::cerr << "something went wrong!\n";
            break;
    }
    
    position.r += 90;

    if (t > 60)
    {
        position.r += 90;
        if (position.r > 180)
        {
            position.r -= 360;
        }
    }
    else if (t >= 40)
    {
        std::cout << "shiet\n";
    }

    return position;
}

// Straight Position Calculator
Coord getPosStraight(unsigned int lane, double t)
{
    Coord position;

    double a = t-50;
    double b = 3;

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

// Left Position Calculator
Coord getPosLeft(unsigned int lane, double t)
{
    Coord position;

    if (t < 40)
    {
        position.x = -1 * (40-t);
        position.y = 13;
    }
    else if (t <= 60)
    {
        position.x = 13 * sin((t-40)*M_PI/40);
        position.y = 13 * cos((t-40)*M_PI/40);
    }
    else
    {
        position.x = 13;
        position.y = 60-t;
    }

    double tmp;
    switch(lane)
    {
        case 0:
            position.x -= 10;
            position.y -= 10;
            position.r = 90;
            break;
        case 1:
            position.x *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x -= 10;
            position.y += 10;
            position.r = 0;
            break;
        case 2:
            position.x *= -1;
            position.y *= -1;
            position.x += 10;
            position.y += 10;
            position.r = -90;
            break;
        case 3:
            position.y *= -1;
            tmp = position.x;
            position.x = position.y;
            position.y = tmp;
            position.x += 10;
            position.y -= 10;
            position.r = 180;
            break;
        default:
            std::cerr << "something went wrong!\n";
            break;
    }

    if (t > 60)
    {
        position.r -= 90;
        if (position.r < -180)
        {
            position.r += 360;
        }
    }
    else if (t >= 40)
    {
        std::cout << "shiet\n";
    }

    return position;
}