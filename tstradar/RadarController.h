#pragma once

#include <QObject>
#include <QPointF>
#include <QColor>

#include "radarownship.h"

class RadarControllerPrivate;
class RadarWidget;
class RadarData;

class RadarController : public QObject
{
    Q_OBJECT
public:
    RadarController(RadarWidget* w, RadarData* data);
    ~RadarController();

    void setBackgroundColor(QColor c);

    void setRange(float val);
    void addRange();
    void subRange();
    float range();

    // val限制在[0, 1]
    void setGain(float val);
    float gain();

    // val限制在[0, 1]
    void setTune(float val);
    float tune();

    // val限制在[0, 1]
    void setSea(float val);
    float sea();

    // val限制在[0, 1]
    void setRain(float val);
    float rain();

    void setEBL1On(bool on);
    bool EBL1On();

    void setEBL2On(bool on);
    bool EBL2On();

    void setEBL1Angle(float val);
    float EBL1Angle();
    void setEBL2Angle(float val);
    float EBL2Angle();

    void setVRM1On(bool on);
    bool VRM1On();

    void setVRM2On(bool on);
    bool VRM2On();

    void setVRM1Dist(float val);
    float VRM1Dist();

    void setVRM2Dist(float val);
    float VRM2Dist();

    void setOwnShipPos(float lat, float lon);
    // 节
    void setOwnShipSog(float val);
    // 度
    void setOwnShipCog(float val);
    // 度
    void setOwnShipHdg(float val);
    RadarOwnship ownship();

    void latLonToXY(float lat, float lon, float *x, float *y);
    void xyToLatLon(float x, float y, float* lat, float *lon);

    bool loadTerrain(const QString& name);

Q_SIGNALS:

    void cursorPosChanged(float lat, float lon, float bearing, float distNM);

private:

    RadarControllerPrivate* m_p;

};

