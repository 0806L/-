#pragma once

class GeoTransform
{
public:
    void init(float clat, float clon);

    void latlon2xy(float lat, float lon, float *x, float *y);

    void xy2latlon(float x, float y, float *lat, float *lon);

    float getDLat(float lat_min);

    float getDY(float y);

protected:

    float m_centreLat;
    float m_centreLon;

    float m_centreCos;
    float m_cosMileLength;
    float m_latCentreBase;
};

