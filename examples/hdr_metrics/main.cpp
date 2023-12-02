//This means that we disable Eigen; some functionalities cannot be used.
//For example, estimating the camera response function
#define PIC_DISABLE_EIGEN

//This means that OpenGL acceleration layer is disabled
#define PIC_DISABLE_OPENGL

#include "piccante.hpp"

int main(int argc, char *argv[])
{
    std::string img0_str, img1_str;
    bool bCreate = false;

    if(argc == 3) {
        img0_str = argv[1];
        img1_str = argv[2];
    } else {
        img0_str = "../data/input/bottles.hdr";
        bCreate = true;
    }

    pic::Image img0, img1;
    img0.Read(img0_str);

    bool bLoaded = false;
    if(!bCreate) {
        bLoaded = img1.Read(img1_str);
    }

    printf("Is it valid? ");
    if(img0.isValid() && (bCreate || bLoaded)) {
        printf("OK\n");

        std::string name = pic::removeLocalPath(img0_str);
        auto name_ext = pic::getExtension(name);
        name = pic::removeExtension(name);

        pic::Image *tmp;
        if(bCreate) {
            printf("Filtering the input image (blurring)...");
            tmp = pic::FilterGaussian2D::execute(&img0, NULL, 4.0f);
            printf("Ok\n");
            tmp->Write("../data/output/" + name + "_flt." + name_ext);
        } else {
            tmp = &img1;
        }

        float ssim_index, ssim_index_pu;
        pic::SSIMIndex ssim;
        pic::Image *ssim_map = ssim.execute(Double(&img0, tmp), ssim_index, NULL);
        printf("Ok\n");

        if(ssim_map != NULL) {
            ssim_map->Write("../data/output/" + name + "_ssim_map_lin.pfm");
        }

        ssim.update(-1.0f, -1.0f, -1.0f, -1.0f, true, pic::MD_PU08);
        ssim_map = ssim.execute(Double(&img0, tmp), ssim_index_pu, ssim_map);

        if(ssim_map != NULL) {
            ssim_map->Write("../data/output/" + name + "_ssim_map_pu.pfm");
        }

        printf("SSIM (classic): %3.3f \t  SSIM (PU-encoding): %3.3f\n",
               ssim_index, ssim_index_pu);

        printf("MSE (classic): %3.3f \t  MSE (PU-encoding): %3.3f\n",
               pic::MSE(&img0, tmp, false, pic::MD_LIN),
               pic::MSE(&img0, tmp, false, pic::MD_PU08));

        printf("RMSE (classic): %3.3f \t  RMSE (PU-encoding): %3.3f\n",
        pic::RMSE(&img0, tmp, false, pic::MD_LIN),
        pic::RMSE(&img0, tmp, false, pic::MD_PU08));

        printf("PSNR (classic): %3.3f \t PSNR (PU-encoding): %3.3f\n",
               pic::PSNR(&img0, tmp, -1.0f, false, pic::MD_LIN ),
               pic::PSNR(&img0, tmp, -1.0f, false, pic::MD_PU08 ));

        printf("MAE (classic): %3.3f \t MAE (PU-encoding): %3.3f\n",
               pic::MAE(&img0, tmp, false, pic::MD_LIN),
               pic::MAE(&img0, tmp, false, pic::MD_PU08));

        printf("Relative Error (classic): %3.3f \t Relative Error (PU-encoding): %3.3f\n",
               pic::RelativeError(&img0, tmp, false, pic::MD_LIN),
               pic::RelativeError(&img0, tmp, false, pic::MD_PU08));

    } else {
        printf("No, the file is not valid!\n");
    }

    return 0;
}
