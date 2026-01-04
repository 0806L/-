#pragma once

#include <QOpenGLWindow>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QMatrix4x4>
#include <QMatrix3x3>
#include <QVector>
#include <QVector3D>

#include <QMouseEvent>

#include "GeoTransform.h"

struct ShaderProgramLocation_t
{
    int vertexPositionAttribute;
    int vertexNormalAttribute;
    int textureCoordAttribute;
    int vertexColorAttribute;
    int heightTextureCoordAttribute;

    int pMatrixUniform;
    int mvMatrixUniform;
    int nMatrixUniform;
    int shipCoordUniform;

    int uHeigthMapUniform;
    int uSamplerUniform;
    int flagLocation;
    int flag1Location;

    int directionalColor;
    int lightPosition;
    int angle0;
    int angle1;

    int antennaHeight;
    int beamHeight2;
    int fRange;
    int fBrilliance;
    int fGain;
    int fTune;
    int fSea;
    int fRain;
    int fFTC;
    int fWaveHeight;
    int fWindDirection;
    int fRotAngle;
};


#include <pshpack1.h>

struct FileVBOBitmapHeader_t
{
    qint32 numtriangles;
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
    float size;
    qint32 bitmapindex;
    qint8 dump1[20];
    qint8 noNormals;
    qint8 dump2[15];
};

struct FileVBOColorpatchHeader_t
{
    qint32 numtriangles;
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
    float size;
    qint8 dump1[24];
    qint8 noNormals;
    qint8 noColors;
    qint8 dump2[14];
};

struct FileVBOStructuresHeader_t
{
    qint32 numpathces;
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
    float size;
    qint32 numbitmappatches;
    qint32 numcolorpatches;
    qint8 dump[16];
};

struct FileVBODataHeader_t
{
    qint32 magicnumber;
    qint32 version;
    qint8 dump1[8];
    qint32 numStructures;
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    float zmin;
    float zmax;
    float size;
    qint8 dump2[16];
};

#include <poppack.h>
struct FileVBOBitmap_t
{
    FileVBOBitmapHeader_t header;
    qint32 vsize;
    float* vertexData;
};

struct FileVBOColorpatch_t
{
    FileVBOColorpatchHeader_t header;
    qint32 vsize;
    float* vertexData;
};

struct FileVBOStructure_t
{
    FileVBOStructuresHeader_t header;
    FileVBOBitmap_t* bitmaps;
    FileVBOColorpatch_t colorpatch;
};

struct FileVBOData_t
{
    FileVBODataHeader_t header;
    FileVBOStructure_t* structures;
};



class RadarController;
class RadarData;


class RadarWidget  : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:

    RadarWidget();
    ~RadarWidget();

    bool loadTerrain(const QString& name);

    RadarController* controller()
    {
        return m_controller;
    }

protected:

    void timerEvent(QTimerEvent* e);
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

	void LoadVBO2(const QString& filename, FileVBOData_t& data);
	void FillGLBuffer();
    void initOffscreenTexture();
    void createPolarVBO(float r0, float r1, int n, int m, float xcentre, float ycentre, float power);

    void drawCircleMask(float x, float y, float radius);
    void drawContent();

    void drawTerrain();
    void drawWater();
    void drawHeightMap();

    void readDEMData(const QString& filepath);

    int m_frameCount;
    QString m_draggingWhat;
    bool m_mouseLeftPressed;

    // 渲染数据
    QOpenGLShaderProgram m_shaderProgram;
    QOpenGLShader *m_fragmentShader;
    QOpenGLShader *m_vertexShader;

	ShaderProgramLocation_t m_shaderLocation;

    QMatrix4x4 m_mvMatrix;
    QMatrix4x4 m_tempMatrix2;
    QMatrix4x4 m_tempMatrix;
    QMatrix4x4 m_pMatrix;
    QMatrix3x3 m_nMatrix;

    float m_minx;
    float m_maxx;
    float m_miny;
    float m_maxy;

    FileVBOData_t m_terrainvboc;
    FileVBOData_t m_terrainvbom;
    // 目前只有colorpatch
    QOpenGLBuffer m_vertexBuffer_terrainvboc;
    // QOpenGLBuffer m_vertexBuffer_terrainvbom;
    QOpenGLTexture *m_screentexture;

    QOpenGLTexture* m_heightTexture;
    QImage m_screenImage;

    QOpenGLBuffer m_wavesvbo;
    int m_wavesNumOfTri;

    // QVector<QVector3D> m_terrainVertices; // x,y坐标 + 颜色高度值
    QOpenGLBuffer m_terrainVbo;
    QOpenGLBuffer m_terrainNormalVbo;
    int m_numOfVert;
    int m_numOfNormVert;

    // test
    // QOpenGLBuffer m_vertexBuffer;
    // QOpenGLVertexArrayObject m_vao;
    QOpenGLShaderProgram m_tstprogram;

    // 雷达数据
    // RadarOwnship m_ship;
    // RadarPPI m_ppi;
    // QList<RadarTarget> m_targets;

    // int m_rangeIndex;
    // float m_angle;

	// GeoTransform m_geo;

    // QColor m_backgroundColor;

    RadarController* m_controller;
    RadarData* m_data;

};
