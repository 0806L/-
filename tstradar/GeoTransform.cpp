#include "GeoTransform.h"

#include <math.h>

#ifndef _PI
#define _PI 3.1415926535897
#endif

void GeoTransform::init(float clat, float clon)
{
    float lat = clat * 60.0;
    float lon = clon * 60.0;

    m_centreLat = lat;
    m_centreLon = lon;

    float latangle = (lat * 0.01666666666667) * _PI / 180.0;
    m_centreCos = cos(latangle);
    m_cosMileLength = 1851.8518519 * m_centreCos;
    m_latCentreBase = getDLat(lat);
}

void GeoTransform::latlon2xy(float lat, float lon, float* x, float* y)
{
    lat = lat * 60.0f;
    lon = lon * 60.0f;
    float dlat = getDLat(lat) - m_latCentreBase;
    float dlon = lon - m_centreLon;

    *x = dlon * m_cosMileLength;
    *y = dlat * m_cosMileLength;
}

void GeoTransform::xy2latlon(float x, float y, float* lat, float* lon)
{
    float lat_min = getDY((y / m_cosMileLength) + m_latCentreBase);
    float lon_min = (x / m_cosMileLength) + m_centreLon;
    *lat = lat_min / 60.0f;
    *lon = lon_min / 60.0f;
}

float GeoTransform::getDLat(float lat)
{
    return log(tan((lat * 0.5 + 2700.0) * _PI / 10800.0)) * 10800.0 / _PI;
}

float GeoTransform::getDY(float lat)
{
    return (atan(exp(lat * _PI / 10800.0)) * 10800.0 / _PI - 2700.0) * 2.0;
}
