#include "acc_image_filter.h"
#include "helper_functions.h"
#include <stdio.h>
#include <string.h>

/* ---- pomocne funkcije ---- */
static uint64_t updateLineBuffer(u8* newDataImg, int RowIndex, int ColIndex, u16 procW,
    uint64_t* lineBuffers, u8* pixel);
static void updateRegisters(u8 registers[MAX_WIN][MAX_WIN], uint64_t regData, u8 pixel, u16 R);
static s16 computePixel(const ProcessingParams* params, u8 registers[MAX_WIN][MAX_WIN], const s16* coeffs);

static void filterSWInit(u8* DataIn, ProcessingParams* Params, u8** newDataImg,
    s16** coeffs, uint64_t** lineBuffers);
static void initParameters(u16* inW, u16* R, u16* inH, u16* outW, u16* outH,
    u16* procW, u16* procH, ProcessingParams* Params, int* extended);

static int isImgExtended(ProcessingParams* Params);

static s16* cutTopLeft(ProcessingParams* params);
static void handleBypass(ProcessingParams* Params, u8* DataIn, u16* DataOut, int* skip_processing);

static u8* imageExpand(u8* DataBuffer, ProcessingParams* params);
static u8* imageExpandFixedVal(u8* DataIn, ProcessingParams params, u16 imgW, u16 imgH, u8 fixedVal);
static u8* imageNearestPad(u8* DataIn, ProcessingParams params, u16 imgW, u16 imgH);

/* ---- implementacija ---- */

uint64_t updateLineBuffer(u8* newDataImg, int RowIndex, int ColIndex, u16 procW,
    uint64_t* lineBuffers, u8* pixel)
{
    *pixel = newDataImg[(size_t)RowIndex * procW + ColIndex];

    uint64_t regData = lineBuffers[ColIndex];

    lineBuffers[ColIndex] <<= 8;
    lineBuffers[ColIndex] |= *pixel;

    return regData;
}


void updateRegisters(u8 registers[MAX_WIN][MAX_WIN], uint64_t regData, u8 pixel, u16 R)
{
    for (int row = 0; row <= 2 * R; row++)
        for (int col = 2 * R; col > 0; col--)
            registers[row][col] = registers[row][col - 1];

    registers[0][0] = pixel;

    for (int i = 1; i <= (int)(2 * R); i++)
        registers[i][0] = (u8)((regData >> ((i - 1) * 8)) & 0xFF);
}

s16 computePixel(const ProcessingParams* params, u8 registers[MAX_WIN][MAX_WIN], const s16* coeffs)
{
    u16 R = params->FilterRadius;
    s64 acc = 0;   /* s64 umesto double: podaci su svi celobrojni */

    int win = 2 * R + 1;
    for (int row = 0; row < win; row++)
        for (int col = 0; col < win; col++)
            acc += (s64)registers[row][col] * (s64)coeffs[row * win + col];

    s64 tmp_res = acc * params->FilterCoeffsScale;
    int shift = (params->Mode == 0) ? 27 : 20;
    s16 out_val = (s16)(tmp_res >> shift);

    /* Saturacija umesto wrap-a za Mode=0 -> mora da se poklopi sa HW clamp logikom
       (acc_image_filter.vhd Stage 9: if acc > 255 -> 255, if acc < 0 -> 0).*/
    s16 out_pixel_val;
    if (params->Mode == 0) {
        if (out_val > 255)      out_pixel_val = 255;
        else if (out_val < 0)   out_pixel_val = 0;
        else                    out_pixel_val = out_val;
    }
    else {
        out_pixel_val = out_val;
    }
    return out_pixel_val;
}

static void filterSWInit(u8* DataIn, ProcessingParams* Params, u8** newDataImg,
    s16** coeffs, uint64_t** lineBuffers)
{
    *newDataImg = imageExpand(DataIn, Params);

    int imgW = Params->imgW;

    *coeffs = cutTopLeft(Params);
    *lineBuffers = (uint64_t*)calloc((size_t)imgW, sizeof(uint64_t));
}

