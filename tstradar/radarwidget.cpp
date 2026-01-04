#define _CRT_SECURE_NO_WARNINGS
#include "radarwidget.h"

#include <QDebug>
#include <QFile>
#include <QOpenGLDebugLogger>
#include <QOpenGLContext>
#include <QDir>

#include <QPainter>

#include "RadarController.h"
#include "radardata.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <pshpack1.h>

struct demdata_header_t
{
	int32_t nrows;
	int32_t ncols;
	double xdim;
	double ydim;
	double xmin;
	double ymin;
};

#include <poppack.h>


QVector3D calc_norm(float x1, float y1, float h1, float x2, float y2, float h2, float x3, float y3, float h3)
{
    // A: (x1, y1, h1)
    // B: (x2, y2, h2)
    // C: (x3, y3, h3)
    float AB_x = x2 - x1;
    float AB_y = y2 - y1;
    float AB_z = h2 - h1;
    float AC_x = x3 - x1;
    float AC_y = y3 - y1;
    float AC_z = h3 - h1;


    float c1 = AB_y * AC_z - AB_z * AC_y;
    float c2 = AB_z * AC_x - AB_x * AC_z;
    float c3 = AB_x * AC_y - AB_y * AC_x;
    float mag = sqrt(c1 * c1 + c2 * c2 + c3 * c3);

    return QVector3D(c1/mag, c2/mag, c3/mag);
}


QVector3D calc_norm1(float x1, float y1, float x2, float y2)
{
    // A: (x1, y1, h)
    // B: (x2, y1, h)
    // C: (x2, y2, h)
    float AB_x = x2 - x1;
    float AB_y = y1 - y1;
    float AC_x = x2 - x1;
    float AC_y = y2 - y1;

    float corss_product = AB_x * AC_y - AB_y * AC_x;

    return QVector3D(0, 0, corss_product);
}

QVector3D calc_norm2(float x1, float y1, float x2, float y2)
{
    // A: (x1, y1, h)
    // B: (x2, y2, h)
    // C: (x1, y2, h)
    float AB_x = x2 - x1;
    float AB_y = y2 - y1;
    float AC_x = x1 - x1;
    float AC_y = y2 - y1;

    float corss_product = AB_x * AC_y - AB_y * AC_x;

    return QVector3D(0, 0, corss_product);
}


RadarWidget::RadarWidget()
{
    // setSurfaceType(QOpenGLWindow::OpenGLSurface);

    m_data = new RadarData();
    m_controller = new RadarController(this, m_data);

    QSurfaceFormat format;
    // format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);
    setFormat(format);

    setAttribute(Qt::WA_AlwaysStackOnTop);

    m_fragmentShader = nullptr;
    m_vertexShader = nullptr;
    m_screentexture = nullptr;

    m_numOfVert = 0;

    m_frameCount = 0;
    m_wavesNumOfTri = 0;

    m_mouseLeftPressed = false;
    setMouseTracking(true);
}

RadarWidget::~RadarWidget()
{
    makeCurrent();
    // m_shaderProgram.release();
    // m_vertexBuffer.destroy();
    doneCurrent();
}



bool RadarWidget::loadTerrain(const QString& name)
{
    QString filepath = "./data/dem/" + name + ".dat";
    if (!QDir().exists(filepath))
        return false;

    std::string filename = filepath.toStdString();
    FILE* fp = fopen(filename.c_str(), "rb");

    demdata_header_t header;
    fread(&header, sizeof(header), 1, fp);

    int16_t* data_cols = new int16_t[header.ncols];
    float xmin = header.xmin;
    float ymin = header.ymin;
    float xdim = header.xdim;
    float ydim = header.ydim;

    float xmax = xmin + header.xdim * header.ncols;
    float ymax = ymin + header.ydim * header.nrows;

    // m_geo.init((ymin + ymax) * 0.5, (xmin + xmax) * 0.5);
    m_data->m_geo.init((ymin + ymax) * 0.5, (xmin + xmax) * 0.5);
    float xxx, yyy;
    m_data->m_geo.latlon2xy(ymin, xmin, &xxx, &yyy);
    m_minx = xxx;
    m_miny = yyy;

    m_data->m_geo.latlon2xy(ymax, xmax, &xxx, &yyy);
    m_maxx = xxx;
    m_maxy = yyy;

    float zmin = FLT_MAX, zmax = -FLT_MAX;
	QImage heightMap(header.ncols, header.nrows, QImage::Format_RGB888);
    for (int i = 0; i < header.nrows; i++)
    {
        fread(data_cols, sizeof(int16_t) * header.ncols, 1, fp);
        for (int j = 0; j < header.ncols; j++)
        {

            int16_t h = data_cols[j];
            int16_t z = qMax(h, (int16_t)0);

            float r = z % 255;
            float g = z / 255;

            heightMap.setPixel(j, i, qRgb(r, g, 0));

            // float lat = ymin + i * ydim;
            // float lon = xmin + j * xdim;
        }
    }

    // heightMap.save("D:\\heigth00001.png", "PNG");

    // 默认顺序中,纬度是从低到高
    // 因此,不需要mirrored
	// QOpenGLTexture* heightTexture = new QOpenGLTexture(heightMap.mirrored());
	QOpenGLTexture* heightTexture = new QOpenGLTexture(heightMap);
	heightTexture->setMinificationFilter(QOpenGLTexture::Linear);
	heightTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    m_heightTexture = heightTexture;

#if 0
    float delta_x = 1.0f;
    float delta_y = 1.0f;
    QVector<QVector3D> vertices; // x,y坐标 + 颜色高度值
    for (int i = 0; i < header.nrows; i++)
    {
        for (int j = 0; j < header.ncols; j++)
        {

            float dz_dx = 0.0f;
            if (i == 0)
            { 
                dz_dx = (raw_vertices[(i + 1) * header.ncols + j].z() - raw_vertices[i * header.ncols + j].z()) / delta_x;
            }
            else if (i == header.nrows - 1)
            {
                dz_dx = (raw_vertices[i * header.ncols + j].z() - raw_vertices[(i - 1) * header.ncols + j].z()) / delta_x;
            }
            else
            {
                dz_dx = (raw_vertices[(i + 1) * header.ncols + j].z() - raw_vertices[(i - 1) * header.ncols + j].z()) / (2*delta_x);
            }

            float dz_dy = 0.0f;
            if (j == 0)
            { 
                dz_dy = (raw_vertices[i * header.ncols + j + 1].z() - raw_vertices[i * header.ncols + j].z()) / delta_y;
            }
            else if (j == header.ncols - 1)
            {
                dz_dy = (raw_vertices[i * header.ncols + j].z() - raw_vertices[i * header.ncols + j-1].z()) / delta_y;
            }
            else
            {
                dz_dy = (raw_vertices[i * header.ncols + j + 1].z() - raw_vertices[i * header.ncols + j - 1].z()) / (2*delta_y);
            }

            float nx = -dz_dx;
            float ny = -dz_dy;
            float nz = 1.0;
            float length = sqrt(nx * nx + ny * ny + nz * nz);
            QVector3D norm;
            if (length == 0.0)
                norm = QVector3D(0.0f, 0.0f, 1.0f);
            else
                norm = QVector3D(nx / length, ny / length, nz / length);

            auto& v = raw_vertices[i * header.ncols + j];
            float h = v.z();
            float lat = v.y();
            float lon = v.x();

			float halfStep = xdim / 2.0f;

			// 四边形的四个角点
			float latMin = lat - halfStep;
			float latMax = lat + halfStep;
			float lonMin = lon - halfStep;
			float lonMax = lon + halfStep;

			// 转换为OpenGL坐标（假设已映射到合适的范围）
			float x1 = lonMin; // 经度对应x轴
			float y1 = latMin; // 纬度对应y轴
			float x2 = lonMax;
			float y2 = latMax;

			m_data->m_geo.latlon2xy(latMin, lonMin, &x1, &y1);
			m_data->m_geo.latlon2xy(latMax, lonMax, &x2, &y2);

			// QVector3D norm = calc_norm(x1, y1, h, x2, y2, h, xxx, yyy, 30);
			// 添加两个三角形组成四边形
			vertices.push_back({ x1, y1, h });
			vertices.push_back(norm);
			vertices.push_back({ x2, y1, h });
			vertices.push_back(norm);
			vertices.push_back({ x2, y2, h });
			vertices.push_back(norm);

			vertices.push_back({ x1, y1, h });
			vertices.push_back(norm);
			vertices.push_back({ x2, y2, h });
			vertices.push_back(norm);
			vertices.push_back({ x1, y2, h });
			vertices.push_back(norm);
        }
    }
#endif


#if 0
    // m_terrainVertices.clear();
    for (int i = 0; i < header.nrows; i++)
    {
        fread(data_cols, sizeof(int16_t) * header.ncols, 1, fp);
        for (int j = 0; j < header.ncols; j++)
        {
            float h = data_cols[j];
            float lat = ymin + i * ydim;
            float lon = xmin + j * xdim;

            float halfStep = xdim / 2.0f;

            // 四边形的四个角点
            float latMin = lat - halfStep;
            float latMax = lat + halfStep;
            float lonMin = lon - halfStep;
            float lonMax = lon + halfStep;

            // 转换为OpenGL坐标（假设已映射到合适的范围）
            float x1 = lonMin; // 经度对应x轴
            float y1 = latMin; // 纬度对应y轴
            float x2 = lonMax;
            float y2 = latMax;

            m_data->m_geo.latlon2xy(latMin, lonMin, &x1, &y1);
            m_data->m_geo.latlon2xy(latMax, lonMax, &x2, &y2);

            QVector3D norm = calc_norm(x1, y1, h, x2, y2, h, xxx, yyy, 30);
            // 添加两个三角形组成四边形
            vertices.push_back({ x1, y1, h });
            vertices.push_back(norm);
            vertices.push_back({ x2, y1, h });
            vertices.push_back(norm);
            vertices.push_back({ x2, y2, h });
            vertices.push_back(norm);

            vertices.push_back({ x1, y1, h });
            vertices.push_back(norm);
            vertices.push_back({ x2, y2, h });
            vertices.push_back(norm);
            vertices.push_back({ x1, y2, h });
            vertices.push_back(norm);
            /*
            m_terrainVertices.push_back({ x1, y1, h });
            m_terrainVertices.push_back({ x2, y1, h });
            m_terrainVertices.push_back({ x2, y2, h });

            m_terrainVertices.push_back({ x1, y1, h });
            m_terrainVertices.push_back({ x2, y2, h });
            m_terrainVertices.push_back({ x1, y2, h });
            */
        }
    }
#endif

#if 0
	m_terrainVbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_terrainVbo.create();
	m_terrainVbo.bind();
	m_terrainVbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));
	m_terrainVbo.release();

    m_numOfVert = vertices.size();
