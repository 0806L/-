#include "radartransform.h"

/*
RadarTransform::RadarTransform()
{
    m_M.setToIdentity();
    m_MBack.setToIdentity();
    m_backTransformReady = false;
}

void RadarTransform::resetIdentity()
{
    m_M.setToIdentity();
    m_backTransformReady = false;
}

void RadarTransform::translate(double x, double y, double z)
{
    QMatrix4x4 temp;
    temp.setToIdentity();

    temp(0, 3) = x;
    temp(1, 3) = y;
    temp(2, 3) = z;

    m_M *= temp;
    m_backTransformReady = false;
}
*/

RadarTransform::RadarTransform()
{
    init();
}

void RadarTransform::init()
{
    M.setToIdentity();
    Mback.setToIdentity();
}

void RadarTransform::identity()
{
    M.setToIdentity();
    BackTransformReady = false;
}

void RadarTransform::translate(float x, float y, float z)
{
    QMatrix4x4 temp = QMatrix4x4(
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1);
    M = temp * M;
    BackTransformReady = false;
}

void RadarTransform::resize(float x, float y, float z)
{
    QMatrix4x4 temp = QMatrix4x4(
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1);
    M = temp * M;
    BackTransformReady = false;
}

void RadarTransform::rotate(float x, float y, float z)
{
    float sincourse = sin(-x);
    float coscourse = cos(-x);
    float sindifferent = sin(-y);
    float cosdifferent = cos(-y);
    float sinroll = sin(-z);
    float cosroll = cos(-z);
    float SBCA = sindifferent * coscourse;
    float SBSA = sindifferent * sincourse;

    QMatrix4x4 temp = QMatrix4x4(cosdifferent * coscourse, sindifferent, cosdifferent * sincourse, 0,
        -SBCA * cosroll + sincourse * sinroll, cosdifferent * cosroll, -SBSA * cosroll - sinroll * coscourse, 0,
        -SBCA * sinroll - sincourse * cosroll, cosdifferent * sinroll, -SBSA * sinroll + coscourse * cosroll, 0,
        0, 0, 0, 1);
    M = temp * M;
    BackTransformReady = false;
}

QVector4D RadarTransform::get(const QVector4D& v)
{
    return v * M.transposed();
}

QVector4D RadarTransform::getBack(const QVector4D& v)
{
    if (!BackTransformReady)
    {
        Mback = M.inverted();
        BackTransformReady = true;
    }
    return v * Mback.transposed();
}
