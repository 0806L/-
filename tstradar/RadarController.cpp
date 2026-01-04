#include "RadarController.h"

#include "radarwidget.h"
#include "radardata.h"
#include "GeoTransform.h"

////////////////////////////////////////////////////////////////////////////////

/*
static float s_ranges[] = {
    0.5f, 0.75f, 1.5f, 3.0f, 6.0f, 12.0f, 24.0f, 48.0f, 96.0f
};

static int s_numOfRings[] = {
    6, 6, 6, 6, 6, 6, 6, 6, 6
};
*/


class RadarControllerPrivate
{
public:
    RadarControllerPrivate()
    {
        m_widget = nullptr;
        m_data = nullptr;
    }

    RadarWidget* m_widget;
    RadarData* m_data;

#if 0

    // 雷达数据
    RadarOwnship m_ship;
    RadarPPI m_ppi;
    QList<RadarTarget> m_targets;
    int m_rangeIndex;
    float m_angle;

	GeoTransform m_geo;

    QColor m_backgroundColor;

    void addRange()
    {
        size_t n = sizeof(s_ranges) / sizeof(s_ranges[0]);

        m_rangeIndex++;
        if (m_rangeIndex >= n)
            m_rangeIndex = n - 1;

        m_ppi.m_range = s_ranges[m_rangeIndex] * 1852.0;
    }

    void subRange()
    {
        size_t n = sizeof(s_ranges) / sizeof(s_ranges[0]);

        m_rangeIndex--;
        if (m_rangeIndex < 0)
            m_rangeIndex = 0;

        m_ppi.m_range = s_ranges[m_rangeIndex] * 1852.0;
    }

    void setRange(float val)
    {
        int idx = -1;
        size_t n = sizeof(s_ranges) / sizeof(s_ranges[0]);
        for (size_t i = 0; i < n; i++)
        {
            if (s_ranges[i] >= val)
            {
                idx = i;
            }
        }

        m_rangeIndex = idx;
        if (m_rangeIndex < 0)
            m_rangeIndex = n - 1;

        m_ppi.m_range = s_ranges[m_rangeIndex] * 1852.0;
    }
#endif
};

////////////////////////////////////////////////////////////////////////////////

RadarController::RadarController(RadarWidget* w, RadarData* data)
{
    m_p = new RadarControllerPrivate();
    m_p->m_widget = w;
    m_p->m_data = data;
}

RadarController::~RadarController()
{

}

void RadarController::setBackgroundColor(QColor c)
{
    m_p->m_data->m_backgroundColor = c;
}

void RadarController::setRange(float val)
{
    m_p->m_data->setRange(val);
}

void RadarController::addRange()
{
    m_p->m_data->addRange();
}

void RadarController::subRange()
{
    m_p->m_data->subRange();
}

float RadarController::range()
{
    return m_p->m_data->m_ppi.m_range / 1852.0;
}

void RadarController::setGain(float val)
{
    val = qMin(val, 1.0f);
    val = qMax(val, 0.0f);

    qDebug() << "gain:" << val;
    m_p->m_data->m_ppi.m_gain = val;
}

float RadarController::gain()
{
    return m_p->m_data->m_ppi.m_gain;
}

void RadarController::setTune(float val)
{
    val = qMin(val, 1.0f);
    val = qMax(val, 0.0f);
    // 调谐有问题,先不开放
    m_p->m_data->m_ppi.m_tune = val;
}

float RadarController::tune()
{
    return m_p->m_data->m_ppi.m_tune;
}

void RadarController::setSea(float val)
{
    val = qMin(val, 1.0f);
    val = qMax(val, 0.0f);
    m_p->m_data->m_ppi.m_sea = val;
}

float RadarController::sea()
{
    return m_p->m_data->m_ppi.m_sea;
}

void RadarController::setRain(float val)
{
    val = qMin(val, 1.0f);
    val = qMax(val, 0.0f);
    m_p->m_data->m_ppi.m_rain = val;
}

float RadarController::rain()
{
    return m_p->m_data->m_ppi.m_rain;
}

void RadarController::setEBL1On(bool on)
{
    m_p->m_data->m_ppi.m_eblvrm1.eblon = on;
}

bool RadarController::EBL1On()
{
    return m_p->m_data->m_ppi.m_eblvrm1.eblon;
}

void RadarController::setEBL2On(bool on)
{
    m_p->m_data->m_ppi.m_eblvrm2.eblon = on;
}

bool RadarController::EBL2On()
{
    return m_p->m_data->m_ppi.m_eblvrm2.eblon;
}

void RadarController::setEBL1Angle(float val)
{
    m_p->m_data->m_ppi.m_eblvrm1.ebl = val;
}

float RadarController::EBL1Angle()
{
    return m_p->m_data->m_ppi.m_eblvrm1.ebl;
}

void RadarController::setEBL2Angle(float val)
{
    m_p->m_data->m_ppi.m_eblvrm2.ebl = val;
}

float RadarController::EBL2Angle()
{
    return m_p->m_data->m_ppi.m_eblvrm2.ebl;
}

void RadarController::setVRM1On(bool on)
{
    m_p->m_data->m_ppi.m_eblvrm1.vrmon = on;
}

bool RadarController::VRM1On()
{
    return m_p->m_data->m_ppi.m_eblvrm1.vrmon;
}

void RadarController::setVRM2On(bool on)
{
    m_p->m_data->m_ppi.m_eblvrm2.vrmon = on;
}

bool RadarController::VRM2On()
{
    return m_p->m_data->m_ppi.m_eblvrm2.vrmon;
}

void RadarController::setVRM1Dist(float val)
{
    m_p->m_data->m_ppi.m_eblvrm1.vrm = val;
}

float RadarController::VRM1Dist()
{
    return m_p->m_data->m_ppi.m_eblvrm1.vrm;
}

void RadarController::setVRM2Dist(float val)
{

    m_p->m_data->m_ppi.m_eblvrm2.vrm = val;
}

float RadarController::VRM2Dist()
{
    return m_p->m_data->m_ppi.m_eblvrm2.vrm;
}


void RadarController::setOwnShipPos(float lat, float lon)
{
    float x1, y1;
    m_p->m_data->m_geo.latlon2xy(lat, lon, &x1, &y1);
    m_p->m_data->m_ship.m_lat = lat;
    m_p->m_data->m_ship.m_lon = lon;
    m_p->m_data->m_ship.m_x = x1;
    m_p->m_data->m_ship.m_y = y1;
}

void RadarController::setOwnShipSog(float val)
{
    m_p->m_data->m_ship.m_speed = val;
}

void RadarController::setOwnShipCog(float val)
{
    m_p->m_data->m_ship.m_course = val;
}

void RadarController::setOwnShipHdg(float val)
{
    m_p->m_data->m_ship.m_heading = val;
}

RadarOwnship RadarController::ownship()
{
    return m_p->m_data->m_ship;
}

void RadarController::latLonToXY(float lat, float lon, float* x, float* y)
{
    float x1, y1;
    m_p->m_data->m_geo.latlon2xy(lat, lon, &x1, &y1);

    QPointF pt = m_p->m_data->m_ppi.PPIMeters2Screen(x1, y1);
    *x = pt.x();
    *y = pt.y();
}


void RadarController::xyToLatLon(float x, float y, float* lat, float* lon)
{
    QPointF pos = m_p->m_data->m_ppi.PPIScreen2Meters(x, y);
    m_p->m_data->m_geo.xy2latlon(pos.x(), pos.y(), lat, lon);
}

bool RadarController::loadTerrain(const QString& name)
{
    return m_p->m_widget->loadTerrain(name);
}

