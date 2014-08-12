
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "piccante.hpp"

class SimpleOperatorsWindow : public pic::OpenGLWindow
{
protected:
    pic::QuadGL *quad;
    pic::FilterGLSimpleTMO *tmo;
    pic::FilterGLOp *op;

public:
    pic::ImageRAWGL img, *imgRand, *imgOut, *imgOutTMO;
    glw::program    program;

    SimpleOperatorsWindow() : OpenGLWindow(NULL)
    {
        tmo = NULL;
        imgOut = NULL;
        imgOutTMO = NULL;

    }

    void init()
    {
        //reading an input image
        img.Read("../data/input/bottles.hdr");
        img.generateTextureGL(false);

        //creating a random image
        imgRand = new pic::ImageRAWGL(img.frames, img.width, img.height, img.channels, pic::IMG_CPU_GPU);
        imgRand->SetRand();
        imgRand->loadFromMemory();

        //creating a screen aligned quad
        pic::QuadGL::getProgram(program,
                                pic::QuadGL::getVertexProgramV3(),
                                pic::QuadGL::getFragmentProgramForView());
        quad = new pic::QuadGL(true);

        //allocating a new filter for simple tone mapping
        tmo = new pic::FilterGLSimpleTMO();

        //allocating the add operator
        op = pic::FilterGLOp::CreateOpAdd(false);
    }

    void render()
    {
        const qreal retinaScale = devicePixelRatio();
        glViewport(0, 0, width() * retinaScale, height() * retinaScale);

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        //imgoOut = img + imgRand
        imgOut = op->Process(DoubleGL(&img, imgRand), imgOut);

        //simple tone mapping: gamma + exposure correction
        imgOutTMO = tmo->Process(SingleGL(imgOut), imgOutTMO);

        //imgOut visualization
        glw::bind_program(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, imgOutTMO->getTexture());

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
    format.setProfile(QSurfaceFormat::CoreProfile );

    SimpleOperatorsWindow window;
    window.setFormat(format);
    window.resize(912, 684);
    window.show();

    window.setAnimating(true);

    return app.exec();
}