#endif

    delete[] data_cols;
    fclose(fp);

    return true;
}

void RadarWidget::timerEvent(QTimerEvent* e)
{
    update();
}

void RadarWidget::mouseMoveEvent(QMouseEvent* event)
{

    QPoint pt = event->pos();

    m_data->m_ppi.m_cursor_x = pt.x();
    m_data->m_ppi.m_cursor_y = pt.y();


	QPointF pos = m_data->m_ppi.PPIScreen2Meters(pt.x(), pt.y());
	float bearing, distNM;
	float dx = pos.x() - (m_data->m_ship.m_x + m_data->m_ppi.m_eblvrm1.x);
	float dy = pos.y() - (m_data->m_ship.m_y + m_data->m_ppi.m_eblvrm1.y);
	m_data->m_ppi.GetBearingAndRange(dx, dy, m_data->m_ppi.m_eblvrm1.bearingmode, m_data->m_ship, &bearing, &distNM);

    float cursor_lat, cursor_lon;
    m_data->m_geo.xy2latlon(pos.x(), pos.y(), &cursor_lat, &cursor_lon);

    emit m_controller->cursorPosChanged(cursor_lat, cursor_lon, bearing, distNM);

    qDebug() << "MOUSE MOVE:" << event->pos();

    if (!m_mouseLeftPressed)
        return;
    if (m_draggingWhat == "EBLVRM1")
    {
        if (distNM > m_data->m_ppi.m_range / 1852.0 * 0.9)
            distNM = m_data->m_ppi.m_range / 1852.0 * 0.9;

        m_data->m_ppi.m_eblvrm1.vrm = distNM * 1852.0;
        m_data->m_ppi.m_eblvrm1.ebl = bearing;
    }
}

void RadarWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }
    m_mouseLeftPressed = true;

    QPoint pos = event->pos();

    m_data->m_ppi.m_cursor_x = pos.x();
    m_data->m_ppi.m_cursor_y = pos.y();

    // qDebug() << "MOUSE Left PRESS:" << pos;
    if (m_data->m_ppi.m_eblvrm1.onoff != EBLVRM_OFF)
    {
        float bearing = m_data->m_ppi.m_eblvrm1.ebl;
        float range = m_data->m_ppi.m_eblvrm1.vrm;
        if (m_data->m_ppi.m_bearing == BEARING_RELATIVE)
            bearing += m_data->m_ship.m_heading;

        float evx = m_data->m_ship.m_x + m_data->m_ppi.m_eblvrm1.x + range * sin(bearing * M_PI / 180.0);
        float evy = m_data->m_ship.m_y + m_data->m_ppi.m_eblvrm1.y + range * cos(bearing * M_PI / 180.0);

        QPointF screenpos = m_data->m_ppi.PPIMeters2Screen(evx, evy);

        float dx = screenpos.x() - pos.x();
        float dy = screenpos.y() - pos.y();

        float dist = sqrt(dx * dx + dy * dy);
        if (dist < 5)
        {
            m_draggingWhat = "EBLVRM1";
        }
    }
    else
    {
        QPointF newpos = m_data->m_ppi.PPIScreen2Meters(pos.x(), pos.y());
        m_data->m_ship.m_x = newpos.x();
        m_data->m_ship.m_y = newpos.y();
    }
}

void RadarWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    m_mouseLeftPressed = false;
}

