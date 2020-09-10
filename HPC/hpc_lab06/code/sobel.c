#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "image.h"
#include "sobel.h"

#define GAUSSIAN_KERNEL_SIZE    3
#define SOBEL_KERNEL_SIZE       3

#define SOBEL_BINARY_THRESHOLD  150  // in the range 0 to uint8_max (255)

const int16_t sobel_v_kernel[SOBEL_KERNEL_SIZE*SOBEL_KERNEL_SIZE] = {
    -1, -2, -1,
     0,  0,  0,
     1,  2,  1,
};

const int16_t sobel_h_kernel[SOBEL_KERNEL_SIZE*SOBEL_KERNEL_SIZE] = {
    -1,  0,  1,
    -2,  0,  2,
    -1,  0,  1,
};

const uint16_t gauss_kernel[GAUSSIAN_KERNEL_SIZE*GAUSSIAN_KERNEL_SIZE] = {
    1, 2, 1,
    2, 4, 2,
    1, 2, 1,
};

void rgb_to_grayscale(const struct img_t *img, struct img_t *result)
{
    size_t index = 0;

    for (size_t y = 0; y < result->height; y++) {
        for (size_t x = 0; x < result->width; x++) {
            result->data[y * result->width + x] =
                FACTOR_R * img->data[index + R_OFFSET] +
                FACTOR_G * img->data[index + G_OFFSET] +
                FACTOR_B * img->data[index + B_OFFSET];

            index += img->components;
        }
    }
}

void gaussian_filter(const struct img_t *img, struct img_t *res_img, const uint16_t *kernel)
{
    uint16_t tableu[3][3];
    
    for (int i= 2 ; i >= 0 ;i--){
        tableu[i][0] = img->data[(i) * img->width + (1 - 1)];
        tableu[i][1] = img->data[(i) * img->width + (1)];
        tableu[i][2] = 0;
    }   

    const uint16_t gauss_ponderation = 16;

    for (size_t y = 0; y < img->height; y++) {
        for (size_t x = 0; x < img->width; x++) {
        
            uint16_t pix_acc = 0;

            if (x < GAUSSIAN_KERNEL_SIZE/2 ||
                x >= img->width - GAUSSIAN_KERNEL_SIZE/2 ||
                y < GAUSSIAN_KERNEL_SIZE/2 ||
                y >= img->height - GAUSSIAN_KERNEL_SIZE/2) {
                    res_img->data[y * res_img->width + x] = img->data[y * img->width + x];
                    continue;
            }
            
    for (int i= 2 ; i >= 0 ;i--){
            tableu[i][2] = img->data[(y + i -1 ) * img->width + (x + 1)];
        }   

        pix_acc = kernel[0] * tableu[0][0] + kernel[3] * tableu[0][1] + kernel[6] * tableu[0][2]
                + kernel[1] * tableu[1][0] + kernel[4] * tableu[0][1] + kernel[7] * tableu[0][2]
                + kernel[2] * tableu[2][0] + kernel[5] * tableu[2][1] + kernel[8] * tableu[2][2];

        //on fait avancer le tableau vers la gauche
        for (int i= 2 ; i >= 0 ;i--){

                tableu[i][0] = tableu[1][1];
                tableu[i][1] = tableu[i][2];
            }
/**
            for (size_t ky = 0; ky < GAUSSIAN_KERNEL_SIZE; ky++) {
                for (size_t kx = 0; kx < GAUSSIAN_KERNEL_SIZE; kx++) {
                    pix_acc += kernel[ky * GAUSSIAN_KERNEL_SIZE + kx] *
                               img->data[(y - 1 + ky) * img->width + (x - 1 + kx)];
                }
            }
*/
            res_img->data[y * res_img->width + x] = pix_acc / gauss_ponderation;
        }
        //si fin de ligne on remet le tableu 
        if (y !=0 && y != img->height){
    for (int i= 2 ; i >= 0 ;i--){
            tableu[i][0] = img->data[(i) * img->width + (1 - 1)];
            tableu[i][1] = img->data[(i) * img->width + (1)];
            tableu[i][2] = 0;
        } 
}
    }
}

