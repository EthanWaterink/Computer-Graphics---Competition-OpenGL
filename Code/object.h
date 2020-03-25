#ifndef OBJECT_H
#define OBJECT_H

#include <QOpenGLFunctions_3_3_Core>
#include <QVector3D>
#include <QImage>
#include <QVector>
#include <QMatrix4x4>

#include "model.h"

class Object : protected QOpenGLFunctions_3_3_Core {
    // Buffers
    GLuint meshVAO;
    GLuint meshVBO;
    GLuint meshVIO;
    GLuint meshSize;
public:
    QMatrix4x4 meshTransform;
    QMatrix3x3 meshNormalTransform;

    Object(QString name, QString modelfile, QString texturefile);
    ~Object();
    void load();
    void draw ();

    QVector3D getLocation() {return location;}
    void setLocation (QVector3D loc) {location = loc;}
    QString getName() {return name;}
    float getAngle() {return angle;}
    float getScale() {return scale;}

    virtual void update(float t, float s) {Q_UNUSED(t) Q_UNUSED(s)}
    void rotate(float a);
    float distanceTo (Object *obj) {return (location - obj->getLocation()).length();}
protected:
    QVector3D location;
    QString texture;
    Model model;
    virtual QVector<float> getMeshData();
    float scale = 1.0f;
private:
    // Texture
    GLuint textureDiff;

    QString name;
    float angle = 0;

    void genBuffers ();
    void loadTexture (QString file, GLuint textureName);
    void loadTextures ();
    void loadMesh ();
    void delBuffers();

    // Useful utility method to convert image to bytes.
    QVector<quint8> imageToBytes(QImage image);
};

class Sphere : public Object {
public:
    Sphere(QString n, QString texturefile, float r, float rotP) : Object{n, ":/models/sphere.obj", texturefile}{
        scale = r;
        rotationPeriod = rotP;
    }
    float getRotationPeriod(){return rotationPeriod;}
protected:
    float rotationPeriod;
};

class Sun : public Sphere {
public:
    Sun (QString n, float r, float rotP) : Sphere{n, ":/textures/sun.jpg", r, rotP} {
        location = QVector3D();
    }
    QVector<float> getMeshData() override;
    void update(float t, float s) override {rotate(s*t*rotationPeriod);}
};

class Planet : public Sphere {
public:
    Planet (QString n, QString texturefile, float r, float rotP, float df, float orbP, Sphere *rot) : Sphere{n, texturefile, r, rotP} {
        distanceFrom = df + r + rot->getScale();
        orbitalPeriod = orbP;
        location = QVector3D(0,0,-distanceFrom);
        rotateAround = rot;
    }
    float distanceFrom;
    float orbitalPeriod;
    Sphere *rotateAround;

    void moveAround(float t);
    void update(float t, float s) {Q_UNUSED(s) moveAround(t);}
};


class Spaceship : public Object {
    float speed = 0;
    Object *moveFrom;
    Object *moveTo;

public:
    Spaceship (QString n, QString texturefile, float s, Object *mf, Object *mt) : Object{n, ":/models/cat.obj", texturefile} {
        speed = s;
        moveFrom = mf;
        moveTo = mt;
        location = moveFrom->getLocation() + QVector3D(0, moveFrom->getScale(), 0);
        scale = 2.0f;
    }
    bool hasReachedDestination ();
    void update(float t, float s) override;
    void setMoveFrom (Object *mf) {moveFrom = mf;}
    void setMoveTo (Object *mt) {moveTo = mt;}
    Object *getDestination () {return moveTo;}
};

#endif // OBJECT_H