void RadarWidget::initializeGL()
{

    initializeOpenGLFunctions();

    auto version = glGetString(GL_VERSION);
    auto glslver = glGetString(GL_SHADING_LANGUAGE_VERSION);

    QOpenGLDebugLogger logger;
    logger.initialize();

    logger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    connect(&logger, &QOpenGLDebugLogger::messageLogged, this, [](const QOpenGLDebugMessage& message) {
        qWarning() << "OpenGL Debug Message:" << message.message();
        });
    // 开始捕获调试消息
    logger.enableMessages();

    // glEnable(GL_DEPTH_TEST);

    QString fs_shader_file = "./data/shader/shader-fs.shader";
    QString vs_shader_file = "./data/shader/shader-vs.shader";

    m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    if (!m_vertexShader->compileSourceFile(vs_shader_file))
    {
        qDebug() << "vertex shader err:" << m_vertexShader->log();
    }

    m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    if (!m_fragmentShader->compileSourceFile(fs_shader_file))
    {
        qDebug() << "fragment shader err:" << m_fragmentShader->log();
    }

    m_shaderProgram.addShader(m_vertexShader);
    m_shaderProgram.addShader(m_fragmentShader);
    auto sid = m_shaderProgram.programId();

    if (!m_shaderProgram.link()) {
        qDebug() << "ERROR:" << m_shaderProgram.log();
    }

    m_shaderLocation.vertexPositionAttribute = m_shaderProgram.attributeLocation( "aVertexPosition");
    m_shaderLocation.vertexNormalAttribute = m_shaderProgram.attributeLocation( "aVertexNormal");
    m_shaderLocation.textureCoordAttribute = m_shaderProgram.attributeLocation( "aTextureCoord");
    m_shaderLocation.vertexColorAttribute = m_shaderProgram.attributeLocation( "aVertexColor");
    m_shaderLocation.heightTextureCoordAttribute = m_shaderProgram.attributeLocation( "aHeightTexCoord");

    m_shaderLocation.pMatrixUniform = m_shaderProgram.uniformLocation( "uPMatrix");
    m_shaderLocation.mvMatrixUniform = m_shaderProgram.uniformLocation( "uMVMatrix");
    m_shaderLocation.nMatrixUniform = m_shaderProgram.uniformLocation( "uNMatrix");
    m_shaderLocation.shipCoordUniform = m_shaderProgram.uniformLocation( "uShipCoord");

    m_shaderLocation.uHeigthMapUniform = m_shaderProgram.uniformLocation( "uHeightMap");
    m_shaderLocation.uSamplerUniform = m_shaderProgram.uniformLocation( "uSampler");
    m_shaderLocation.flagLocation = m_shaderProgram.uniformLocation( "uFlag");
    m_shaderLocation.flag1Location = m_shaderProgram.uniformLocation( "uFlag1");

    m_shaderLocation.directionalColor = m_shaderProgram.uniformLocation( "uDirectionalColor");
    m_shaderLocation.lightPosition = m_shaderProgram.uniformLocation( "uLightPosition");
    m_shaderLocation.angle0 = m_shaderProgram.uniformLocation( "uAngle0");
    m_shaderLocation.angle1 = m_shaderProgram.uniformLocation( "uAngle1");

    m_shaderLocation.antennaHeight = m_shaderProgram.uniformLocation( "uAntennaHeight");
    m_shaderLocation.beamHeight2 = m_shaderProgram.uniformLocation( "uBeamHeight2");
    m_shaderLocation.fRange = m_shaderProgram.uniformLocation( "uRange");
    m_shaderLocation.fBrilliance = m_shaderProgram.uniformLocation( "uBrilliance");
    m_shaderLocation.fGain = m_shaderProgram.uniformLocation( "uGain");
    m_shaderLocation.fTune = m_shaderProgram.uniformLocation( "uTune");
    m_shaderLocation.fSea = m_shaderProgram.uniformLocation( "uSea");
    m_shaderLocation.fRain = m_shaderProgram.uniformLocation( "uRain");
    m_shaderLocation.fFTC = m_shaderProgram.uniformLocation( "uFTC");
    m_shaderLocation.fWaveHeight = m_shaderProgram.uniformLocation( "uWaveHeight");
    m_shaderLocation.fWindDirection = m_shaderProgram.uniformLocation( "uWindDirection");
    m_shaderLocation.fRotAngle = m_shaderProgram.uniformLocation( "uRotAngle");

    // m_geo.init(-23, -44);

    loadTerrain("xiamen");
    // LoadVBO2("data/areas/xiamenc.vbo2", m_terrainvboc);
    // LoadVBO2("data/areas/xiamenm.vbo2", m_terrainvbom);
    // FillGLBuffer();
    initOffscreenTexture();

    // r0 for polar grid to represent waves
    //!!! temporarily	var r0 = ship.antennaheight / Math.tan(PPI.beamheight * 0.5 * Math.PI / 180.0);
    float r0 = 50.0;
    // max radius for polar grid to represent waves;
    // with these r0(10),r1(4000), grid 32x64 and
    // power 2 wave lengths should vary from 4 to 
    // 64 metres
    float r1 = 4000.0;
    createPolarVBO(50.0, 4000.0, 32, 64, 0, 0, 2);

    /*
    m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexPositionAttribute);
    m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexNormalAttribute);
    // m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexColorAttribute);
    // m_shaderProgram.enableAttributeArray(m_shaderLocation.textureCoordAttribute);

    m_vertexBuffer_terrainvboc.bind();

    // glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer_terrainvboc.bufferId());
    m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexPositionAttribute, GL_FLOAT, 0, 3, 24);
    m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexNormalAttribute, GL_FLOAT, 12, 3, 24);

    m_vertexBuffer_terrainvboc.release();
    */

    // 50毫秒
    // 扫描速度: 10秒360度
    // 50毫秒扫描1.8度
    startTimer(32);

    // glViewport(0, 0, 512, 512);
    // glClearColor(0, 0, 0, 1);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*
    m_vertexBuffer.create();
    m_vertexBuffer.bind();
    // 设置多边形顶点数据，这里以六边形为例
    const GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
        -0.5f,  0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f
    };
    m_vertexBuffer.allocate(vertices, sizeof(vertices));
    m_vao.create();
    m_vao.bind();
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(0);
    m_vao.release();
    m_vertexBuffer.release();
    */

    /*
    // 初始化着色器程序
    m_tstprogram.addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
        #version 330 core
        layout(location = 0) in vec3 a_Position;
        void main() {
            gl_Position = vec4(a_Position, 1.0);
        }
    )");
    m_tstprogram.addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
        #version 330 core
        out vec4 fragColor;
        void main() {
            fragColor = vec4(1.0, 0.5, 0.2, 1.0); // 橙色
        }
    )");
    m_tstprogram.link();

    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f, // 左下角
         0.5f, -0.5f, 0.0f, // 右下角
         0.0f,  0.5f, 0.0f  // 顶部
    };

    float arrVertex[] = {
        //   position                 color
        0.0f, 0.707f, 0.0f,     1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.0f,     0.0f, 0.0f, 1.0f,
    };

    // 创建和绑定VBO
    m_vbo.create();
    m_vbo.bind();
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    m_vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vbo.allocate(vertices, sizeof(vertices));

    // 定位顶点属性
    GLint posAttr = m_tstprogram.attributeLocation("a_Position");
    m_tstprogram.enableAttributeArray(posAttr);
    m_tstprogram.setAttributeBuffer(posAttr, GL_FLOAT, 0, 3, sizeof(float) * 3);
    // glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    // m_tstprogram.disableAttributeArray(posAttr);
    m_vbo.release();
    */

}