void initParameters(u16* inW, u16* R, u16* inH, u16* outW, u16* outH, u16* procW, u16* procH,
    ProcessingParams* Params, int* extended)
{
    *inW = Params->imgW;
    *inH = Params->imgH;
    *R = Params->FilterRadius;

    *procW = *inW;
    *procH = *inH;

    if (*extended) {
        *procW = *inW + 2 * (*R);
        *procH = *inH + 2 * (*R);
    }

    *outW = (*extended) ? *inW : (*inW - 2 * (*R));
    *outH = (*extended) ? *inH : (*inH - 2 * (*R));
}

int isImgExtended(ProcessingParams* Params)
{
    int border = Params->Border;
    if (border == 0b10 || border == 0b11) return 1;
    if (border == 0b00 || border == 0b01) return 0; 
    
    fprintf(stderr, "Nonexistent option\n");
    exit(-1);
}

s16* cutTopLeft(ProcessingParams* params)
{
    s16 imgR = (s16)params->FilterRadius;

    if (imgR == 4) return params->FilterCoeffs;

    u16 newRowSize = 2 * imgR + 1;
    s16* newArr = (s16*)malloc((size_t)newRowSize * newRowSize * sizeof(s16));

    for (int row = 0; row < newRowSize; row++)
        for (int col = 0; col < newRowSize; col++)
            newArr[row * newRowSize + col] = params->FilterCoeffs[row * MAX_WIN + col];

    return newArr;
}

void handleBypass(ProcessingParams* Params, u8* DataIn, u16* DataOut, int* skip_processing)
{
    u16 imgW = Params->imgW, imgH = Params->imgH;

    if (Params->Bypass == 1) {
        packPixelsBypass(imgW, imgH, DataIn, DataOut);
        *skip_processing = 1;
    }
    else {
        *skip_processing = 0;
    }
}


static u8* imageExpand(u8* DataIn, ProcessingParams* params)
{
    int imgH = params->imgH, imgW = params->imgW, radius = params->FilterRadius;
    int newH = imgH + 2 * radius, newW = imgW + 2 * radius;
    u8* newDataIn = NULL;

    switch (params->Border) {
    case 0b10:
        newDataIn = imageNearestPad(DataIn, *params, (u16)newW, (u16)newH);
        params->imgW = (u16)newW; params->imgH = (u16)newH;
        break;
    case 0b11:
        newDataIn = imageExpandFixedVal(DataIn, *params, (u16)newW, (u16)newH, params->BorderVal);
        params->imgW = (u16)newW; params->imgH = (u16)newH;
        break;
    case 0b00:
    default:
        return DataIn;
    }
    return newDataIn;
}

static u8* imageNearestPad(u8* DataIn, ProcessingParams params, u16 newW, u16 newH)
{
    u16 oldW = params.imgW, oldH = params.imgH, img_r = params.FilterRadius;
    u8* newDataIn = (u8*)malloc((size_t)newW * newH * sizeof(u8));
    if (!newDataIn) return NULL;

    for (int row = 0; row < newH; row++) {
        for (int col = 0; col < newW; col++) {
            int orig_row, orig_col;

            // Računanje reda sa nearestom ako smo izvan opsega zakucaj na ivice 
            if (row < img_r) {
                orig_row = 0;
            } else if (row >= newH - img_r) {
                orig_row = oldH - 1; 
            } else {
                orig_row = row - img_r;
            }

            // Računanje kolone sa nearest-neighbor clamping-om
            if (col < img_r) {
                orig_col = 0;
            } else if (col >= newW - img_r) {
                orig_col = oldW - 1; 
            } else {
                orig_col = col - img_r;
            }

            /* preslikavanje na izlaz */
            newDataIn[row * newW + col] = DataIn[orig_row * oldW + orig_col];
        }
    }
    return newDataIn;
}

