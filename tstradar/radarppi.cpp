#include "radarppi.h"

#include "radarownship.h"

#define _USE_MATH_DEFINES
#include <math.h>


float GetSlope(float Xt, float Yt)
{
    const float NEARLY_ZERO = 0.000001f;
    if (fabs(Xt) > NEARLY_ZERO)
    {
        if (Xt > 0)
            return atanf(Yt / Xt);
        else
            return atanf(Yt / Xt) + M_PI;
    }
    else
    {
        if (Yt > 0)
            return M_PI * 0.5;
        else
            return -M_PI * 0.5;
    }
    return 0.0f;
}

float GetSlopeDeg(float Xt, float Yt)
{
    float rad = atan2f(Yt, Xt);
    // float rad = GetSlope(Xt, Yt);
    return rad * 180.0f / M_PI;
}


RadarPPI::RadarPPI()
{
    init();
}

void RadarPPI::init()
{
    m_rangeRings = RANGRINGS_ON;
    m_motion = MOTION_RELATIVE;
    m_headingMode = HMODE_HUP;
    m_headline = HEADINGLINE_ON;
    m_bearing = BEARING_RELATIVE;

    m_center_dx = 0;
    m_center_dy = 0;

    m_motion_startx = 0;
    m_motion_starty = 0;

    m_width = 512;
    m_height = 512;

    m_range = 12 * 1852.0;

	m_beam_width = 5.0;
	m_beam_height = 20.0;

	m_cursor_x = m_width / 2;
	m_cursor_y = m_height / 2;
	m_cursor_size = 20;


	m_brilliance = 1.0;

	m_gain = 1.0;
	m_tune = 1.0;
	m_sea = 1.0;
	m_rain = 1.0;
	m_FTC = 1.0;

    EBLVRMInit();

    m_eblvrm1.onoff = EBLVRM_OFF;
    m_eblvrm1.vrm = m_range * 0.333333f;
    m_eblvrm1.ebl = 45.f;

    m_eblvrm2.onoff = EBLVRM_OFF;
    m_eblvrm2.vrm = m_range * 0.5f;
    m_eblvrm2.ebl = 120.f;

    m_color = QColor(0, 255, 0);
    m_inactivecolor = QColor(0, 127, 0);
}

void RadarPPI::updateTransform(RadarOwnship& ship)
{

    m_transform.identity();

    if (m_motion == MOTION_RELATIVE)
    {
        m_transform.translate(-ship.m_x + m_center_dx, -ship.m_y + m_center_dy, 0);
        if (m_headingMode == HMODE_HUP)
            m_transform.rotate(0, ship.m_heading * M_PI / 180.0, 0);
        else if (m_headingMode == HMODE_CUP)
            m_transform.rotate(0, ship.m_course * M_PI / 180.0, 0);
    }
    else
    {
        m_transform.translate(-m_motion_startx, -m_motion_starty, 0);
    }

    m_transform.resize(m_width / 2.0 / m_range, -m_height / 2.0 / m_range, 1.0);
    m_transform.translate(m_width / 2.0, m_height / 2.0, 0.0);

    // auto aaa = PPIMeters2Screen(ship.m_x, ship.m_y);
    // int stop = 1;
}

QPointF RadarPPI::PPIMeters2Screen(float x, float y)
{
    QVector4D screenpos = m_transform.get(QVector4D(x, y, 0, 1));

    return QPointF(screenpos.x(), screenpos.y());
}

QPointF RadarPPI::PPIScreen2Meters(float xi, float yi)
{
    QVector4D pos = m_transform.getBack(QVector4D(xi, yi, 0, 1));

    return QPointF(pos.x(), pos.y());
}

bool RadarPPI::PPICenterOffset()
{
    return (abs(m_center_dx) > 0.001 || abs(m_center_dy) > 0.001 || m_motion == MOTION_TRUE);
}

bool RadarPPI::PPIOffset()
{
    return (abs(m_center_dx) > 1.0 || abs(m_center_dy) > 1.0);
}

float RadarPPI::TrimAngle(float angle)
{
    while (angle < 0.0) angle += 360.0f;
    while (angle >= 360.0) angle -= 360.0f;

    return angle;
}

void RadarPPI::GetBearingAndRange(float dx, float dy, BEARING_e mode, RadarOwnship& ship, float* bearing, float* distNM)
{
    float rangenm = sqrt(dx * dx + dy * dy) / 1852.0;
    float brg = GetSlopeDeg(dy, dx);

    if (mode == BEARING_RELATIVE)
    {
        brg -= ship.m_heading;
    }

    brg = TrimAngle(brg);

    *distNM = rangenm;
    *bearing = brg;
}