void RadarWidget::resizeGL(int w, int h)
{
    int setw = qMin(w, h);
	// 设置视口时启用多重采样
	// glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, w, h);
    m_data->m_ppi.m_width = w;
    m_data->m_ppi.m_height = h;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_TEST);

    m_screenImage = QImage(w, h, QImage::Format_ARGB32);
    m_screenImage.fill(QColor(0, 0, 0, 255));
    // QImage image(256, 256, QImage::Format_RGB32);

    m_data->m_ppi.updateTransform(m_data->m_ship);
}


void RadarWidget::paintGL()
{
    glClearColor(m_data->m_backgroundColor.redF(), m_data->m_backgroundColor.greenF(), 
        m_data->m_backgroundColor.blueF(), m_data->m_backgroundColor.alphaF());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_STENCIL_TEST); // 启用模板测试

    glStencilFunc(GL_ALWAYS, 1, 0xFF); // 总是通过测试，并设置模板值为1
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE); // 操作为替换
    glClear(GL_STENCIL_BUFFER_BIT); // 清除模板缓冲区

    // 绘制圆形掩码
    glStencilMask(0xFF); // 允许写入模板缓冲区

    drawCircleMask(0.0f, 0.0f, 1.0f); // 绘制圆形掩码

    // 绘制内容
    glStencilMask(0x00); // 不再写入模板缓冲区
    glStencilFunc(GL_EQUAL, 1, 0xFF); // 只在模板值为1的区域绘制

    // glClearColor时,好像模板测试(GL_STENCIL_TEST)没有效果
    // 手动绘制矩形
    /*
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();
    */
    drawContent();

    // drawCircleMask(0.0f, 0.0f, 1.0f); // 绘制圆形掩码

    glDisable(GL_STENCIL_TEST); // 启用模板测试
    glDisable(GL_BLEND); // 启用模板测试
}

void RadarWidget::LoadVBO2(const QString& filename, FileVBOData_t& data)
{
    QByteArray u8str = filename.toUtf8();
    FILE* fp = fopen(u8str.constData(), "rb");

    memset(&data, sizeof(data), 0);
    fread(&data.header, sizeof(data.header), 1, fp);
    data.structures = new FileVBOStructure_t[data.header.numStructures];
    memset(data.structures, 0, sizeof(FileVBOStructure_t) * data.header.numStructures);
    for (size_t i = 0; i < data.header.numStructures; i++)
    {
        auto& s = data.structures[i];
        fread(&s.header, sizeof(s.header), 1, fp);
        if (s.header.numbitmappatches > 0)
        {
            s.bitmaps = new FileVBOBitmap_t[s.header.numbitmappatches];
            memset(s.bitmaps, 0, sizeof(FileVBOBitmap_t) * s.header.numbitmappatches);
        }
        for (size_t j = 0; j < s.header.numbitmappatches; j++)
        {
            auto& b = s.bitmaps[j];
            fread(&b.header, sizeof(b.header), 1, fp);
            if (b.header.noNormals > 0)
                b.vsize = b.header.numtriangles * 3 * 5 * 4;
            else
                b.vsize = b.header.numtriangles * 3 * 8 * 4;
            b.vertexData = new float[b.vsize / 4];
            fread(b.vertexData, b.vsize, 1, fp);
        }

        fread(&s.colorpatch.header, sizeof(s.colorpatch.header), 1, fp);
        if (s.header.numcolorpatches > 0)
        {
            if (s.colorpatch.header.noColors > 0)
            {
                if (s.colorpatch.header.noNormals > 0)
                    s.colorpatch.vsize = s.colorpatch.header.numtriangles * 3 * 3 * 4;
                else
                    s.colorpatch.vsize = s.colorpatch.header.numtriangles * 3 * 6 * 4;
            }
            else
            {
                if (s.colorpatch.header.noNormals > 0)
                    s.colorpatch.vsize = s.colorpatch.header.numtriangles * 3 * 7 * 4;
                else
                    s.colorpatch.vsize = s.colorpatch.header.numtriangles * 3 * 10 * 4;
            }
        }
        s.colorpatch.vertexData = new float[s.colorpatch.vsize / 4];
        auto ttt = fread(s.colorpatch.vertexData, 1, s.colorpatch.vsize, fp);
        int stop = 1;
    }

    fclose(fp);
}

void RadarWidget::FillGLBuffer()
{
    m_vertexBuffer_terrainvboc.create();
    m_vertexBuffer_terrainvboc.bind();
    m_vertexBuffer_terrainvboc.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer_terrainvboc.allocate(
        m_terrainvboc.structures[0].colorpatch.vertexData, m_terrainvboc.structures[0].colorpatch.vsize);
    m_vertexBuffer_terrainvboc.release();
    // glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer_terrainvboc.bufferId());
    // glBufferData(GL_ARRAY_BUFFER, m_terrainvboc.structures[0].colorpatch.vsize, 
    //     m_terrainvboc.structures[0].colorpatch.vertexData, GL_STATIC_DRAW);


    /*
    m_vertexBuffer_terrainvbom.create();
    m_vertexBuffer_terrainvbom.bind();
    m_vertexBuffer_terrainvbom.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer_terrainvbom.allocate(
        m_terrainvbom.structures[0].colorpatch.vertexData, m_terrainvboc.structures[0].colorpatch.vsize);
        */

}

void RadarWidget::initOffscreenTexture()
{
    QOpenGLTexture* texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    
    texture->create();
    texture->bind();

    // 设置纹理参数
    texture->setMinificationFilter(QOpenGLTexture::Linear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);
    texture->setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
    texture->setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToEdge);

    m_screentexture = texture;

    m_screentexture->release();
}

