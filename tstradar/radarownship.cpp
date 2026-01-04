#include "radarownship.h"

RadarOwnship::RadarOwnship()
{

    init();
}

void RadarOwnship::init()
{
    m_L = 100.0;
    m_B = 15.0;
    m_antennaHeight = 30.0;
    m_maxSpeed = 20.0;
    m_speed = 0.0;
    m_course = 0.0;
    m_heading = 0.0;
    m_lat = 24.5;
    m_lon = 118.25;
    m_x = 0;
    m_y = 0;
}
