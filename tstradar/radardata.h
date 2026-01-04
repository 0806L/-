#pragma once

#include <qglobal.h>
#include <QString>

#include "radarownship.h"
#include "radarppi.h"
#include "radartarget.h"
#include "GeoTransform.h"


class RadarData : public QObject
{
    Q_OBJECT
public:
    RadarData();

    // À×´ïÊý¾Ý
    RadarOwnship m_ship;
    RadarPPI m_ppi;
    QList<RadarTarget> m_targets;
    int m_rangeIndex;
    float m_angle;

	GeoTransform m_geo;

    QColor m_backgroundColor;

    void addRange();

    void subRange();

    void setRange(float val);

    int numOfRings();
};