void RadarWidget::createPolarVBO(float r0, float r1, int n, int m, float xcentre, float ycentre, float power)
{
    int numtriangles = n * m * 2;
    float A1 = (r1 - r0) / pow(n, power);

    float dr = r1 - r0;
    float da = 2.0 * M_PI / m;

    QVector<float> vertexData;
    vertexData.resize(numtriangles * (3 + 3 + 2) * 3); // 顶点数据
    int count = 0;
    int tricount = 0;

    for (int i = 0; i < n; i++)
    {
        float rmin = r0 + A1 * pow(i, power);
        float rmax = r0 + A1 * pow(i + 1, power);

        for (int j = 0; j < m; j++)
        {
            float a0 = da * j;
            float a1 = da * (j + 1);
            float smin = sin(a0);
            float cmin = cos(a0);
            float smax = sin(a1);
            float cmax = cos(a1);

            // 四个顶点
            float x0 = xcentre + rmin * cmin;
            float y0 = ycentre + rmin * smin;
            float x1 = xcentre + rmax * cmin;
            float y1 = ycentre + rmax * smin;
            float x2 = xcentre + rmax * cmax;
            float y2 = ycentre + rmax * smax;
            float x3 = xcentre + rmin * cmax;
            float y3 = ycentre + rmin * smax;

            // 四个纹理坐标
            float u0 = (x0 - xcentre) / r1;
            float v0 = (y0 - ycentre) / r1;
            float u1 = (x1 - xcentre) / r1;
            float v1 = (y1 - ycentre) / r1;
            float u2 = (x2 - xcentre) / r1;
            float v2 = (y2 - ycentre) / r1;
            float u3 = (x3 - xcentre) / r1;
            float v3 = (y3 - ycentre) / r1;

            // 存储两个三角形
            // 顶点
            vertexData[count++] = x0;
            vertexData[count++] = y0;
            vertexData[count++] = 0.0;
            // 法线
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 1.0;
            // 纹理坐标
            vertexData[count++] = u0;
            vertexData[count++] = v0;

            vertexData[count++] = x1;
            vertexData[count++] = y1;
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 1.0;
            vertexData[count++] = u1;
            vertexData[count++] = v1;

            vertexData[count++] = x2;
            vertexData[count++] = y2;
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 1.0;
            vertexData[count++] = u2;
            vertexData[count++] = v2;

            tricount++;

            // 第二个三角形
            vertexData[count++] = x2;
            vertexData[count++] = y2;
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 1.0;
            vertexData[count++] = u2;
            vertexData[count++] = v2;

            vertexData[count++] = x3;
            vertexData[count++] = y3;
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 1.0;
            vertexData[count++] = u3;
            vertexData[count++] = v3;

            vertexData[count++] = x0;
            vertexData[count++] = y0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 0.0;
            vertexData[count++] = 1.0;
            vertexData[count++] = u0;
            vertexData[count++] = v0;

            tricount++;
        }
    }

    m_wavesNumOfTri = numtriangles;
    // 创建和绑定VBO
    m_wavesvbo.create();
    m_wavesvbo.bind();
    m_wavesvbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_wavesvbo.allocate(vertexData.data(), vertexData.size() * sizeof(float));
    m_wavesvbo.release();
}

void RadarWidget::drawCircleMask(float x, float y, float radius)
{
    const int vertexCount = 360; // 圆形的顶点数
    // glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y); // 圆心
    for (int i = 0; i <= vertexCount; ++i)
    {
        float angle = i * M_PI / 180.0;
        glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
    }
    glEnd();
}

