#include "mainview.h"
#include "model.h"
#include "object.h"

/**
 * @brief MainView::MainView
 *
 * Constructor of MainView
 *
 * @param parent
 */
MainView::MainView(QWidget *parent) : QOpenGLWidget(parent)/*, cat(":/models/cat.obj")*/ {
    qDebug() << "MainView constructor";

    connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

/**
 * @brief MainView::~MainView
 *
 * Destructor of MainView
 * This is the last function called, before exit of the program
 * Use this to clean up your variables, buffers etc.
 *
 */
MainView::~MainView() {
    qDebug() << "MainView destructor";

    makeCurrent();
}

// --- OpenGL initialization

void MainView::loadObjects() {
    for (Object *o : solarSystem.objects) {
        o->load();
    }
}

/**
 * @brief MainView::initializeGL
 *
 * Called upon OpenGL initialization
 * Attaches a debugger and calls other init functions
 */
void MainView::initializeGL() {
    qDebug() << ":: Initializing OpenGL";
    initializeOpenGLFunctions();

    connect(&debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)),
             this, SLOT(onMessageLogged(QOpenGLDebugMessage)), Qt::DirectConnection);

    if (debugLogger.initialize()) {
        qDebug() << ":: Logging initialized";
        debugLogger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    }

    QString glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0F, 0.0F, 0.0F, 0.0F);

    fillComboBoxes(&solarSystem);
    createShaderProgram();
    loadObjects ();

    // Initialize transformations.
    updateModelTransforms();
    updateViewTransform();
    updateProjectionTransform();

    timer.start(1000.0 / 60.0);
}

void MainView::fillComboBoxes (SolarSystem *ss) {
    for (Object *o : ss->objects) {
        comboBox_lookingFrom->addItem(o->getName());
        comboBox_lookingAt->addItem(o->getName());
    }
    comboBox_lookingFrom->setCurrentIndex(0);
    comboBox_lookingAt->setCurrentIndex(2);
}

void MainView::createShaderProgram() {
    // Create Phong shader program.
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Vertex,
                                           ":/shaders/vertshader_phong.glsl");
    phongShaderProgram.addShaderFromSourceFile(QOpenGLShader::Fragment,
                                           ":/shaders/fragshader_phong.glsl");
    phongShaderProgram.link();

    // Get the uniforms for the Phong shader program.
    uniformModelTransformPhong       = phongShaderProgram.uniformLocation("modelTransform");
    uniformViewTransformPhong        = phongShaderProgram.uniformLocation("viewTransform");
    uniformProjectionTransformPhong  = phongShaderProgram.uniformLocation("projectionTransform");
    uniformNormalTransformPhong      = phongShaderProgram.uniformLocation("normalTransform");
    uniformMaterialPhong             = phongShaderProgram.uniformLocation("material");
    uniformLightPositionPhong        = phongShaderProgram.uniformLocation("lightPosition");
    uniformLightColorPhong           = phongShaderProgram.uniformLocation("lightColor");
    uniformTextureSamplerPhong       = phongShaderProgram.uniformLocation("textureSampler");
    uniformCameraPosition            = phongShaderProgram.uniformLocation("cameraPosition");
}

// --- OpenGL drawing

/**
 * @brief MainView::paintGL
 *
 * Actual function used for drawing to the screen
 *
 */
void MainView::paintGL() {
    // Clear the screen before rendering
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    solarSystem.simulate(time, speed);
    calculateCameraPosition();

    updateModelTransforms();
    updateViewTransform();

    // Choose the selected shader.
    switch (currentShader) {
    case NORMAL:
        qDebug() << "Normal shader program not implemented";
        break;
    case GOURAUD:
        qDebug() << "Gouraud shader program not implemented";
        break;
    case PHONG:
        phongShaderProgram.bind();
        updatePhongUniforms();
        break;
    }

    paintSolarSystem(&solarSystem);

    phongShaderProgram.release();

    time += timeStep*speed;
}

void MainView::paintSolarSystem (SolarSystem *ss) {
    for (Object *o : ss->objects) {
        paintObject(o);
    }
}

