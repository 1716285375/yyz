#ifndef RENDEROPENGL_H
#define RENDEROPENGL_H


#include <QtOpenGLWidgets/QOpenGLWidget>  // 必须首先包含
 #include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QMutex>
#include <QImage>

class RenderOpenGL : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core
{
    Q_OBJECT
public:
    explicit RenderOpenGL(QWidget *parent = nullptr);
    ~RenderOpenGL() override;

public slots:
    void updateImage(const QImage& image);
    void setVideoSize(int w, int h);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private:
    void initShaders();
    void initGeometry();
    void updateVertices();

    void cleanup();

    QOpenGLShaderProgram* shaderProgram_ = nullptr;
    QOpenGLTexture* texture_ = nullptr;
    QImage currentImage_;
    QMutex textureMutex_;
    GLuint vbo_ = 0;
    GLuint vao_ = 0;
    GLuint ebo_ = 0;

    int videoWidth_ = 0;
    int videoHeight_ = 0;
    int windowWidth_ = 640;
    int windowHeight_ = 480;
};

#endif // RENDEROPENGL_H
