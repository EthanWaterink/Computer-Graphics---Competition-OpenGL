#include "solarsystem.h"
#include <QDebug>

#define rScale 10
#define rotScale 10
#define dfoScale 2500
#define orbScale 5

SolarSystem::SolarSystem()
{
    objects.reserve(12);

    Sun *eye = new Sun("Eye", 0, 0);
    eye->setLocation(QVector3D(0.1f,8000,0.1f));
    Sun *eye2 = new Sun("Eye 2", 0, 0);
    eye2->setLocation(QVector3D(0.1f,2200,0.1f));
    objects.push_back(eye);
    objects.push_back(eye2);

    Sun *sun = new Sun("Sun", rScale*200, 0.001f);
    objects.push_back(sun);
    // Data from https://nssdc.gsfc.nasa.gov/planetary/factsheet/planet_table_ratio.html
    Planet *mercury = new Planet("Mercury", ":/textures/mercury.jpg",   rScale*0.338f,  rotScale*58.f,      dfoScale*0.387f,    orbScale*0.241f,    sun);
    Planet *venus = new Planet("Venus", ":/textures/venus.jpg",         rScale*0.949f,  rotScale*-244.0f,   dfoScale*0.723f,    orbScale*0.615f,    sun);
    Planet *earth = new Planet("Earth", ":/textures/earth2.jpg",        rScale*1.0f,    rotScale*1.0f,      dfoScale*1.0f,      orbScale*1.0f,      sun);
    Planet *moon = new Planet("Moon Earth", ":/textures/moon.jpg",      rScale*0.2724f, rotScale*27.4f,     dfoScale*0.00257f,  orbScale*0.0748f,   earth);
    Planet *mars =  new Planet("Mars", ":/textures/mars.jpg",           rScale*0.532f,  rotScale*1.03f,     dfoScale*1.52f,     orbScale*1.88f,     sun);
    Planet *jupiter = new Planet("Jupiter", ":/textures/jupiter.jpg",   rScale*11.21f,  rotScale*0.415f,    dfoScale*5.20f,     orbScale*11.9f,     sun);
    Planet *saturn = new Planet("Saturn", ":/textures/saturn.jpg",      rScale*9.45f,   rotScale*0.445f,    dfoScale*9.58f,     orbScale*29.4f,     sun);
    Planet *uranus = new Planet("Uranus", ":/textures/uranus.jpg",      rScale*4.01f,   rotScale*-0.72f,    dfoScale*19.20f,    orbScale*163.7f,    sun);
    Planet *neptune = new Planet("Neptune", ":/textures/neptune.jpg",   rScale*3.88f,   rotScale*0.673f,    dfoScale*30.05f,    orbScale*247.9f,    sun);
    planets.push_back(mercury);
    planets.push_back(venus);
    planets.push_back(earth);
    planets.push_back(moon);
    planets.push_back(mars);
    planets.push_back(jupiter);
    planets.push_back(saturn);
    planets.push_back(uranus);
    planets.push_back(neptune);

    Spaceship *spaceship1 = new Spaceship("Apollo 13", ":/textures/cat_diff.png", 8.0f, earth, randomPlanet());
    Spaceship *spaceship2 = new Spaceship("Spaceshuttle", ":/textures/cat_diff.png", 9.0f, earth, randomPlanet());
    spaceships.push_back(spaceship1);
    spaceships.push_back(spaceship2);

    for (Planet *p : planets) {
        objects.push_back(p);
    }
    for (Spaceship *s : spaceships) {
        objects.push_back(s);
    }
}

Planet *SolarSystem::randomPlanet () {
    return planets[qrand() % planets.size()];
}

void SolarSystem::simulate(float t, float s) {
    for (Object *o : objects) {
        o->update(t, s);
    }
    for (Spaceship *s : spaceships) {
        if (s->hasReachedDestination()) {
            qDebug() << "Spaceship" << s->getName() << "reached planet" << s->getDestination()->getName();
            s->setMoveTo(randomPlanet());
            qDebug() << "Spaceship" << s->getName() << "new destination: " << s->getDestination()->getName();
        }
    }
}
