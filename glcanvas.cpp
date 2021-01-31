#include "glcanvas.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMouseEvent>

GLCanvas::GLCanvas(QImage image, QWidget *parent)
    : QOpenGLWidget(parent)
    , m_clearColor(Qt::black)
    , m_texture(nullptr)
    , m_program(nullptr)
    , m_imageInit(image)
{
}

GLCanvas::GLCanvas(QWidget *parent)
    : QOpenGLWidget(parent)
    , m_clearColor(Qt::black)
    , m_texture(nullptr)
    , m_program(nullptr)
{
}

GLCanvas::~GLCanvas()
{
    makeCurrent();
    m_vbo.destroy();
    if(m_texture)
    {
        m_texture->destroy();
        delete m_texture;
    }
    delete m_program;
    doneCurrent();
}

QSize GLCanvas::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLCanvas::sizeHint() const
{
    return QSize(200, 200);
}

void GLCanvas::setClearColor(const QColor &color)
{
    m_clearColor = color;
    update();
}


void GLCanvas::initializeGL()
{
    initializeOpenGLFunctions();
    glEnable(GL_DEPTH_TEST);

    makeObject();

    if(m_imageInit.isNull())
        m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
    else {
        m_texture = new QOpenGLTexture(m_imageInit);
    }
    m_texture->create();

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
    const char *vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec4 texCoord;\n"
        "varying mediump vec4 texc;\n"
        "uniform mediump mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "    gl_Position = matrix * vertex;\n"
        "    texc = texCoord;\n"
        "}\n";
    vshader->compileSourceCode(vsrc);

    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
    const char *fsrc =
        "uniform sampler2D texture;\n"
        "varying mediump vec4 texc;\n"
        "void main(void)\n"
        "{\n"
        "    gl_FragColor = texture2D(texture, texc.st);\n"
        "}\n";
    fshader->compileSourceCode(fsrc);

    m_program = new QOpenGLShaderProgram;
    m_program->addShader(vshader);
    m_program->addShader(fshader);
    m_program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    m_program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);
    m_program->link();

    m_program->bind();
    m_program->setUniformValue("texture", 0);
}

void GLCanvas::paintGL()
{
    glClearColor(m_clearColor.redF(), m_clearColor.greenF(),
                 m_clearColor.blueF(), m_clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 m;
    m.ortho(-0.5f, +0.5f, +0.5f, -0.5f, 2.0f, 10.0f);
    m.translate(0.0f, 0.0f, -5.0f);

    m_program->setUniformValue("matrix", m);
    m_program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    m_program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    m_program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    m_program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE, GL_FLOAT, 3 * sizeof(GLfloat), 2, 5 * sizeof(GLfloat));

    m_texture->bind();
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
}

void GLCanvas::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
}

void GLCanvas::mousePressEvent(QMouseEvent *event)
{
}

void GLCanvas::mouseReleaseEvent(QMouseEvent * /* event */)
{
    emit clicked();
}

void GLCanvas::displayImage(QImage image)
{
    if(m_texture)
    {
        m_texture->destroy();
        delete m_texture;
    }
    m_texture = new QOpenGLTexture(image);

    update();
}

void GLCanvas::makeObject()
{
    static const double coords[4][4][3] = {
        {{ -0.6, +0.6, 0 }, { -1, +0.6, 0 }, { -1, +1, 0 }, { -0.6, +1, 0 }},
        { { +1, +1, -1 }, { -1, +1, -1 }, { -1, -1, -1 }, { +1, -1, -1 }},
        {{ +0.2, -0.2, 0 }, { -1, -0.2, 0 }, { -1, +1, 0 }, { +0.2, +1, 0 }},
        {{ +1, -1, -1 }, { -0.2, -1, -1 }, { -0.2, +0.2, -1 }, { +1, +0.2, -1 }},
    };
    QVector<GLfloat> vertData;
    for(int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; ++j)
        {
            // vertex position
            vertData.append(0.5 * coords[i][j][0]);
            vertData.append(0.5 * coords[i][j][1]);
            vertData.append(0.5 * coords[i][j][2]);
            // texture coordinate
            vertData.append(j == 0 || j == 3);
            vertData.append(j == 0 || j == 1);
        }
    }

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}

