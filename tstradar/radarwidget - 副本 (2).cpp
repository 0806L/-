#define _CRT_SECURE_NO_WARNINGS
#include "radarwidget.h"

#include <QDebug>
#include <QFile>


RadarWidget::RadarWidget()
{
    // setSurfaceType(QOpenGLWindow::OpenGLSurface);

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
    FillGLBuffer();
    initOffscreenTexture();

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
    glClear(GL_COLOR_BUFFER_BIT);
    // glUseProgram(m_shaderProgram.programId());
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
        fread(s.colorpatch.vertexData, s.colorpatch.vsize, 1, fp);

    }
}

void RadarWidget::FillGLBuffer()
{
    m_vertexBuffer_terrainvboc.create();
    m_vertexBuffer_terrainvboc.bind();
    m_vertexBuffer_terrainvboc.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer_terrainvboc.allocate(
        m_terrainvboc.structures[0].colorpatch.vertexData, m_terrainvboc.structures[0].colorpatch.vsize);

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
                m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexPositionAttribute, GL_FLOAT, 0, 3, 24);
                m_shaderProgram.setAttributeBuffer(m_shaderLocation.vertexNormalAttribute, GL_FLOAT, 12, 3, 24);

                m_shaderProgram.setUniformValue(m_shaderLocation.pMatrixUniform, m_pMatrix);
                m_shaderProgram.setUniformValue(m_shaderLocation.pMatrixUniform, m_mvMatrix);
                m_shaderProgram.setUniformValue(m_shaderLocation.pMatrixUniform, m_nMatrix);

                glDrawArrays(GL_TRIANGLES, 0, s.colorpatch.header.numtriangles * 3);

                m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexPositionAttribute);
                m_shaderProgram.disableAttributeArray(m_shaderLocation.vertexNormalAttribute);
            }
        }
    }
}
