
#include "radardata.h"

static float s_ranges[] = {
    0.5f, 0.75f, 1.5f, 3.0f, 6.0f, 12.0f, 24.0f, 48.0f, 96.0f
};

static int s_numOfRings[] = {
    6, 6, 6, 6, 6, 6, 6, 6, 6
};


RadarData::RadarData()
{
    m_angle = 0.0f;
    // 12nm
    m_rangeIndex = 5;
}

void RadarData::addRange()
{
    size_t n = sizeof(s_ranges) / sizeof(s_ranges[0]);

    m_rangeIndex++;
    if (m_rangeIndex >= n)
        m_rangeIndex = n - 1;

    m_ppi.m_range = s_ranges[m_rangeIndex] * 1852.0;
}

void RadarData::subRange()
{
    size_t n = sizeof(s_ranges) / sizeof(s_ranges[0]);

    m_rangeIndex--;
    if (m_rangeIndex < 0)
        m_rangeIndex = 0;

    m_ppi.m_range = s_ranges[m_rangeIndex] * 1852.0;
}

void RadarData::setRange(float val)
{
    int idx = -1;
    size_t n = sizeof(s_ranges) / sizeof(s_ranges[0]);
    for (size_t i = 0; i < n; i++)
    {
        if (s_ranges[i] >= val)
        {
            idx = i;
            break;
        }
    }

    m_rangeIndex = idx;
    if (m_rangeIndex < 0)
        m_rangeIndex = n - 1;

    m_ppi.m_range = s_ranges[m_rangeIndex] * 1852.0;
}

int RadarData::numOfRings()
{
    return s_numOfRings[m_rangeIndex];
}