void sobel_filter(const struct img_t *img, struct img_t *res_img, 
                  const int16_t *v_kernel, const int16_t *h_kernel)
{
        uint16_t tableu[3][3];
        
    for (int i= 2 ; i >= 0 ;i--){
            tableu[i][0] = img->data[(i) * img->width + (1 - 1)];
            tableu[i][1] = img->data[(i) * img->width + (1)];
            tableu[i][2] = 0;
        }   
    for (size_t y = 0; y < img->height; y++) {
        for (size_t x = 0; x < img->width; x++) {

            int16_t Gx = 0;
            int16_t Gy = 0;
            
            if (x < SOBEL_KERNEL_SIZE/2 ||
                x >= img->width - SOBEL_KERNEL_SIZE/2 ||
                y < SOBEL_KERNEL_SIZE/2 ||
                y >= img->height - SOBEL_KERNEL_SIZE/2) {
                    res_img->data[y * res_img->width + x] = img->data[y * img->width + x];
                    continue;
            }
        //on calcul la derniere ligne
        for (int i= 2 ; i >= 0 ;i--){
            tableu[i][2] = img->data[(y + i -1 ) * img->width + (x + 1)];;
        }   

        Gx = h_kernel[0] * tableu[0][0]  + h_kernel[1] * tableu[0][1]  +  h_kernel[2] * tableu[0][2] 
                + h_kernel[3] * tableu[1][0] + h_kernel[4] * tableu[1][1] + h_kernel[5] * tableu[1][2]
                + h_kernel[6] * tableu[2][0] + h_kernel[7] * tableu[2][1] + h_kernel[8] * tableu[2][2];

            Gy = v_kernel[0] * tableu[0][1] + v_kernel[1] * tableu[0][2] + v_kernel[2] * tableu[0][3]
                + v_kernel[3] * tableu[1][0] + v_kernel[4] * tableu[1][1]+ v_kernel[5] * tableu[1][2]
                + v_kernel[6] * tableu[2][0] + v_kernel[7] * tableu[2][1] + v_kernel[8] * tableu[2][2];
            //on décale notre tableau vers la gauche
        for (int i= 2 ; i >= 0 ;i--){
            tableu[i][0] = tableu[1][1];
            tableu[i][1] = tableu[i][2];
        }   

/**
            for (size_t ky = 0; ky < SOBEL_KERNEL_SIZE; ky++) {
                for (size_t kx = 0; kx < SOBEL_KERNEL_SIZE; kx++) {
                    Gx += h_kernel[ky * SOBEL_KERNEL_SIZE + kx] *
                          img->data[(y - 1 + ky) * img->width + (x - 1 + kx)];

                    Gy += v_kernel[ky * SOBEL_KERNEL_SIZE + kx] *
                          img->data[(y - 1 + ky) * img->width + (x - 1 + kx)];
                }
            }
*/
            res_img->data[y * res_img->width + x] = 
                sqrt(Gx*Gx + Gy*Gy) > SOBEL_BINARY_THRESHOLD ? UINT8_MAX : 0;
        }
        //si fin de ligne on remet le tableu 
        if (y !=0 && y != img->height){
            for (int i= 2 ; i >= 0 ;i--){
                tableu[i][0] = img->data[(i) * img->width + (1 - 1)];
                tableu[i][1] = img->data[(i) * img->width + (1)];
                tableu[i][2] = 0;
            }
        }
    }
}

struct img_t *edge_detection(const struct img_t *input_img)
{
    struct img_t *res_img;
    struct img_t *gauss_img;
    struct img_t *gs_img;

    if (input_img->components < COMPONENT_RGB) {
        fprintf(stderr, "[%s] only accepts images with RGB(A) components", __func__);
        return NULL;
    }

    gs_img = allocate_image(input_img->width, input_img->height, COMPONENT_GRAYSCALE);
    gauss_img = allocate_image(gs_img->width, gs_img->height, gs_img->components);
    res_img = allocate_image(gs_img->width, gs_img->height, gs_img->components);

    rgb_to_grayscale(input_img, gs_img);
    gaussian_filter(gs_img, gauss_img, gauss_kernel);
    sobel_filter(gauss_img, res_img, sobel_v_kernel, sobel_h_kernel);

    free_image(gauss_img);
    free_image(gs_img);

    return res_img;
}
