#pragma once
class RadarOwnship
{
public:
    RadarOwnship();

    void init();

    float m_L;
    float m_B;
    float m_antennaHeight;
    float m_maxSpeed;
    float m_speed;
    float m_course;
    float m_heading;
    double m_lat;
    double m_lon;
    float m_x;
    float m_y;
    float m_z;

};

