#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include "object.h"

class SolarSystem
{
public:
    SolarSystem();
    QVector<Object*> objects;
    QVector <Planet*> planets;
    QVector<Spaceship*> spaceships;

    void simulate (float t, float s);
private:
    Planet *randomPlanet();
};

#endif // SOLARSYSTEM_H