void RadarPPI::EBLVRMInit()
{
    m_eblvrm1.onoff = EBLVRM_OFF;
    m_eblvrm1.vrmon = false;
    m_eblvrm1.eblon = false;
    m_eblvrm1.bearingmode = BEARING_RELATIVE;
    m_eblvrm1.ebl = 0.0f;
    m_eblvrm1.vrm = 0.0f;
    m_eblvrm1.floating = EBLVRM_FLOAT_OFF;
    m_eblvrm1.x = 0.0f;
    m_eblvrm1.y = 0.0f;
    m_eblvrm1.screenx = -1.0f;
    m_eblvrm1.screeny = -1.0f;

    m_eblvrm2.onoff = EBLVRM_OFF;
    m_eblvrm2.vrmon = false;
    m_eblvrm2.eblon = false;
    m_eblvrm2.bearingmode = BEARING_RELATIVE;
    m_eblvrm2.ebl = 0.0f;
    m_eblvrm2.vrm = 0.0f;
    m_eblvrm2.floating = EBLVRM_FLOAT_OFF;
    m_eblvrm2.x = 0.0f;
    m_eblvrm2.y = 0.0f;
    m_eblvrm2.screenx = -1.0f;
    m_eblvrm2.screeny = -1.0f;
}

float RadarPPI::getRelativeAngle(double a, RadarOwnship& ship)
{
    if (m_headingMode == HMODE_CUP)
        return a + ship.m_course;

    if (m_headingMode == HMODE_HUP)
        return a + ship.m_heading;

    return a + ship.m_course;
}

void RadarPPI::PPIDrawScale(QPainter& painter, RadarOwnship& ship)
{
    QPointF Sbeam(ship.m_x, ship.m_y);

    bool offset = PPICenterOffset();
    const float MARK_COEF = 0.01f;
    const float MARK_COEF5 = 0.02f;
    const float MARK_COEF10 = 0.02f;
    const int RINGS_TEXTWIDTH = 20;

    for (int i = 0; i < 360; i += 1)
    {
        double a = i * M_PI / 180.0;
        QPointF Dbeam(sin(a), cos(a));

        double markcoef;
        if ((i % 10) == 0) 
            markcoef = MARK_COEF10;
        else if ((i % 5) == 0) 
            markcoef = MARK_COEF5;
        else
            markcoef = MARK_COEF;

        double testa;
        if (m_headingMode == HMODE_HUP)
        {
            testa = i - qRound(ship.m_heading);
        }
        else if (m_headingMode == HMODE_CUP)
        {
            testa = i - qRound(ship.m_course);
        }
        else
        {
            testa = i;
        }
        testa = TrimAngle(testa);

        double intr0, intr1;
        double v20, v21;

        double r = m_range * (1.0 - 2.0 / m_width);
        double t = r;
        if (offset)
        {
            double dx = m_center_dx;
            double dy = m_center_dy;
            if (m_motion == MOTION_TRUE)
            {
                dx = ship.m_x - m_motion_startx;
                dy = ship.m_y - m_motion_starty;
            }

            double a_quad = Dbeam.x() * Dbeam.x() + Dbeam.y() * Dbeam.y();
            double b_quad = 2.0 * (Dbeam.x() * dx + Dbeam.y() * dy);
            double c_quad = dx * dx + dy * dy - r * r;
            t = (-b_quad + sqrt(b_quad * b_quad - 4.0 * a_quad * c_quad)) / (a_quad + a_quad);

            intr0 = Sbeam.x() + Dbeam.x() * t;
            intr1 = Sbeam.y() + Dbeam.y() * t;
        }
        else
        {
            intr0 = Sbeam.x() + Dbeam.x() * r;
            intr1 = Sbeam.y() + Dbeam.y() * r;
        }

        v20 = intr0 + (Sbeam.x() - intr0) * markcoef;
        v21 = intr1 + (Sbeam.y() - intr1) * markcoef;

        QPointF s1 = PPIMeters2Screen(intr0, intr1);
        QPointF s2 = PPIMeters2Screen(v20, v21);

        painter.setPen(QPen(m_color, 1));
        painter.drawLine(s1, s2);

        if ((i % 10) == 0)
        {
            QString str = QString::asprintf("%03d", i);
            double coef = 1.0 - RINGS_TEXTWIDTH * 3.0 / m_width;
            if (offset)
            {
                intr0 = Sbeam.x() + Dbeam.x() * t * coef;
                intr1 = Sbeam.y() + Dbeam.y() * t * coef;
            }
            else
            {
                intr0 = Sbeam.x() + Dbeam.x() * (m_range * coef);
                intr1 = Sbeam.y() + Dbeam.y() * (m_range * coef);
            }

            QPointF sc = PPIMeters2Screen(intr0, intr1);

            QFont font("Arial", 10);
            painter.setFont(font);
            painter.setPen(QPen(m_color, 1));
            QRect text_rect = painter.fontMetrics().boundingRect(str);
            text_rect.moveCenter(sc.toPoint());
            // painter.drawRect(text_rect);
            // painter.drawText(sc, str, Qt::AlignCenter);
            painter.drawText(text_rect, Qt::AlignCenter, str);
            // painter.drawPoint(sc);
            // painter.drawText(sc.x(), sc.y(), text_rect.width(), text_rect.height(), Qt::AlignCenter, str);
            // painter.drawText(sc, str);
        }
    }
}

