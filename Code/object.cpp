#include "object.h"
#include "utility"
#include <QDebug>
#include <math.h>

Object::Object(QString n, QString filename, QString texturefile) : model(filename) {
    qDebug() << "Instantiated object " << filename;
    texture = texturefile;
    name = n;
}

Object::~Object() {
    delBuffers();
}

void Object::load() {
    initializeOpenGLFunctions();

    genBuffers();
    loadTextures();
    loadMesh();
}

void Object::genBuffers() {
    glGenBuffers(1, &meshVBO);
    glGenBuffers(1, &meshVIO);
    glGenVertexArrays(1, &meshVAO);

    glGenTextures(1, &textureDiff);
}

void Object::delBuffers() {
    glDeleteBuffers(1, &meshVBO);
    glDeleteBuffers(1, &meshVIO);
    glDeleteVertexArrays(1, &meshVAO);

    glDeleteTextures(1, &textureDiff);
}

void Object::loadTexture(QString file, GLuint textureName) {
    // Set texture parameters.
    glBindTexture(GL_TEXTURE_2D, textureName);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Push image data to texture.
    QImage image(file);
    QVector<quint8> imageData = imageToBytes(image);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image.width(), image.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());
}

void Object::loadTextures() {
    loadTexture(texture, textureDiff);
}

QVector<float> Object::getMeshData() {
    return model.getVNTInterleaved_indexed();
}

// Inverted normals
QVector<float> Sun::getMeshData() {
    return model.getVNinvTInterleaved_indexed();
}

void Object::loadMesh() {
    qDebug() << "loadMesh";
    model.unitize();
    QVector<float> meshData = getMeshData();

    meshSize = model.getIndices().size();

    glBindVertexArray(meshVAO);

    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    // Write the data to the buffer
    glBufferData(GL_ARRAY_BUFFER, meshData.size() * sizeof(GL_FLOAT), meshData.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshVIO);
    // Write the data to the buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.getIndices().size()*sizeof(unsigned), model.getIndices().data(), GL_STATIC_DRAW);

    // Set vertex coordinates to location 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // Set vertex normals to location 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);

    // Set vertex texture coordinates to location 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), reinterpret_cast<void*>(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Object::draw() {
    // Set the texture and draw the mesh.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureDiff);

//    glActiveTexture(GL_TEXTURE1);
//    glBindTexture(GL_TEXTURE_2D, textureNorm);

    glBindVertexArray(meshVAO);
    glDrawElements(GL_TRIANGLES, meshSize, GL_UNSIGNED_INT, nullptr);
}

void Object::rotate(float a) {
    angle += a;
}


void Planet::moveAround(float t) {
    location.setX(rotateAround->getLocation().x() + distanceFrom * sin (t/orbitalPeriod));
    location.setZ(rotateAround->getLocation().z() + distanceFrom * cos (t/orbitalPeriod));
}

void Spaceship::update(float t, float s) {
    Q_UNUSED(t)
    location += s*speed*(moveTo->getLocation() - location).normalized();
}

bool Spaceship::hasReachedDestination() {
    return (this->distanceTo(moveTo) <= getScale() + moveTo->getScale()*1.5f + 5); // reached object
}