void MainView::paintObject(Object *obj) {
    glUniformMatrix4fv(uniformModelTransformPhong, 1, GL_FALSE, obj->meshTransform.data());
    glUniformMatrix3fv(uniformNormalTransformPhong, 1, GL_FALSE, obj->meshNormalTransform.data());
    obj->draw();
}

void MainView::calculateCameraPosition() {
    Object *lookingFrom = solarSystem.objects[comboBox_lookingFrom->currentIndex()];
    QVector3D dir =
            solarSystem.objects[comboBox_lookingAt->currentIndex()]->getLocation() -
            lookingFrom->getLocation();
    if (dir == QVector3D()) dir = QVector3D (0,0,1);
    QVector3D pos = -dir.normalized() * radius * lookingFrom->getScale();
    QVector3D r = QVector3D::crossProduct(dir, QVector3D(0,1,0));
    QMatrix4x4 rot = QMatrix4x4();
    rot.rotate(angle, r);
    camera.setPosition(rot * pos + solarSystem.objects[comboBox_lookingFrom->currentIndex()]->getLocation());
}

/**
 * @brief MainView::resizeGL
 *
 * Called upon resizing of the screen
 *
 * @param newWidth
 * @param newHeight
 */
void MainView::resizeGL(int newWidth, int newHeight) {
    Q_UNUSED(newWidth)
    Q_UNUSED(newHeight)
    updateProjectionTransform();
}

void MainView::updatePhongUniforms() {
    glUniformMatrix4fv(uniformViewTransformPhong, 1, GL_FALSE, viewTransform.data());
    glUniformMatrix4fv(uniformProjectionTransformPhong, 1, GL_FALSE, projectionTransform.data());

    glUniform4fv(uniformMaterialPhong, 1, &material[0]);
    glUniform3fv(uniformLightPositionPhong, 1, &lightPosition[0]);
    glUniform3f(uniformLightColorPhong, lightColor.x(), lightColor.y(), lightColor.z());
    glUniform3f(uniformCameraPosition, camera.getPosition().x(), camera.getPosition().y(), camera.getPosition().z());

    glUniform1i(uniformTextureSamplerPhong, 0);
}

void MainView::updateProjectionTransform() {
    float aspectRatio = static_cast<float>(width()) / static_cast<float>(height());
    projectionTransform.setToIdentity();
    projectionTransform.perspective(
                camera.getFOV(),
                aspectRatio,
                camera.getNearPlane(),
                camera.getFarPlane()
            );
}

void MainView::updateViewTransform() {
    viewTransform.setToIdentity();
    viewTransform.lookAt(camera.getPosition(), solarSystem.objects[comboBox_lookingAt->currentIndex()]->getLocation(), QVector3D(0,1,0));
}

void MainView::updateModelTransform(Object *obj) {
    obj->meshTransform.setToIdentity();
    obj->meshTransform.translate(obj->getLocation());

    obj->meshTransform.rotate(rotation.x(), {1.0F, 0.0F, 0.0F});
    obj->meshTransform.rotate(rotation.y(), {0.0F, 1.0F, 0.0F});
    obj->meshTransform.rotate(rotation.z(), {0.0F, 0.0F, 1.0F});

    obj->meshTransform.scale(obj->getScale());

    obj->meshTransform.rotate(obj->getAngle(), {0,1,0});

    obj->meshNormalTransform = obj->meshTransform.normalMatrix();

    update();
}

void MainView::updateModelTransforms() {
    for (Object *o : solarSystem.objects) {
        updateModelTransform(o);
    }

    update();
}

// --- Public interface

void MainView::setShadingMode(ShadingMode shading) {
    qDebug() << "Changed shading to" << shading;
    currentShader = shading;
}

void MainView::setCameraFOV(float fov) {
    camera.setFOV(fov);
    updateProjectionTransform();
}

// --- Private helpers

/**
 * @brief MainView::onMessageLogged
 *
 * OpenGL logging function, do not change
 *
 * @param Message
 */
void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
    qDebug() << " → Log:" << Message;
}
