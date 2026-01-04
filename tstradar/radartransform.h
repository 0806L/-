#pragma once
#include <QMatrix4X4>

class RadarTransform
{
public:

    RadarTransform();

    /*
    void resetIdentity();

    void translate(double x, double y, double z);

    void resize(double x, double y, double z);

    QMatrix4x4 m_M;
    QMatrix4x4 m_MBack;
    bool m_backTransformReady;
    */

    QMatrix4x4 M;
    QMatrix4x4 Mback;
    bool BackTransformReady = false;

    void init();
    void identity();
    void translate(float x, float y, float z);

    void resize(float x, float y, float z);
    void rotate(float x, float y, float z);

    QVector4D get(const QVector4D& v);

    QVector4D getBack(const QVector4D& v);
};

