#include "RenderOpenGL.h"

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QDebug>

RenderOpenGL::RenderOpenGL(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setMinimumSize(640, 480);
    setUpdateBehavior(QOpenGLWidget::PartialUpdate);
}

RenderOpenGL::~RenderOpenGL()
{
    makeCurrent();
    cleanup();
    doneCurrent();
}

void RenderOpenGL::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_TEXTURE_2D);

    initShaders();
    initGeometry();
}

void RenderOpenGL::resizeGL(int w, int h)
{
    windowWidth_ = w;
    windowHeight_ = h;
    glViewport(0, 0, w, h);
    updateVertices();
}

void RenderOpenGL::updateVertices()
{
    if (videoWidth_ == 0 || videoHeight_ == 0 || windowWidth_ == 0 || windowHeight_ == 0)
        return;

    float windowAspect = (float)windowWidth_ / windowHeight_;
    float videoAspect = (float)videoWidth_ / videoHeight_;

    float scaleX = 1.0f;
    float scaleY = 1.0f;

    if (windowAspect > videoAspect) {
        // 窗口比视频宽，宽度缩小
        scaleX = videoAspect / windowAspect;
    } else {
        // 窗口比视频高，高度缩小
        scaleY = windowAspect / videoAspect;
    }

    GLfloat vertices[] = {
        // positions           // texCoords
        -scaleX,  scaleY, 0.0f,   0.0f, 1.0f, // top-left
        -scaleX, -scaleY, 0.0f,   0.0f, 0.0f, // bottom-left
        scaleX, -scaleY, 0.0f,   1.0f, 0.0f, // bottom-right
        scaleX,  scaleY, 0.0f,   1.0f, 1.0f  // top-right
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderOpenGL::paintGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    QMutexLocker locker(&textureMutex_);

    if (currentImage_.isNull())
        return;

    if (!texture_) {
        texture_ = new QOpenGLTexture(QOpenGLTexture::Target2D);
        texture_->create();
    }

    texture_->bind();
    texture_->setMinificationFilter(QOpenGLTexture::Linear);
    texture_->setMagnificationFilter(QOpenGLTexture::Linear);
    texture_->setWrapMode(QOpenGLTexture::ClampToEdge);

    QImage glImage = currentImage_.convertToFormat(QImage::Format_RGBA8888).mirrored();
    texture_->setData(glImage);

    shaderProgram_->bind();
    glBindVertexArray(vao_);

    shaderProgram_->setUniformValue("u_texture", 0);  // texture unit 0

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
    shaderProgram_->release();
    texture_->release();
}

void RenderOpenGL::setVideoSize(int w, int h)
{
    videoWidth_ = w;
    videoHeight_ = h;
    updateVertices();
}

void RenderOpenGL::updateImage(const QImage &image)
{
    {
        QMutexLocker locker(&textureMutex_);
        currentImage_ = image.copy();
    }

    setVideoSize(currentImage_.width(), currentImage_.height());

    update();  // trigger paintGL()
}

void RenderOpenGL::initShaders()
{
    shaderProgram_ = new QOpenGLShaderProgram(this);

    const char *vShader =
        R"(#version 430 core
        layout(location = 0) in vec3 position;
        layout(location = 1) in vec2 texCoord;
        out vec2 v_texCoord;
        void main() {
            gl_Position = vec4(position, 1.0);
            v_texCoord = texCoord;
        })";

    const char *fShader =
        R"(#version 430 core
        in vec2 v_texCoord;
        out vec4 fragColor;
        uniform sampler2D u_texture;
        void main() {
            fragColor = texture(u_texture, v_texCoord);
        })";

    if (!shaderProgram_->addShaderFromSourceCode(QOpenGLShader::Vertex, vShader))
        qDebug() << "Vertex shader compile error:" << shaderProgram_->log();

    if (!shaderProgram_->addShaderFromSourceCode(QOpenGLShader::Fragment, fShader))
        qDebug() << "Fragment shader compile error:" << shaderProgram_->log();

    if (!shaderProgram_->link())
        qDebug() << "Shader link error:" << shaderProgram_->log();
}

void RenderOpenGL::initGeometry()
{
    GLfloat vertices[] = {
        // positions      // texCoords
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f, // top-left
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom-left
        1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom-right
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f  // top-right
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void RenderOpenGL::cleanup()
{
    makeCurrent();

    if (texture_) {
        delete texture_;
        texture_ = nullptr;
    }

    if (vao_)
        glDeleteVertexArrays(1, &vao_);
    if (vbo_)
        glDeleteBuffers(1, &vbo_);
    if (ebo_)
        glDeleteBuffers(1, &ebo_);

    doneCurrent();
}
