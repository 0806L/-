#include "transformutils.h"

TransformUtil::TransformUtil()
{
    init();
}

void TransformUtil::init()
{
    M.setToIdentity();
    Mback.setToIdentity();
}

void TransformUtil::identity()
{
    M.setToIdentity();
    BackTransformReady = false;
}

void TransformUtil::translate(float x, float y, float z)
{
    QMatrix4x4 temp = QMatrix4x4(1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1);
    M = temp * M;
    BackTransformReady = false;
}

void TransformUtil::resize(float x, float y, float z)
{
    QMatrix4x4 temp = QMatrix4x4(x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1);
    M = temp * M;
    BackTransformReady = false;
}

void TransformUtil::rotate(float x, float y, float z)
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

QVector3D TransformUtil::get(const QVector3D& v)
{
    return M * v;
}

QVector3D TransformUtil::getBack(const QVector3D& v)
{
    if (!BackTransformReady)
    {
        Mback = M.inverted();
        BackTransformReady = true;
    }
    return Mback * v;
}
