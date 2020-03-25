#ifndef CAMERA_H
#define CAMERA_H

#include <QVector3D>

class Camera {
    QVector3D position = QVector3D(0,0,0);
    float near_plane = 0.2f, far_plane = 100000.0f, fov = 60.0f;
public:
    Camera() {}

    QVector3D getPosition() {return position;}
    void setPosition(QVector3D pos) {position = pos;}
    float getNearPlane() {return near_plane;}
    float getFarPlane() {return far_plane;}
    float getFOV() {return fov;}
    void setFOV(float f) {fov = f;}
};

#endif // CAMERA_H
