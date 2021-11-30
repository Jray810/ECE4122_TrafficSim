/**
 * Calc
 * 
 * Authors: Marcus Chan, Raymond Jia
 * Class: ECE 4122 - Hurley
 * Final Project - Autonomous Traffic Simulator
 * 
 * Description:
 *      Provides calculation functions for mathematical calculations.
 * 
 * Revision History:
 *      14NOV2021  R-11-14: Document Created, initial coding
 * 
 **/

#ifndef CALC_H
#define CALC_H

#include <cmath>

double futurePos(double a, double v0, double x0, double t)
{
    return 0.5 * a * pow(t,2) + v0 * t + x0;
}

double futureVel(double a, double v0, double t)
{
    return a * t + v0;
}

double findTimeToPos(double a, double v0, double x0, double xf)
{
    double cof1 = 0.5 * a;
    double cof2 = v0;
    double cof3 = x0 - xf;

    double det = pow(cof2,2) - 4 * cof1 * cof3;

    double soln;

    if (det == 0)
    {
        soln = -1 * cof2 / (2 * cof1);
        if (soln >= 0)
        {
            return soln;
        }
        return -1;
    }

    if (det > 0)
    {
        soln = (-1 * cof2 + sqrt(det)) / (2 * cof1);
        if (soln < 0)
        {
            return soln * -1;
        }
        return soln;
    }

    return -1;
}

double findTimeToVel(double a, double v0, double vf)
{
    return (vf - v0) / a;
}

#endif