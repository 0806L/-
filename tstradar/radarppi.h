#pragma once

#include "radartransform.h"

#include <QPainter>

enum RANGERINGS_e
{
    RANGRINGS_OFF = 0,
    RANGRINGS_ON = 1
};

enum MOTION_e
{
    MOTION_RELATIVE = 0,
    MOTION_TRUE = 1,
};

enum HEADINGMODE_e
{
    HMODE_NUP = 0,
    HMODE_CUP = 1,
    HMODE_HUP = 2,
};

enum HEADINGLINE_e
{
    HEADINGLINE_OFF = 0,
    HEADINGLINE_ON = 1,
};

enum EBLVRMONOFF_e
{
    // EBL和VRM全部关闭
    EBLVRM_OFF = 0,
    // EBL和VRM全部打开
    EBLVRM_ON = 1,
    // 激活状态
    // EBLVRM_ACTIVE = 2,
    // 关闭EBL,打开VRM
    EBLOFF_VRMON = 3,
    // 打开EBL,关闭VRM
    EBLON_VRMOFF = 4,
};

enum GUARDZONE_e
{
    GRD_OFF = 0,
    GRD_ON = 1,
    GRD_ACTIVE = 2
};

enum EBLVRMFLOAT_e
{
    EBLVRM_FLOAT_OFF = 0,
    EBLVRM_FLOAT_ON = 1,
};

enum BEARING_e
{
    BEARING_RELATIVE = 0,
    BEARING_TRUE = 1,
};


class RadarOwnship;

class RadarPPI
{
public:
    RadarPPI();

    struct EBLVRM_t
    {
        // onoff只用于状态更新,不要直接设置
        // 使用eblon和vrmon
        EBLVRMONOFF_e onoff;
        bool eblon;
        bool vrmon;
        BEARING_e bearingmode;
        // ebl角度
        float ebl;
        // vrm距离
        float vrm;
        float x;
        float y;
        EBLVRMFLOAT_e floating;
        float screenx = -1;
        float screeny = -1;
    };

    void init();
    void updateTransform(RadarOwnship& ship);
    QPointF PPIMeters2Screen(float x, float y);
    QPointF PPIScreen2Meters(float xi, float yi);
    bool PPICenterOffset();
    bool PPIOffset();
    float TrimAngle(float angle);

    void GetBearingAndRange(float dx, float dy, BEARING_e mode, RadarOwnship& ship, float *bearing, float *distNM);

    void EBLVRMInit();

    float getRelativeAngle(double a, RadarOwnship& ship);

    void PPIDrawScale(QPainter& painter, RadarOwnship& ship);
    void PPIDrawRings(QPainter& painter, RadarOwnship& ship, int numrings);
    void PPIDrawHeadingLine(QPainter& painter, RadarOwnship& ship);
    void PPIDrawCursor(QPainter& painter);
    void PPIDrawCross(QPainter& painter, float x, float y, float size);
    void PPIDrawEBLVRM(QPainter& painter, RadarOwnship& ship);

    void DrawLine(QPainter& painter, float offsetx, float offsety, float angledeg);

    void DrawRing(QPainter& painter, float offsetx, float offsety, float radius, bool text);

    RadarTransform m_transform;
    RANGERINGS_e m_rangeRings;
    MOTION_e m_motion;
    HEADINGMODE_e m_headingMode;
    HEADINGLINE_e m_headline;
    BEARING_e m_bearing;

    float m_center_dx;
    float m_center_dy;
    float m_motion_startx;
    float m_motion_starty;

    float m_width;
    float m_height;

    float m_range;
    // 雷达波束宽度
    float m_beam_width;
    // 雷达波束高度
    float m_beam_height;

    float m_cursor_x;
    float m_cursor_y;
    float m_cursor_size;

    float m_brilliance;
    float m_gain;
    float m_tune;
    float m_sea;
    float m_rain;
    float m_FTC;

    EBLVRM_t m_eblvrm1;
    EBLVRM_t m_eblvrm2;

    QColor m_color;
    QColor m_inactivecolor;

};