void RadarWidget::drawContent()
{
    // qDebug() << m_frameCount;
    /*
    QOpenGLDebugLogger logger;
    logger.initialize();

    logger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    connect(&logger, &QOpenGLDebugLogger::messageLogged, this, [](const QOpenGLDebugMessage& message) {
        qWarning() << "OpenGL Debug Message:" << message.message();
        });
    // 开始捕获调试消息
    logger.enableMessages();
    */

    /*
    //打开抗锯齿功能
//1.开启混合功能
    glEnable(GL_BLEND);

    //2.指定混合因子
    //注意:如果你修改了混合方程式,当你使用混合抗锯齿功能时,请一定要改为默认混合方程式
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //3.开启对点\线\多边形的抗锯齿功能
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    */


    // glClear(GL_COLOR_BUFFER_BIT);

    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // 在 OpenGL 上下文中注册回调函数
    // glDebugMessageCallback(OpenGLDebugOutput, nullptr);

    // glUseProgram(m_shaderProgram.programId());
#if 1

    m_frameCount++;

    m_shaderProgram.bind();
    // m_tstprogram.bind();

    float motion_dx = 0.0f;
    float motion_dy = 0.0f;

    if (m_data->m_ppi.m_motion == MOTION_TRUE)
    {
        motion_dx = m_data->m_ppi.m_motion_startx - m_data->m_ship.m_x;
        motion_dy = m_data->m_ppi.m_motion_starty - m_data->m_ship.m_y;
    }

    if (m_data->m_ppi.m_motion == MOTION_TRUE && m_frameCount % 20 == 0)
    {
        float r = sqrtf(motion_dx * motion_dx + motion_dy * motion_dy);
        if (r < m_data->m_ppi.m_range * 0.6666667f)
        { 
        }
        else
        {
            m_data->m_ppi.m_motion_startx = m_data->m_ship.m_x;
            m_data->m_ppi.m_motion_starty = m_data->m_ship.m_y;
        }
    }

    if (m_frameCount % 5 == 0)
    {
        m_data->m_ppi.updateTransform(m_data->m_ship);
    }

    float angle = m_data->m_angle;
    // float dangle = m_data->m_ppi.m_beam_width;
    float dangle = 360.0f / 10.0f / 1000.0f * 50.0f;
    float waveheight = 1.0f;
    float winddirection = 315.0f;

    angle += dangle;
    if (angle >= 360.0f)
        angle = 0.0f;

    m_data->m_angle = angle;

    m_shaderProgram.setUniformValue(m_shaderLocation.directionalColor, 1.0f, 1.0f, 0.0f);
    m_shaderProgram.setUniformValue(m_shaderLocation.lightPosition, 
        m_data->m_ship.m_x, m_data->m_ship.m_y, m_data->m_ship.m_antennaHeight);
    m_shaderProgram.setUniformValue(m_shaderLocation.angle0, angle);
    m_shaderProgram.setUniformValue(m_shaderLocation.angle1, angle + dangle);
    m_shaderProgram.setUniformValue(m_shaderLocation.antennaHeight, m_data->m_ship.m_antennaHeight);
    m_shaderProgram.setUniformValue(m_shaderLocation.beamHeight2, (float)(m_data->m_ppi.m_beam_height * 0.5f * M_PI / 180.0f));
    m_shaderProgram.setUniformValue(m_shaderLocation.fRange, m_data->m_ppi.m_range);
    m_shaderProgram.setUniformValue(m_shaderLocation.fBrilliance, m_data->m_ppi.m_brilliance);

    // m_ppi.m_gain = 1.0;
    m_shaderProgram.setUniformValue(m_shaderLocation.fGain, m_data->m_ppi.m_gain);
    m_shaderProgram.setUniformValue(m_shaderLocation.fTune, m_data->m_ppi.m_tune);
    m_shaderProgram.setUniformValue(m_shaderLocation.fSea, m_data->m_ppi.m_sea);
    m_shaderProgram.setUniformValue(m_shaderLocation.fRain, m_data->m_ppi.m_rain);
    m_shaderProgram.setUniformValue(m_shaderLocation.fFTC, m_data->m_ppi.m_FTC);
    m_shaderProgram.setUniformValue(m_shaderLocation.fWaveHeight, waveheight);
    m_shaderProgram.setUniformValue(m_shaderLocation.fWindDirection, winddirection);

    //QOpenGLFunctions* glFuncs = QOpenGLContext::currentContext()->functions();

    // gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    // glViewport(0, 0, 512, 512);
    // glFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // glMatrixMode(GL_PROJECTION); // 设置当前矩阵为投影矩阵
    // glLoadIdentity(); // 重置投影矩阵为单位矩阵
    m_pMatrix.setToIdentity();
    float scale = 1.00f;
    m_pMatrix.ortho(-m_data->m_ppi.m_range * scale, m_data->m_ppi.m_range * scale, 
        -m_data->m_ppi.m_range * scale, m_data->m_ppi.m_range * scale, 1.0f, m_data->m_ppi.m_range + 100.0f);

    glEnable(GL_DEPTH_TEST);

    //===== draw terrain ===========================
    m_shaderProgram.setUniformValue(m_shaderLocation.flagLocation, 0);
    m_shaderProgram.setUniformValue(m_shaderLocation.flag1Location, 0);

    float rotangle = 0.0f;
    // if (m_ppi.m_motion == MOTION_RELATIVE)
    {
        if (m_data->m_ppi.m_headingMode == HMODE_HUP)
        {
            rotangle = m_data->m_ship.m_heading;
        }
        else if (m_data->m_ppi.m_headingMode == HMODE_CUP)
        {
            rotangle = m_data->m_ship.m_course;
        }
    }
    m_shaderProgram.setUniformValue(m_shaderLocation.fRotAngle, rotangle);

    m_mvMatrix.setToIdentity();

    // 将角度从度转换为弧度并绕 Z 轴旋转
    m_mvMatrix.rotate(M_PI * rotangle / 180.0, 0, 0, 1);
    // 平移模型视图矩阵
    m_mvMatrix.translate(-m_data->m_ship.m_x - motion_dx, -m_data->m_ship.m_y - motion_dy,
        -m_data->m_ppi.m_range * 0.9);
    /*
    m_mvMatrix.setRow(0, QVector4D(1, 0, 0, 0));
    m_mvMatrix.setRow(1, QVector4D(0, 1, 0, 0));
    m_mvMatrix.setRow(2, QVector4D(0, 0, 1, 0));
    m_mvMatrix.setRow(3, QVector4D(-35950.1484375, 31134.2421875, -6667.2001953125, 1));

    m_pMatrix.setRow(0, QVector4D(0.0001349891972495243, 0, 0, 0));
    m_pMatrix.setRow(1, QVector4D(0, 0.0001349891972495243, 0, 0));
    m_pMatrix.setRow(2, QVector4D(0, 0, -0.00026641800650395453, 0));
    m_pMatrix.setRow(3, QVector4D(-0, -0, -1.000266432762146, 1));

    m_mvMatrix.setRow(0, QVector4D(1, 0, 0, 0));
    m_mvMatrix.setRow(1, QVector4D(0, 1, 0, 0));
    m_mvMatrix.setRow(2, QVector4D(0, 0, 1, 0));
    m_mvMatrix.setRow(3, QVector4D(-35950.1484375, 31134.2421875, -6667.2001953125, 1));
    */

    // 从 4x4 矩阵中提取法向量矩阵
    m_nMatrix = m_mvMatrix.normalMatrix();
    /*
    float value_arr[] = {
        -0.9993908405303955, 0.03489949554204941, 0,
        -0.03489949554204941, -0.9993908405303955, 0,
        0, -0, 1
    };
    m_nMatrix = QMatrix3x3(value_arr);
    */

    // drawTerrain();
    drawHeightMap();
    // m_vbo.bind();
    // GLint posAttr = m_tstprogram.attributeLocation("a_Position");
    // m_tstprogram.enableAttributeArray(posAttr);
    // m_tstprogram.setAttributeBuffer(posAttr, GL_FLOAT, 0, 3, sizeof(float) * 3);
    // 绘制三角形
    // glDrawArrays(GL_TRIANGLES, 0, 3);
    // m_tstprogram.disableAttributeArray(posAttr);
    // m_vbo.release();



    // draw targets
    m_shaderProgram.setUniformValue(m_shaderLocation.flagLocation, 0);
    m_shaderProgram.setUniformValue(m_shaderLocation.flag1Location, 3);

    /*
    for (size_t i = 0; i < length; i++)
    {

    }
    */


    m_shaderProgram.setUniformValue(m_shaderLocation.lightPosition, 0.0f, 0.0f, m_data->m_ship.m_antennaHeight);
    m_shaderProgram.setUniformValue(m_shaderLocation.flagLocation, 2);
    m_shaderProgram.setUniformValue(m_shaderLocation.flag1Location, 2);

    //===== draw water around the ship =============
    m_mvMatrix.setToIdentity();
    m_mvMatrix.rotate(M_PI* rotangle / 180.0, 0, 0, 1);
    m_mvMatrix.translate(- motion_dx, - motion_dy, -m_data->m_ppi.m_range * 0.9 - 1.0);
    m_nMatrix = m_mvMatrix.normalMatrix();

    drawWater();

    //===== draw offscreen bitmap on invisible canvas =
    // not very often - this is slow
    if (m_frameCount % 10 == 0)
    {
        QPainter painter(&m_screenImage);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillRect(m_screenImage.rect(), QColor(0, 0, 0, 255));
        // painter.fillRect(m_screenImage.rect(), QColor(255, 0, 0, 255));

        m_data->m_ppi.PPIDrawScale(painter, m_data->m_ship);

        if (m_data->m_ppi.m_rangeRings == RANGRINGS_ON)
        {
            m_data->m_ppi.PPIDrawRings(painter, m_data->m_ship, m_data->numOfRings());
        }

        if (m_data->m_ppi.m_headline == HEADINGLINE_ON)
        {
            m_data->m_ppi.PPIDrawHeadingLine(painter, m_data->m_ship);
        }

        m_data->m_ppi.PPIDrawCursor(painter);

        m_data->m_ppi.PPIDrawEBLVRM(painter, m_data->m_ship);

    }

    // draw offscreen bitmap
    m_shaderProgram.setUniformValue(m_shaderLocation.flagLocation, 1);
    m_shaderProgram.setUniformValue(m_shaderLocation.flag1Location, 1);

    m_pMatrix.setToIdentity();
    scale = 1.00f;
    m_pMatrix.ortho(-m_data->m_ppi.m_range * scale, m_data->m_ppi.m_range * scale,
        -m_data->m_ppi.m_range * scale, m_data->m_ppi.m_range * scale,
        1.0, m_data->m_ppi.m_range + 100.0);

    m_mvMatrix.setToIdentity();
    m_mvMatrix.translate(0, 0, -m_data->m_ppi.m_range * 0.9);
    m_nMatrix = m_mvMatrix.normalMatrix();

#if 1
    QOpenGLBuffer squareVertexPositionBuffer2;
    squareVertexPositionBuffer2.create();
    squareVertexPositionBuffer2.bind();
    squareVertexPositionBuffer2.setUsagePattern(QOpenGLBuffer::StaticDraw);
    auto bbbid = squareVertexPositionBuffer2.bufferId();

    float vertices[] = {
        -m_data->m_ppi.m_range, -m_data->m_ppi.m_range, 0.0f, 0.0f, 0.0f,
        +m_data->m_ppi.m_range, -m_data->m_ppi.m_range, 0.0f, 1.0f, 0.0f,
        +m_data->m_ppi.m_range, +m_data->m_ppi.m_range, 0.0f, 1.0f, 1.0f,
        +m_data->m_ppi.m_range, +m_data->m_ppi.m_range, 0.0f, 1.0f, 1.0f,
        -m_data->m_ppi.m_range, +m_data->m_ppi.m_range, 0.0f, 0.0f, 1.0f,
        -m_data->m_ppi.m_range, -m_data->m_ppi.m_range, 0.0f, 0.0f, 0.0f

        /*
        -0.9f, -0.9f, 0.0f, 0.0f, 0.0f,
        +0.9f, -0.9f, 0.0f, 1.0f, 0.0f,
        +0.9f, +0.9f, 0.0f, 1.0f, 1.0f,
        -0.9f, +0.9f, 0.0f, 0.0f, 1.0f
        */
    };
    squareVertexPositionBuffer2.allocate(vertices, sizeof(vertices));

    // m_screentexture->setData(m_screenImage, QOpenGLTexture::DontGenerateMipMaps);
    // m_screentexture->bind();
    // QOpenGLTexture st(m_screenImage);
    QOpenGLTexture st(QOpenGLTexture::Target2D);
    st.setMinificationFilter(QOpenGLTexture::Linear);
    st.setMagnificationFilter(QOpenGLTexture::Linear);
    st.setWrapMode(QOpenGLTexture::DirectionS, QOpenGLTexture::ClampToEdge);
    st.setWrapMode(QOpenGLTexture::DirectionT, QOpenGLTexture::ClampToEdge);
    st.setData(m_screenImage.mirrored());
    st.bind(0);
    m_shaderProgram.setUniformValue(m_shaderLocation.uSamplerUniform, 0);

    m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexPositionAttribute);
    // m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexNormalAttribute);
    m_shaderProgram.enableAttributeArray(m_shaderLocation.textureCoordAttribute);


    /*
    float vertices[] = {
        -0.9f, -0.9f, 0.0f,
        +0.9f, -0.9f, 0.0f,
        +0.9f, +0.9f, 0.0f,
        -0.9f, +0.9f, 0.0f
    };

    float vertices1[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    */

    m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexPositionAttribute, GL_FLOAT, 0, 3, 20);
    m_shaderProgram.setAttributeBuffer(m_shaderLocation.textureCoordAttribute, GL_FLOAT, 12, 2, 20);
    // m_shaderProgram.setAttributeArray(m_shaderLocation.vertexPositionAttribute, vertices, 3, 3*4);
    // m_shaderProgram.setAttributeArray(m_shaderLocation.textureCoordAttribute, vertices, 2, 2*4);

    m_shaderProgram.setUniformValue(m_shaderLocation.pMatrixUniform, m_pMatrix);
    m_shaderProgram.setUniformValue(m_shaderLocation.mvMatrixUniform, m_mvMatrix);
    m_shaderProgram.setUniformValue(m_shaderLocation.nMatrixUniform, m_nMatrix);

    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // glDrawArrays(GL_QUADS, 0, 4);

    m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexPositionAttribute);
    m_shaderProgram.disableAttributeArray(m_shaderLocation.textureCoordAttribute);
