#define _CRT_SECURE_NO_WARNINGS
#include "radarwidget.h"

#include <QDebug>
#include <QFile>
#include <QOpenGLDebugLogger>
#include <QOpenGLContext>

#include <math.h>
#ifndef _PI
#define _PI 3.1415926535897
#endif


RadarWidget::RadarWidget()
{
    // setSurfaceType(QOpenGLWindow::OpenGLSurface);

    QSurfaceFormat format;
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setOption(QSurfaceFormat::DebugContext);
    QSurfaceFormat::setDefaultFormat(format);
    setFormat(format);

    m_fragmentShader = nullptr;
    m_vertexShader = nullptr;
    m_screentexture = nullptr;
}

RadarWidget::~RadarWidget()
{
    makeCurrent();
    m_shaderProgram.release();
    m_vertexBuffer.destroy();
    doneCurrent();
}

void RadarWidget::initializeGL()
{
    initializeOpenGLFunctions();

    QOpenGLDebugLogger logger;
    logger.initialize();

    logger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    connect(&logger, &QOpenGLDebugLogger::messageLogged, this, [](const QOpenGLDebugMessage& message) {
        qWarning() << "OpenGL Debug Message:" << message.message();
        });
    // 开始捕获调试消息
    logger.enableMessages();

    QString fs_shader_file = "./data/shader/shader-fs.shader";
    QString vs_shader_file = "./data/shader/shader-vs.shader";

    m_fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment);
    if (!m_fragmentShader->compileSourceFile(fs_shader_file))
    {
        qDebug() << "fragment shader err:" << m_fragmentShader->log();
    }

    m_vertexShader = new QOpenGLShader(QOpenGLShader::Vertex);
    if (!m_vertexShader->compileSourceFile(vs_shader_file))
    {
        qDebug() << "vertex shader err:" << m_vertexShader->log();
    }
    m_shaderProgram.addShader(m_vertexShader);
    m_shaderProgram.addShader(m_fragmentShader);

    if (!m_shaderProgram.link()) {
        qDebug() << "ERROR:" << m_shaderProgram.log();
    }

    m_shaderLocation.vertexPositionAttribute = m_shaderProgram.attributeLocation( "aVertexPosition");
    m_shaderLocation.vertexNormalAttribute = m_shaderProgram.attributeLocation( "aVertexNormal");
    m_shaderLocation.textureCoordAttribute = m_shaderProgram.attributeLocation( "aTextureCoord");
    m_shaderLocation.vertexColorAttribute = m_shaderProgram.attributeLocation( "aVertexColor");

    m_shaderLocation.pMatrixUniform = m_shaderProgram.uniformLocation( "uPMatrix");
    m_shaderLocation.mvMatrixUniform = m_shaderProgram.uniformLocation( "uMVMatrix");
    m_shaderLocation.nMatrixUniform = m_shaderProgram.uniformLocation( "uNMatrix");

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

    m_geo.init(-23, -44);

    LoadVBO2("data/areas/xiamenc.vbo2", m_terrainvboc);
    LoadVBO2("data/areas/xiamenm.vbo2", m_terrainvbom);
    // FillGLBuffer();
    // initOffscreenTexture();

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
}

void RadarWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}


