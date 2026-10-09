#ifndef _COEFFITIENTS_H
#define _COEFFITIENTS_H

#include "acc_image_filter.h"
#include "xil_types.h"

typedef enum FiltType { FILT_BOX_S1, FILT_BOX_S3, FILT_LOG, FILT_GAUSS_S1, FILT_GAUSS_S1_Q4,
FILT_SHARP_GAUSS_R2, FILT_SHARP_GAUSS_R4, FILT_SHARP_BOX_R4, FILT_SHARP_BOX_R1, FILT_SOBEL_R1} FilterConfig;
void setFilterConfig(ProcessingParams* Params, FilterConfig config);

#endif