#endif

    m_screentexture->release();
    m_shaderProgram.release();

#endif
    /*
    m_tstprogram.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_vbo.bind();
    // 绘制三角形
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_vbo.release();
    m_tstprogram.release();
    8?

    /*
    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6); // 绘制六边形
    m_vao.release();
    */

}

void RadarWidget::drawTerrain()
{
	m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexPositionAttribute);
	m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexNormalAttribute);

	m_terrainVbo.bind();

	m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexPositionAttribute, GL_FLOAT, 0, 3, sizeof(QVector3D)*2);
	// m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexNormalAttribute, GL_FLOAT, 12, 3, 24);
	m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexNormalAttribute, GL_FLOAT, 12, 3, sizeof(QVector3D) * 2);

	m_shaderProgram.setUniformValue(m_shaderLocation.pMatrixUniform, m_pMatrix);
	m_shaderProgram.setUniformValue(m_shaderLocation.mvMatrixUniform, m_mvMatrix);
	m_shaderProgram.setUniformValue(m_shaderLocation.nMatrixUniform, m_nMatrix);

	glDrawArrays(GL_TRIANGLES, 0, m_numOfVert);
	m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexPositionAttribute);
	m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexNormalAttribute);

    m_terrainVbo.release();

    /*
    for (size_t i = 0; i < m_terrainvboc.header.numStructures; i++)
    {
        auto& s = m_terrainvboc.structures[i];
        if (s.colorpatch.header.numtriangles > 0)
        {
            if (s.colorpatch.header.noColors > 0)
            {
                m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexPositionAttribute);
                m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexNormalAttribute);
                // m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexColorAttribute);
                // m_shaderProgram.enableAttributeArray(m_shaderLocation.textureCoordAttribute);

                m_vertexBuffer_terrainvboc.bind();
                auto bid = m_vertexBuffer_terrainvboc.bufferId();
                auto sid = m_shaderProgram.programId();
                // auto vid1 = m_vertexShader->shaderId();
                // auto vid2 = m_fragmentShader->shaderId();

                // glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer_terrainvboc.bufferId());
                m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexPositionAttribute, GL_FLOAT, 0, 3, 24);
                m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexNormalAttribute, GL_FLOAT, 12, 3, 24);

                // int offset1 = 0;
                // glVertexAttribPointer(m_shaderLocation.vertexPositionAttribute, 3, GL_FLOAT, GL_FALSE, 24, reinterpret_cast<const void*>(qintptr(offset1)));
                // int offset2 = 12;
                // glVertexAttribPointer(m_shaderLocation.vertexNormalAttribute, 3, GL_FLOAT, GL_FALSE, 24, reinterpret_cast<const void*>(qintptr(offset2)));

                m_shaderProgram.setUniformValue(m_shaderLocation.pMatrixUniform, m_pMatrix);
                m_shaderProgram.setUniformValue(m_shaderLocation.mvMatrixUniform, m_mvMatrix);
                m_shaderProgram.setUniformValue(m_shaderLocation.nMatrixUniform, m_nMatrix);

                glDrawArrays(GL_TRIANGLES, 0, s.colorpatch.header.numtriangles * 3);
                m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexPositionAttribute);
                m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexNormalAttribute);

                // m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexPositionAttribute);
                // m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexNormalAttribute);
                m_vertexBuffer_terrainvboc.release();
            }
        }
    }
    */
}

void RadarWidget::drawWater()
{
    if (!m_wavesvbo.isCreated())
        return;


    m_shaderProgram.setUniformValue(m_shaderLocation.pMatrixUniform, m_pMatrix);
    m_shaderProgram.setUniformValue(m_shaderLocation.mvMatrixUniform, m_mvMatrix);
    m_shaderProgram.setUniformValue(m_shaderLocation.nMatrixUniform, m_nMatrix);

    bool drawnormal = true;
    bool drawtexture = false;

    m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexPositionAttribute);
    m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexNormalAttribute);

    m_wavesvbo.bind();
    m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexPositionAttribute, GL_FLOAT, 0, 3, 32);
    m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexNormalAttribute, GL_FLOAT, 12, 3, 32);
    glDrawArrays(GL_TRIANGLES, 0, m_wavesNumOfTri * 3);

    m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexPositionAttribute);
    m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexNormalAttribute);


    m_wavesvbo.release();

}