static u8* imageExpandFixedVal(u8* DataIn, ProcessingParams params, u16 imgW, u16 imgH, u8 fixedVal)
{
    u16 oldW = params.imgW, oldH = params.imgH, img_r = params.FilterRadius;
    u8* newDataIn = (u8*)malloc((size_t)imgW * imgH);
    if (!newDataIn) return NULL; 

    for (int row = 0; row < imgH; row++) {
        for (int col = 0; col < imgW; col++) {
            
            // Provera da li smo na ivici
            if (row < img_r || row >= oldH + img_r || col < img_r || col >= oldW + img_r) {
                newDataIn[row * imgW + col] = fixedVal;
            } 
            else {
                // Unutrašnjost slike - preslikavamo original sa pomakom za radijus
                newDataIn[row * imgW + col] = DataIn[(row - img_r) * oldW + (col - img_r)];
            }
        }
    }
    return newDataIn;
}

void imageFilterSW(u8* DataIn, u16* DataOut, ProcessingParams* Params)
{
    int skip_processing = 0, extended = 0;
    s16* coeffs = Params->FilterCoeffs;
    uint64_t* lineBuffers = NULL;
    u8* newDataImg;
    u8 pixel;
    u8 registers[MAX_WIN][MAX_WIN];
    memset(registers, 0, sizeof(registers));

    u16 inW, inH, R, procW, procH, outW, outH;

    extended = isImgExtended(Params);
    initParameters(&inW, &R, &inH, &outW, &outH, &procW, &procH, Params, &extended);

    handleBypass(Params, DataIn, DataOut, &skip_processing);
    if (skip_processing) return;

    filterSWInit(DataIn, Params, &newDataImg, &coeffs, &lineBuffers);

    /* Za Mode==0 pakujemo 2 piksela u jednu 16-bitnu rec, "u letu" tokom konvolucije.
       Layout mora da se poklopi sa packPixelsBypass / HW MODE=0 pakovanjem. */
    u32 wordsPerRow = (Params->Mode == 0) ? (((u32)outW + 1) / 2) : 0;
    u8  pendingLow  = 0;

    for (int RowIndex = 0; RowIndex < procH; RowIndex++) {
        for (int ColIndex = 0; ColIndex < procW; ColIndex++) {
            uint64_t regData = updateLineBuffer(newDataImg, RowIndex, ColIndex, procW, lineBuffers, &pixel);

            updateRegisters(registers, regData, pixel, R);

            if ((ColIndex >= 2 * R && ColIndex < procW) && (RowIndex >= 2 * R && RowIndex < procH)) {
                s16 val = computePixel(Params, registers, coeffs);
                
                int localRow = RowIndex - 2 * R;
                int localCol = ColIndex - 2 * R;

                if (Params->Mode == 0) {
                    /* 2 px u 16-bitnu rec */
                    u32 wordIndex = (u32)localRow * wordsPerRow + (u32)(localCol / 2);

                    if (localCol % 2 == 0) {
                        /* donja vrednost pixela */
                        pendingLow = (u8)val;

                        /* ako je to poslednji pixel u redu */
                        if (localCol == outW - 1) { DataOut[wordIndex] = (u16)pendingLow; }
                    } 
                    else {
                        /* visi pixel u reci */
                        u16 packedWord = ((u16)(u8)val << 8) | (u16)pendingLow;
                        DataOut[wordIndex] = packedWord;
                    }
                } 
                else {
                    /* mode 1: jedna reč po pikselu */
                    u32 directIdx = (u32)localRow * outW + (u32)localCol;
                    DataOut[directIdx] = (u16)val;
                }
            }
        }
    }

    Params->imgW = inW; Params->imgH = inH;

    free(lineBuffers);
    if (extended) free(newDataImg);
    if (coeffs != Params->FilterCoeffs) free(coeffs);
}