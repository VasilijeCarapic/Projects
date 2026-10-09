#ifndef _GENERATE_COEFFS_ 
#define _GENERATE_COEFFS_ 

#include <stdint.h>
#include "xil_types.h"
#include "acc_image_filter.h"

typedef enum { BOX_IDEAL, GAUSS, LOG, SHARPEN_BOX, SHARPEN_GAUSS, SOBEL } filt_type_t; 
void generateCoeffitients(ProcessingParams* params, filt_type_t FILT_TYPE, float sigma, float k, float inv_scale);

/* za debug da vidim koji se koeficijenti dobijaju */
void printCoeffs(int16_t s16_arr[MAX_WIN * MAX_WIN]);

#endif