void RadarWidget::drawHeightMap()
{

    // draw offscreen bitmap
    m_shaderProgram.setUniformValue(m_shaderLocation.flagLocation, 4);
    m_shaderProgram.setUniformValue(m_shaderLocation.flag1Location, 4);

    /*
    m_pMatrix.setToIdentity();
    float scale = 1.00f;
    m_pMatrix.ortho(-m_data->m_ppi.m_range * scale, m_data->m_ppi.m_range * scale,
        -m_data->m_ppi.m_range * scale, m_data->m_ppi.m_range * scale,
        1.0, m_data->m_ppi.m_range + 100.0);

    m_mvMatrix.setToIdentity();
    m_mvMatrix.translate(0, 0, -m_data->m_ppi.m_range * 0.9);
    m_nMatrix = m_mvMatrix.normalMatrix();
	*/

    QOpenGLBuffer squareVertexPositionBuffer2;
    squareVertexPositionBuffer2.create();
    squareVertexPositionBuffer2.bind();
    squareVertexPositionBuffer2.setUsagePattern(QOpenGLBuffer::StaticDraw);

    float vertices[] = {
        /*
        -m_data->m_ppi.m_range, -m_data->m_ppi.m_range, 0.0f, 0.0f, 0.0f,
        +m_data->m_ppi.m_range, -m_data->m_ppi.m_range, 0.0f, 1.0f, 0.0f,
        +m_data->m_ppi.m_range, +m_data->m_ppi.m_range, 0.0f, 1.0f, 1.0f,
        +m_data->m_ppi.m_range, +m_data->m_ppi.m_range, 0.0f, 1.0f, 1.0f,
        -m_data->m_ppi.m_range, +m_data->m_ppi.m_range, 0.0f, 0.0f, 1.0f,
        -m_data->m_ppi.m_range, -m_data->m_ppi.m_range, 0.0f, 0.0f, 0.0f
        */
        m_minx, m_miny, 0.0f, 0.0f, 0.0f,
        m_maxx, m_miny, 0.0f, 1.0f, 0.0f,
        m_maxx, m_maxy, 0.0f, 1.0f, 1.0f,
        m_maxx, m_maxy, 0.0f, 1.0f, 1.0f,
        m_minx, m_maxy, 0.0f, 0.0f, 1.0f,
        m_minx, m_miny, 0.0f, 0.0f, 0.0f
    };
    squareVertexPositionBuffer2.allocate(vertices, sizeof(vertices));

    m_heightTexture->bind(1);
    m_shaderProgram.setUniformValue(m_shaderLocation.uHeigthMapUniform, 1);

    m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexPositionAttribute);
    m_shaderProgram.enableAttributeArray(m_shaderLocation.heightTextureCoordAttribute);

    m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexPositionAttribute, GL_FLOAT, 0, 3, 20);
    m_shaderProgram.setAttributeBuffer(m_shaderLocation.heightTextureCoordAttribute, GL_FLOAT, 12, 2, 20);

    // minx ---> 0.0
    // maxx ---> 1.0
    float coordx = (m_data->m_ship.m_x - m_minx) / (m_maxx - m_minx);
    coordx = qMax(coordx, 0.0f);
    coordx = qMin(coordx, 1.0f);
    // miny --> 0.0
    // maxy --> 1.0
    float coordy = (m_data->m_ship.m_y - m_miny) / (m_maxy - m_miny);
    coordy = qMax(coordy, 0.0f);
    coordy = qMin(coordy, 1.0f);
    QVector2D shipcoord{ coordx, coordy };
    m_shaderProgram.setUniformValueArray(m_shaderLocation.shipCoordUniform, &shipcoord, 1);

    m_shaderProgram.setUniformValue(m_shaderLocation.pMatrixUniform, m_pMatrix);
    m_shaderProgram.setUniformValue(m_shaderLocation.mvMatrixUniform, m_mvMatrix);
    m_shaderProgram.setUniformValue(m_shaderLocation.nMatrixUniform, m_nMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexPositionAttribute);
    m_shaderProgram.disableAttributeArray(m_shaderLocation.heightTextureCoordAttribute);

    m_heightTexture->release();
}

void RadarWidget::readDEMData(const QString& filepath)
{
    std::string filename = filepath.toStdString();
    FILE* fp = fopen(filename.c_str(), "rb");

    demdata_header_t header;
    // in.read(reinterpret_cast<char*>(&header), sizeof(header));
    fread(&header, sizeof(header), 1, fp);

    // int16_t* data_mat = new int16_t[header.nrows * header.ncols];
    // in.read((char*)data_mat, header.nrows * header.ncols);

    int16_t* data_cols = new int16_t[header.ncols];
    float xmin = header.xmin;
    float ymin = header.ymin;
    float xdim = header.xdim;
    float ydim = header.ydim;

    QVector<QVector3D> vertices; // x,y坐标 + 颜色高度值
    // m_terrainVertices.clear();
    for (int i = 0; i < header.nrows; i++)
    {
        // in.read((char*)data_cols, header.ncols*sizeof(char));
        fread(data_cols, sizeof(int16_t) * header.ncols, 1, fp);
        for (int j = 0; j < header.ncols; j++)
        {
            float h = data_cols[j];
            float lat = ymin + i * ydim;
            float lon = xmin + j * xdim;

            float halfStep = xdim / 2.0f;

            // 四边形的四个角点
            float latMin = lat - halfStep;
            float latMax = lat + halfStep;
            float lonMin = lon - halfStep;
            float lonMax = lon + halfStep;

            // 转换为OpenGL坐标（假设已映射到合适的范围）
            float x1 = lonMin; // 经度对应x轴
            float y1 = latMin; // 纬度对应y轴
            float x2 = lonMax;
            float y2 = latMax;

            // 添加两个三角形组成四边形
            vertices.push_back({ x1, y1, h });
            vertices.push_back({ x2, y1, h });
            vertices.push_back({ x2, y2, h });

            vertices.push_back({ x1, y1, h });
            vertices.push_back({ x2, y2, h });
            vertices.push_back({ x1, y2, h });

            /*
            m_terrainVertices.push_back({ x1, y1, h });
            m_terrainVertices.push_back({ x2, y1, h });
            m_terrainVertices.push_back({ x2, y2, h });

            m_terrainVertices.push_back({ x1, y1, h });
            m_terrainVertices.push_back({ x2, y2, h });
            m_terrainVertices.push_back({ x1, y2, h });
            */

            /*
            m_vertices.push_back({ x1, y1, h });
            m_vertices.push_back({ x1, y2, h });
            m_vertices.push_back({ x2, y2, h });
            m_vertices.push_back({ x2, y1, h });
            */
            // m_vertices.push_back({ lon, lat, h });

        }
    }

	m_terrainVbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_terrainVbo.create();
	m_terrainVbo.bind();
	m_terrainVbo.allocate(vertices.constData(), vertices.size() * sizeof(QVector3D));
	m_terrainVbo.release();

    m_numOfVert = vertices.size();

    delete data_cols;
    fclose(fp);
}
