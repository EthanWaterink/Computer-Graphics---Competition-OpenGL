#include "model.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QMatrix4x4>
#include <limits>
#include <math.h>

Model::Model(QString filename) {
    qDebug() << ":: Loading model:" << filename;
    QFile file(filename);
    if(file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);

        QString line;
        QStringList tokens;

        while(!in.atEnd()) {
            line = in.readLine();
            if (line.startsWith("#")) continue; // skip comments

            tokens = line.split(" ", QString::SkipEmptyParts);

            // Switch depending on first element
            if (tokens[0] == "v") {
                parseVertex(tokens);
            }

            if (tokens[0] == "vn" ) {
                parseNormal(tokens);
            }

            if (tokens[0] == "vt" ) {
                parseTexture(tokens);
            }

            if (tokens[0] == "f" ) {
                parseFace(tokens);
            }
        }

        file.close();

        // create an array version of the data
        unpackIndexes();

        // Align all vertex indices with the right normal/texturecoord indices
        alignData();
    }
}

/**
 * @brief Unitize the model by scaling so that it fits a box with sides 1 and origin at 0,0,0
 * Usefull for models with different scales
 * NOTE: it unitizes the vertices_indexed!
 */
void Model::unitize() {
    // Determine minX, minY, minZ, maxX, maxY, maxZ
    float minX =  std::numeric_limits<float>::max(), minY =  std::numeric_limits<float>::max(), minZ =  std::numeric_limits<float>::max();
    float maxX = -std::numeric_limits<float>::max(), maxY = -std::numeric_limits<float>::max(), maxZ = -std::numeric_limits<float>::max();
    for (QVector3D qv : vertices_indexed) {
        if (qv.x() < minX) minX = qv.x();
        if (qv.y() < minY) minY = qv.y();
        if (qv.z() < minZ) minZ = qv.z();
        if (qv.x() > maxX) maxX = qv.x();
        if (qv.y() > maxY) maxY = qv.y();
        if (qv.z() > maxZ) maxZ = qv.z();
    }

    // One of the dimension is the longest
    float maxS = fmax(fmax (abs(maxX - minX), abs(maxY - minY)), abs(maxZ - minZ));
    float newWidth, newHeight, newDepth;
    // Divide the length by the biggest, so that the biggest will be precisly 1, and the others between 0 and 1. Multiple by 2, because the cube has length 2
    newWidth  = 2*abs(maxX - minX)/maxS;
    newHeight = 2*abs(maxY - minY)/maxS;
    newDepth  = 2*abs(maxZ - minZ)/maxS;

    // Move to (0,0,0)
    QMatrix4x4 M_moveCornerToOrigin = QMatrix4x4();
    M_moveCornerToOrigin.translate(-minX, -minY, -minZ);
    // Scale down to [0,2]
    QMatrix4x4 M_scaleDown = QMatrix4x4();
    M_scaleDown.scale(
                (float) 2 / maxS,
                (float) 2 / maxS,
                (float) 2 / maxS);
    // Move so that the center of the model is at (0,0,0)
    QMatrix4x4 M_moveCenterToOrigin = QMatrix4x4();
    M_moveCenterToOrigin.translate(-newWidth/2, -newHeight/2, -newDepth/2);

    // Compute transform matrix
    QMatrix4x4 M = M_moveCenterToOrigin * M_scaleDown * M_moveCornerToOrigin;

    // Update all the vertices
    int size = vertices_indexed.count();
    for (int v = 0; v < size; v++) {
        QVector3D qv = vertices_indexed.first();
        vertices_indexed.push_back((M * QVector4D(qv,1)).toVector3D());
        vertices_indexed.removeFirst();
    }
}

QVector<QVector3D> Model::getVertices() {
    return vertices;
}

QVector<QVector3D> Model::getNormals() {
    return normals;
}

QVector<QVector2D> Model::getTextureCoords() {
    return textureCoords;
}

QVector<QVector3D> Model::getVertices_indexed() {
    return vertices_indexed;
}

QVector<QVector3D> Model::getNormals_indexed() {
    return normals_indexed;
}

QVector<QVector2D> Model::getTextureCoords_indexed() {
    return textureCoords_indexed;
}

QVector<unsigned>  Model::getIndices() {
    return indices;
}

QVector<float> Model::getVNInterleaved() {
    QVector<float> buffer;

    for (int i = 0; i != vertices.size(); ++i) {
        QVector3D vertex = vertices.at(i);
        QVector3D normal = normals.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
    }

    return buffer;
}

QVector<float> Model::getVNTInterleaved() {
    QVector<float> buffer;

    for (int i = 0; i != vertices.size(); ++i) {
        QVector3D vertex = vertices.at(i);
        QVector3D normal = normals.at(i);
        QVector2D uv = textureCoords.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
        buffer.append(uv.x());
        buffer.append(uv.y());
    }

    return buffer;
}

// Throws when there are no normals or texture values
QVector<float> Model::getVNInterleaved_indexed() {
    QVector<float> buffer;

    for (int i = 0; i != vertices_indexed.size(); ++i) {
        QVector3D vertex = vertices_indexed.at(i);
        QVector3D normal = normals_indexed.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
    }

    return buffer;
}

