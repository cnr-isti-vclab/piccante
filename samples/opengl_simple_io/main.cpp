
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#define PIC_DEBUG
#include "piccante.hpp"

class SimpleIOWindow : public pic::OpenGLWindow
{
protected:
    pic::QuadGL *quad;
    pic::FilterGLSimpleTMO *tmo;

    int m_frame;

public:
    pic::ImageRAWGL img, *imgOut;
    glw::program    program;

    SimpleIOWindow() : OpenGLWindow(NULL)
    {
        tmo = NULL;
        imgOut = NULL;
        m_frame = 0;
    }

    void init()
    {
        bool b = img.Read("../data/input/bottles.hdr");
        img.generateTextureGL(false);

        pic::QuadGL::getProgram(program,
                                pic::QuadGL::getVertexProgramV3(),
                                pic::QuadGL::getFragmentProgramForView());

        tmo = new pic::FilterGLSimpleTMO();
        quad = new pic::QuadGL(true);
    }

    void render()
    {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        //simple tone mapping: gamma + exposure correction
        imgOut = tmo->Process(SingleGL(&img), imgOut);

        //imgOut visualization
        glw::bind_program(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, imgOut->getTexture());

        quad->Render();

        glw::bind_program(0);

        //Textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        m_frame++;
    }
};


int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(4);
    format.setMajorVersion(4);
    format.setMinorVersion(0);
    format.setProfile(QSurfaceFormat::CoreProfile );

    SimpleIOWindow window;
    window.setFormat(format);
    window.resize(912, 684);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
