
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "piccante.hpp"

class SimpleFilteringWindow : public pic::OpenGLWindow
{
protected:
    pic::QuadGL *quad;
    pic::FilterGLSimpleTMO *tmo;
    pic::FilterGLGaussian2D *fltGauss;

public:
    pic::ImageRAWGL img, *img_flt, *img_flt_tmo;
    glw::program    program;

    SimpleFilteringWindow() : OpenGLWindow(NULL)
    {
        tmo = NULL;
        img_flt = NULL;
        img_flt_tmo = NULL;
    }

    void init()
    {
        //reading an input image
        img.Read("../data/input/bottles.hdr");
        img.generateTextureGL(false);

        //creating a screen aligned quad
        pic::QuadGL::getProgram(program,
                                pic::QuadGL::getVertexProgramV3(),
                                pic::QuadGL::getFragmentProgramForView());
        quad = new pic::QuadGL(true);

        //allocating a new filter for simple tone mapping
        tmo = new pic::FilterGLSimpleTMO();

        //allocating a Gaussian filter with sigma = 2.0
        fltGauss = new pic::FilterGLGaussian2D(2.0);
    }

    void render()
    {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        //imgoOut = img + imgRand
        img_flt = fltGauss->Process(SingleGL(&img), img_flt);

        //simple tone mapping: gamma + exposure correction
        img_flt_tmo = tmo->Process(SingleGL(img_flt), img_flt_tmo);

        //visualization of the Gaussian filtering
        glw::bind_program(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, img_flt_tmo->getTexture());

        quad->Render();

        glw::bind_program(0);

        //Textures
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(4);
    format.setMajorVersion(4);
    format.setMinorVersion(0);
    format.setProfile(QSurfaceFormat::CoreProfile);

    SimpleFilteringWindow window;
    window.setFormat(format);
    window.resize(912, 684);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
