#ifndef MAINVIEW_H
#define MAINVIEW_H

#include "model.h"
#include "object.h"
#include "camera.h"
#include "solarsystem.h"

#include <QImage>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QTimer>
#include <QVector>
#include <QVector3D>
#include <QComboBox>
#include <QtMath>

#include <memory>

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    QOpenGLDebugLogger debugLogger;
    QTimer timer; // Timer used for animation.

    QOpenGLShaderProgram phongShaderProgram;

    // Uniforms for the Phong shader program.
    GLint uniformModelTransformPhong;
    GLint uniformViewTransformPhong;
    GLint uniformProjectionTransformPhong;
    GLint uniformNormalTransformPhong;

    GLint uniformMaterialPhong;
    GLint uniformLightPositionPhong;
    GLint uniformLightColorPhong;
    GLint uniformCameraPosition;

    GLint uniformTextureSamplerPhong;

    SolarSystem solarSystem;

    // Transforms
    QMatrix4x4 projectionTransform;
    QMatrix4x4 viewTransform;
    QVector3D rotation = QVector3D();

    Camera camera;
    float angle = 0, radius = 1.0f;

    float time = 0;
    float timeStep = 0.016f/10.0f;
    float speed = 1.0f;

    // Phong model constants.
    QVector4D material = {0.15F, 1.0F, 0.2F, 1.0F};
    QVector3D lightPosition = {0.0F, 0.0F, 0.0F};
    QVector3D lightColor = {1.0F, 1.0F, 1.0F};

public:
    enum ShadingMode : GLuint
    {
        PHONG = 0, NORMAL, GOURAUD
    };

    MainView(QWidget *parent = 0);
    ~MainView();

    QComboBox *comboBox_lookingFrom;
    QComboBox *comboBox_lookingAt;

    // Functions for widget input events.
    void setRotation(int rotateX, int rotateY, int rotateZ);
    void setShadingMode(ShadingMode shading);
    SolarSystem *getSolarSystem() {return &solarSystem;}
    void setHeight(float r) {radius = r;}
    void setAngle(float a) {angle = a;}
    void setSpeed(float s) {speed = s;}
    void setCameraFOV(float fov);

protected:
    void initializeGL();
    void resizeGL(int newWidth, int newHeight);
    void paintGL();

    // Functions for keyboard input events
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

    // Function for mouse input events
    void mouseDoubleClickEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

private slots:
    void onMessageLogged( QOpenGLDebugMessage Message );

private:
    void createShaderProgram();
    void loadObjects();
    void fillComboBoxes(SolarSystem *ss);

    void destroyObjects();

    void updateProjectionTransform();
    void updateViewTransform();
    void updateModelTransforms();
    void updateModelTransform(Object *obj);

    void updatePhongUniforms();

    void paintObject (Object *obj);
    void paintSolarSystem (SolarSystem *ss);
    void calculateCameraPosition();

    // The current shader to use.
    ShadingMode currentShader = PHONG;
};

#endif // MAINVIEW_H