// Throws when there are no normals or texture values
QVector<float> Model::getVNTInterleaved_indexed() {
    QVector<float> buffer;

    for (int i = 0; i != vertices_indexed.size(); ++i) {
        QVector3D vertex = vertices_indexed.at(i);
        QVector3D normal = normals_indexed.at(i);
        QVector2D uv = textureCoords_indexed.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(normal.x());
        buffer.append(normal.y());
        buffer.append(normal.z());
        buffer.append(uv.x());
        buffer.append(uv.y());
    }

    return buffer;
}

QVector<float> Model::getVNinvTInterleaved_indexed() {
    QVector<float> buffer;

    for (int i = 0; i != vertices_indexed.size(); ++i) {
        QVector3D vertex = vertices_indexed.at(i);
        QVector3D normal = normals_indexed.at(i);
        QVector2D uv = textureCoords_indexed.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(-normal.x());
        buffer.append(-normal.y());
        buffer.append(-normal.z());
        buffer.append(uv.x());
        buffer.append(uv.y());
    }

    return buffer;
}

// Throws when there are no texture values
QVector<float> Model::getVTInterleaved_indexed() {
    QVector<float> buffer;

    for (int i = 0; i != vertices_indexed.size(); ++i) {
        QVector3D vertex = vertices_indexed.at(i);
        QVector2D uv = textureCoords_indexed.at(i);
        buffer.append(vertex.x());
        buffer.append(vertex.y());
        buffer.append(vertex.z());
        buffer.append(uv.x());
        buffer.append(uv.y());
    }

    return buffer;
}

/**
 * @brief Model::getNumTriangles
 *
 * Gets the number of triangles in the model
 *
 * @return number of triangles
 */
int Model::getNumTriangles() {
    return vertices.size()/3;
}

void Model::parseVertex(QStringList tokens) {
    float x,y,z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    vertices_indexed.append(QVector3D(x,y,z));
}

void Model::parseNormal(QStringList tokens) {
    hNorms = true;
    float x,y,z;
    x = tokens[1].toFloat();
    y = tokens[2].toFloat();
    z = tokens[3].toFloat();
    norm.append(QVector3D(x,y,z));
}

void Model::parseTexture(QStringList tokens) {
    hTexs = true;
    float u,v;
    u = tokens[1].toFloat();
    v = tokens[2].toFloat();
    tex.append(QVector2D(u,v));
}

void Model::parseFace(QStringList tokens) {
    QStringList elements;

    for( int i = 1; i != tokens.size(); ++i ) {
        elements = tokens[i].split("/");
        // -1 since .obj count from 1
        indices.append(elements[0].toInt()-1);

        if ( elements.size() > 1 && ! elements[1].isEmpty() ) {
            texcoord_indices.append(elements[1].toInt()-1);
        }

        if (elements.size() > 2 && ! elements[2].isEmpty() ) {
            normal_indices.append(elements[2].toInt()-1);
        }
    }
}


/**
 * @brief Model::alignData
 *
 * Make sure that the indices from the vertices align with those
 * of the normals and the texture coordinates, create extra vertices
 * if vertex has multiple normals or texturecoords
 */
void Model::alignData() {
    QVector<QVector3D> verts = QVector<QVector3D>();
    verts.reserve(vertices_indexed.size());
    QVector<QVector3D> norms = QVector<QVector3D>();
    norms.reserve(vertices_indexed.size());
    QVector<QVector2D> texcs = QVector<QVector2D>();
    texcs.reserve(vertices_indexed.size());
    QVector<Vertex> vs = QVector<Vertex>();

    QVector<unsigned> ind = QVector<unsigned>();
    ind.reserve(indices.size());

    unsigned currentIndex = 0;

    for (int i = 0; i != indices.size(); ++i) {
        QVector3D v = vertices_indexed[indices[i]];

        QVector3D n = QVector2D(0,0);
        if ( hNorms ) {
            n = norm[normal_indices[i]];
        }

        QVector2D t = QVector2D(0,0);
        if ( hTexs ) {
            t = tex[texcoord_indices[i]];
        }

        Vertex k = Vertex(v,n,t);
        if (vs.contains(k)) {
            // Vertex already exists, use that index
            ind.append(vs.indexOf(k));
        } else {
            // Create a new vertex
            verts.append(v);
            norms.append(n);
            texcs.append(t);
            vs.append(k);
            ind.append(currentIndex);
            ++currentIndex;
        }
    }
    // Remove old data
    vertices_indexed.clear();
    normals_indexed.clear();
    textureCoords_indexed.clear();
    indices.clear();

    // Set the new data
    vertices_indexed = verts;
    normals_indexed = norms;
    textureCoords_indexed = texcs;
    indices = ind;
}

/**
 * @brief Model::unpackIndexes
 *
 * Unpack indices so that they are available for glDrawArrays()
 *
 */
void Model::unpackIndexes() {
    vertices.clear();
    normals.clear();
    textureCoords.clear();
    for ( int i = 0; i != indices.size(); ++i ) {
        vertices.append(vertices_indexed[indices[i]]);

        if ( hNorms ) {
            normals.append(norm[normal_indices[i]]);
        }

        if ( hTexs ) {
            textureCoords.append(tex[texcoord_indices[i]]);
        }
    }
}
