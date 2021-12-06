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

void cleanup();

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
    titleText.setPosition(WINDOW_XDIM / 2.0f, WINDOW_YDIM / 5.0f);

    // Create and Initialize Background Sprite
    Texture texturebackground;
    texturebackground.loadFromFile("graphics/background.png");
    Sprite spriteBackground;
    spriteBackground.setTexture(texturebackground);
    spriteBackground.setPosition(0,0);

    // Create and Initialize Vehicle Sprites
    Texture textureVehicleA;
    textureVehicleA.loadFromFile("graphics/vehicleA.png");
    Sprite spriteVehicleA;
    spriteVehicleA.setTexture(textureVehicleA);
    spriteVehicleA.setPosition(160,40);
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

        // Draw Text
        window.draw(titleText);
        // Display Window
        window.display();
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