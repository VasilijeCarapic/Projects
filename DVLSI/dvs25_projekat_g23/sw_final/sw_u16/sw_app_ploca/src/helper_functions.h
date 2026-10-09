#ifndef _HELPER_FUNCTIONS_H
#define _HELPER_FUNCTIONS_H 

#include "xil_types.h"
#include "acc_image_filter.h"

int calculatePxSize(ProcessingParams* params);
int calcOutputDims(u16* outW, u16* outH, ProcessingParams* params);
void packPixelsBypass(u16 imgW, u16 imgH, u8* DataIn, u16* DataOut);

/* funkcije za validaciju unosa */
int   validateInt(int minVal, int maxVal);
float validateFloat(float minVal, float maxVal);   
char  validateChar();
void  flush_stdin_line(void);

#endif