#ifndef ACC_IMAGE_FILTER_H
#define ACC_IMAGE_FILTER_H

#include <stdlib.h>
#include "xil_types.h"

/* Maksimalni podrzani radius filtra  */
#define MAX_R 4
#define MAX_WIN (2 * MAX_R + 1)   /* 9x9 prozor*/

typedef struct ProcessingParams {
    u16 imgW, imgH, FilterCoeffsScale, FilterRadius;
    s16 FilterCoeffs[81];
    u8 BorderVal;
    int Mode, Bypass, Border;
} ProcessingParams;

typedef enum {
    PIXEL_LOW,
    PIXEL_HIGH
} PixelState;

void imageFilterSW(u8* DataIn, u16* DataOut, ProcessingParams* Params);

#endif