void RadarPPI::DrawRing(QPainter& painter, float offsetx, float offsety, float radius, bool text)
{
    // 获取中心点
    QPointF screencentre = PPIMeters2Screen(offsetx, offsety);

    // 获取屏幕半径
    // double r = painter.device()->width() / 2.0 * radius / m_range;
    double r = m_width / 2.0 * radius / m_range;
    double rw = m_width / 2.0 * radius / m_range;
    double rh = m_height / 2.0 * radius / m_range;

    // 绘制圆形
    // painter.setPen(QPen(m_color, 1));
    painter.drawEllipse(screencentre, rw, rh);

    // 绘制文本
    if (text)
    {
        QString str = QString::number(radius / 1852.0); // 假设1852是转换因子
        double x = screencentre.x() + r * 0.7071;
        double y = screencentre.y() - r * 0.7071;

        QFont font("Arial", 10);
        painter.setFont(font);
        // painter.setPen(m_color);
        QRect text_rect = painter.fontMetrics().boundingRect(str);
        text_rect.moveCenter(QPoint(x, y));
        painter.drawText(text_rect, Qt::AlignCenter, str);
    }
}

void RadarPPI::PPIDrawRings(QPainter& painter, RadarOwnship& ship, int numrings)
{
    double dr = m_range / numrings;
    double r = dr;
    int num = PPIOffset() ? numrings : (numrings - 1);
    painter.setPen(QPen(m_color, 1));
    for (size_t i = 0; i < num; i++)
    {
        DrawRing(painter, ship.m_x, ship.m_y, r, false);
        r += dr;
    }
}

void RadarPPI::PPIDrawHeadingLine(QPainter& painter, RadarOwnship& ship)
{
    painter.setPen(QPen(m_color, 1));
    DrawLine(painter, ship.m_x, ship.m_y, ship.m_heading);
}

void RadarPPI::PPIDrawCursor(QPainter& painter)
{
    // 检查位置
    double dx = m_cursor_x - m_width / 2.0;
    double dy = m_cursor_y - m_height / 2.0;
    double r = sqrt(dx * dx + dy * dy);
    if (r > m_width / 2.0)
    {
        m_cursor_x = m_width / 2.0;
        m_cursor_y = m_height / 2.0;
    }

    // 绘制十字标记
    PPIDrawCross(painter, m_cursor_x, m_cursor_y, m_cursor_size);
}

void RadarPPI::PPIDrawCross(QPainter& painter, float x, float y, float size)
{
    painter.setPen(QPen(m_color, 1));
    painter.drawLine(x - size / 2, y, x + size / 2, y);
    painter.drawLine(x, y - size / 2, x, y + size / 2);
}