void RadarWidget::paintGL()
{

    QOpenGLDebugLogger logger;
    logger.initialize();

    logger.startLogging(QOpenGLDebugLogger::SynchronousLogging);
    connect(&logger, &QOpenGLDebugLogger::messageLogged, this, [](const QOpenGLDebugMessage& message) {
        qWarning() << "OpenGL Debug Message:" << message.message();
        });
    // 开始捕获调试消息
    logger.enableMessages();

    glClear(GL_COLOR_BUFFER_BIT);

    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // 在 OpenGL 上下文中注册回调函数
    // glDebugMessageCallback(OpenGLDebugOutput, nullptr);

    // glUseProgram(m_shaderProgram.programId());
#if 0
    m_shaderProgram.bind();

    float angle = 0.0;
    float dangle = m_ppi.m_beam_width;
    float waveheight = 1.0f;
    float winddirection = 315.0f;

    m_shaderProgram.setUniformValue(m_shaderLocation.directionalColor, 1.0f, 1.0f, 0.0f);
    m_shaderProgram.setUniformValue(m_shaderLocation.lightPosition, m_ship.m_x, m_ship.m_y, m_ship.m_antennaHeight);
    m_shaderProgram.setUniformValue(m_shaderLocation.angle0, angle);
    m_shaderProgram.setUniformValue(m_shaderLocation.angle1, angle + dangle);
    m_shaderProgram.setUniformValue(m_shaderLocation.antennaHeight, m_ship.m_antennaHeight);
    m_shaderProgram.setUniformValue(m_shaderLocation.beamHeight2, (float)(m_ppi.m_beam_height * 0.5f * _PI / 180.0));
    m_shaderProgram.setUniformValue(m_shaderLocation.fRange, m_ppi.m_range);
    m_shaderProgram.setUniformValue(m_shaderLocation.fBrilliance, m_ppi.m_brilliance);
    m_shaderProgram.setUniformValue(m_shaderLocation.fGain, m_ppi.m_gain);
    m_shaderProgram.setUniformValue(m_shaderLocation.fTune, m_ppi.m_tune);
    m_shaderProgram.setUniformValue(m_shaderLocation.fSea, m_ppi.m_sea);
    m_shaderProgram.setUniformValue(m_shaderLocation.fRain, m_ppi.m_rain);
    m_shaderProgram.setUniformValue(m_shaderLocation.fFTC, m_ppi.m_FTC);
    m_shaderProgram.setUniformValue(m_shaderLocation.fWaveHeight, waveheight);
    m_shaderProgram.setUniformValue(m_shaderLocation.fWindDirection, winddirection);

    //QOpenGLFunctions* glFuncs = QOpenGLContext::currentContext()->functions();

    // gl.viewport(0, 0, gl.viewportWidth, gl.viewportHeight);
    // glViewport(0, 0, 512, 512);
    // glFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // glMatrixMode(GL_PROJECTION); // 设置当前矩阵为投影矩阵
    // glLoadIdentity(); // 重置投影矩阵为单位矩阵
    m_pMatrix.ortho(-m_ppi.m_range, m_ppi.m_range, -m_ppi.m_range, m_ppi.m_range, 1.0, m_ppi.m_range + 100.0);

    glEnable(GL_DEPTH_TEST);

    //===== draw terrain ===========================
    m_shaderProgram.setUniformValue(m_shaderLocation.flagLocation, 0);
    m_shaderProgram.setUniformValue(m_shaderLocation.flag1Location, 0);

    float rotangle = 178.0f;
    m_shaderProgram.setUniformValue(m_shaderLocation.fRotAngle, rotangle);

    m_mvMatrix.setToIdentity();

    // 将角度从度转换为弧度并绕 Z 轴旋转
    // m_mvMatrix.rotate(_PI * rotangle / 180.0, 0, 0, 1);

    float motion_dx = 0.0;
    float motion_dy = 0.0;
    // 平移模型视图矩阵
    m_mvMatrix.translate(-m_ship.m_x - motion_dx, -m_ship.m_y - motion_dy, -m_ppi.m_range * 0.9);
    m_mvMatrix.setRow(0, QVector4D(1, 0, 0, 0));
    m_mvMatrix.setRow(1, QVector4D(0, 1, 0, 0));
    m_mvMatrix.setRow(2, QVector4D(0, 0, 1, 0));
    m_mvMatrix.setRow(3, QVector4D(-35950.1484375, 31134.2421875, -6667.2001953125, 1));

    // 从 4x4 矩阵中提取法向量矩阵
    m_nMatrix = m_mvMatrix.normalMatrix();

    // drawTerrain();

    glDisable(GL_DEPTH_TEST);
    // glDrawArrays(GL_TRIANGLES, 0, 6);
#endif

    m_vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6); // 绘制六边形
    m_vao.release();
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
}

void RadarWidget::FillGLBuffer()
{
    m_vertexBuffer_terrainvboc.create();
    m_vertexBuffer_terrainvboc.bind();
    m_vertexBuffer_terrainvboc.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer_terrainvboc.allocate(
        m_terrainvboc.structures[0].colorpatch.vertexData, m_terrainvboc.structures[0].colorpatch.vsize);
    // glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer_terrainvboc.bufferId());
    // glBufferData(GL_ARRAY_BUFFER, m_terrainvboc.structures[0].colorpatch.vsize, 
    //     m_terrainvboc.structures[0].colorpatch.vertexData, GL_STATIC_DRAW);

    m_vertexBuffer_terrainvbom.create();
    m_vertexBuffer_terrainvbom.bind();
    m_vertexBuffer_terrainvbom.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer_terrainvbom.allocate(
        m_terrainvbom.structures[0].colorpatch.vertexData, m_terrainvboc.structures[0].colorpatch.vsize);

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
}

void RadarWidget::drawTerrain()
{
    for (size_t i = 0; i < m_terrainvboc.header.numStructures; i++)
    {
        auto& s = m_terrainvboc.structures[i];
        if (s.colorpatch.header.numtriangles > 0)
        {
            if (s.colorpatch.header.noColors > 0)
            {
                m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexPositionAttribute);
                m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexNormalAttribute);
                m_shaderProgram.enableAttributeArray(m_shaderLocation.vertexColorAttribute);
                m_shaderProgram.enableAttributeArray(m_shaderLocation.textureCoordAttribute);

                m_vertexBuffer_terrainvboc.bind();
                // glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer_terrainvboc.bufferId());
                // m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexPositionAttribute, GL_FLOAT, 0, 3, 24);
                // m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexNormalAttribute, GL_FLOAT, 12, 3, 24);

                int offset1 = 0;
                glVertexAttribPointer(m_shaderLocation.vertexPositionAttribute, 3, GL_FLOAT, GL_FALSE, 24, reinterpret_cast<const void*>(qintptr(offset1)));
                int offset2 = 12;
                glVertexAttribPointer(m_shaderLocation.vertexNormalAttribute, 3, GL_FLOAT, GL_FALSE, 24, reinterpret_cast<const void*>(qintptr(offset2)));

                m_shaderProgram.setUniformValue(m_shaderLocation.pMatrixUniform, m_pMatrix);
                m_shaderProgram.setUniformValue(m_shaderLocation.mvMatrixUniform, m_mvMatrix);
                m_shaderProgram.setUniformValue(m_shaderLocation.nMatrixUniform, m_nMatrix);

                glDrawArrays(GL_TRIANGLES, 0, s.colorpatch.header.numtriangles * 3);

                m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexPositionAttribute);
                m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexNormalAttribute);
            }
        }
    }
}
