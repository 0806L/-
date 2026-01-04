#pragma once

#include <QMatrix4x4>
#include <QVector3D>
#include <QDebug>

class TransformUtil
{
public:
    TransformUtil();

    QMatrix4x4 M;
    QMatrix4x4 Mback;
    bool BackTransformReady = false;

    void init();
    void identity();
    void translate(float x, float y, float z);

    void resize(float x, float y, float z);
    void rotate(float x, float y, float z);

    QVector3D get(const QVector3D& v);

    QVector3D getBack(const QVector3D& v);
};