void RadarPPI::PPIDrawEBLVRM(QPainter& painter, RadarOwnship& ship)
{
    if (m_eblvrm1.eblon && m_eblvrm1.vrmon)
        m_eblvrm1.onoff = EBLVRM_ON;
    else if (m_eblvrm1.eblon && !m_eblvrm1.vrmon)
        m_eblvrm1.onoff = EBLON_VRMOFF;
    else if (!m_eblvrm1.eblon && m_eblvrm1.vrmon)
        m_eblvrm1.onoff = EBLOFF_VRMON;
    else
        m_eblvrm1.onoff = EBLVRM_OFF;

    // EBL/VRM1
    if (m_eblvrm1.onoff != EBLVRM_OFF)
    {
        float angledeg = m_eblvrm1.ebl;
        if (m_eblvrm1.bearingmode == BEARING_RELATIVE)
            angledeg = getRelativeAngle(m_eblvrm1.ebl, ship);

        painter.setPen(QPen(m_color, 2, Qt::DashLine));
        if (m_eblvrm1.floating == EBLVRM_FLOAT_OFF)
        {
            if (m_eblvrm1.onoff == EBLOFF_VRMON)
            {
                DrawRing(painter, ship.m_x, ship.m_y, m_eblvrm1.vrm, false);
            }
            else if (m_eblvrm1.onoff == EBLON_VRMOFF)
            {
                DrawLine(painter, ship.m_x, ship.m_y, angledeg);
            }
            else
            {
                DrawRing(painter, ship.m_x, ship.m_y, m_eblvrm1.vrm, false);
                DrawLine(painter, ship.m_x, ship.m_y, angledeg);
            }
        }

        painter.setPen(QPen(m_color, 1, Qt::SolidLine));
        float xx = ship.m_x + m_eblvrm1.x + m_eblvrm1.vrm * sin(angledeg * M_PI / 180.0);
        float yy = ship.m_y + m_eblvrm1.y + m_eblvrm1.vrm * cos(angledeg * M_PI / 180.0);
        QPointF point1 = PPIMeters2Screen(xx, yy);

        painter.drawEllipse(point1, 6, 6);

        m_eblvrm1.screenx = point1.x();
        m_eblvrm1.screeny = point1.y();
    }

    if (m_eblvrm2.eblon && m_eblvrm2.vrmon)
        m_eblvrm2.onoff = EBLVRM_ON;
    else if (m_eblvrm2.eblon && !m_eblvrm2.vrmon)
        m_eblvrm2.onoff = EBLON_VRMOFF;
    else if (!m_eblvrm2.eblon && m_eblvrm2.vrmon)
        m_eblvrm2.onoff = EBLOFF_VRMON;
    else
        m_eblvrm2.onoff = EBLVRM_OFF;

    // EBL/VRM2
    if (m_eblvrm2.onoff != EBLVRM_OFF)
    {
        float angledeg = m_eblvrm2.ebl;
        if (m_eblvrm2.bearingmode == BEARING_RELATIVE)
            angledeg = getRelativeAngle(m_eblvrm2.ebl, ship);

        painter.setPen(QPen(m_color, 1, Qt::DotLine));
        if (m_eblvrm2.floating == EBLVRM_FLOAT_OFF)
        {
            if (m_eblvrm2.onoff == EBLOFF_VRMON)
            {
                DrawRing(painter, ship.m_x, ship.m_y, m_eblvrm2.vrm, false);
            }
            else if (m_eblvrm2.onoff == EBLON_VRMOFF)
            {
                DrawLine(painter, ship.m_x, ship.m_y, angledeg);
            }
            else
            {
                DrawRing(painter, ship.m_x, ship.m_y, m_eblvrm2.vrm, false);
                DrawLine(painter, ship.m_x, ship.m_y, angledeg);
            }
        }

        painter.setPen(QPen(m_color, 1, Qt::SolidLine));
        float xx = ship.m_x + m_eblvrm2.x + m_eblvrm2.vrm * sin(angledeg * M_PI / 180.0);
        float yy = ship.m_y + m_eblvrm2.y + m_eblvrm2.vrm * cos(angledeg * M_PI / 180.0);
        QPointF point1 = PPIMeters2Screen(xx, yy);

        painter.drawEllipse(point1, 6, 6);

        m_eblvrm2.screenx = point1.x();
        m_eblvrm2.screeny = point1.y();
    }
}

void RadarPPI::DrawLine(QPainter& painter, float offsetx, float offsety, float angledeg)
{
    // coefficient to enlarge lengths when drawing
    // on PPI screen
    const double ENLARGE_COEF = 2.0;
    double rangeenlarged = m_range * ENLARGE_COEF;
    // 获取中心点
    QPointF point0 = PPIMeters2Screen(offsetx, offsety);
    // 获取第二个点
    double xx = offsetx + rangeenlarged * sin(angledeg * M_PI / 180.0);
    double yy = offsety + rangeenlarged * cos(angledeg * M_PI / 180.0);
    QPointF point1 = PPIMeters2Screen(xx, yy);

    // 绘制线条
    // painter.setPen(QPen(m_color, 1));
    painter.drawLine(point0, point1);

    // 返回结果
    // return { point0.x(), point0.y(), point1.x(), point1.y() };

}
