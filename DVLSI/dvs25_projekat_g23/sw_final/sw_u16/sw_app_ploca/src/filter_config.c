#include "filter_config.h"
#include <stdio.h>

static const s16 logNoScale[5][9] = {
    {0, 0, 4, 16, 24, 16, 4, 0, 0},
    {0, 10, 86, 281, 406, 281, 86, 10, 0},
    {4, 86, 573, 1284, 1412, 1284, 573, 86, 4},
    {16, 281, 1284, 0, -3163, 0, 1284, 281, 16},
    {24, 406, 1412, -3163, -10430, -3163, 1412, 406, 24}
};

static const s16 gaussNoScale[5][9] = {
    {0, 0, 0, 1, 2, 1, 0, 0 ,0},
    {0, 1 , 8, 35, 58, 35, 8, 1, 0},
    {0, 8, 96, 428, 706, 428, 96, 8, 0},
    {1, 35, 428, 1919, 3163, 1919, 428, 35, 1},
    {2, 58, 706, 3163, 5215, 3163, 706, 58, 2}
};

static const s16 gaussScale1By4[5][9] = {
    {0, 0, 1, 4, 7, 4, 1, 0 ,0},
    {0, 3 , 31, 141, 232, 141, 31, 3, 0},
    {1, 31, 382, 1712, 2823, 1712, 382, 31, 1},
    {4, 141, 1712, 7674, 12653, 7674, 1712, 141, 4},
    {7, 232, 2823, 12653, 20861, 12653, 2823, 232, 7}
};

static const s16 gaussSharpenR4[5][9] = {
    {0, 0, 0, -1, -1, -1, 0, 0, 0},
    {0, 0, -4, -18, -29, -18, -4, 0, 0},
    {0, -4, -48, -214, -353, -214, -48, -4, 0},
    {-1, -18, -214, -959, -1582, -959, -214, -18, -1},
    {-1, -29, -353, -1582, 30160, -1582, -353, -29, -1},
};

static const s16 gaussSharpenR2[5][9] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, -49, -218, -359, -218, -49, 0, 0},
    {0, 0, -218, -977, -1611, -977, -218, 0, 0},
    {0, 0, -359, -1611, 30112, -1611, -359, 0, 0}
};

static const s16 sobelR1S1[81] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -4096, 0, 4096, 0, 0, 0,
    0, 0, 0, -8192, 0, 8192, 0, 0, 0,
    0, 0, 0, -4096, 0, 4096, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

static const char* filterNames[] = {
    "BOX_S1",
    "BOX_S3",
    "LOG",
    "GAUSS_S1",
    "GAUSS_Q4",
    "SHARP_BOX_R4",
    "SHARP_BOX_R1",
    "SHARP_GAUSS_R4",
    "SHARP_GAUSS_R2"
};

static void fillFilterCoeffs2D(ProcessingParams* Params, const s16 arr[5][9]);
static void shiftTopLeft(ProcessingParams* params);
static void setUniformValue(ProcessingParams* params, int uniformVal);

static void fillFilterCoeffs2D(ProcessingParams* Params, const s16 arr[5][9])
{
    int size = 9;
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            if (col == 0 || col == 8) Params->FilterCoeffs[row * size + col] = arr[0][row];
            else if (col == 1 || col == 7) Params->FilterCoeffs[row * size + col] = arr[1][row];
            else if (col == 2 || col == 6) Params->FilterCoeffs[row * size + col] = arr[2][row];
            else if (col == 3 || col == 5) Params->FilterCoeffs[row * size + col] = arr[3][row];
            else Params->FilterCoeffs[row * size + col] = arr[4][row];
        }
    }
}

static void shiftTopLeft(ProcessingParams* Params)
{
    int R = Params->FilterRadius;
    if (R == 4) return;

    s16 tmpArr[81];

    for (int i = 0; i < 81; i++) {
        tmpArr[i] = Params->FilterCoeffs[i];
        Params->FilterCoeffs[i] = 0;
    }

    int middle = 4;

    for (int r = middle - R; r <= middle + R; r++) {
        for (int c = middle - R; c <= middle + R; c++) {
            int srcIdx = r * 9 + c;
            int dstIdx = (r - (middle - R)) * 9 + (c - (middle - R));
            Params->FilterCoeffs[dstIdx] = tmpArr[srcIdx];
        }
    }
}

static void setUniformValue(ProcessingParams* params, int uniformVal)
{
    for (int i = 0; i < 81; i++) params->FilterCoeffs[i] = (s16)uniformVal;
}

void setFilterConfig(ProcessingParams* Params, FilterConfig config)
{
    Params->FilterRadius = 4;
    Params->FilterCoeffsScale = 4096;

    for (int i = 0; i < 81; i++) Params->FilterCoeffs[i] = 0;

    switch (config) {
    case FILT_BOX_S1:
        setUniformValue(Params, 405); Params->FilterRadius = 4; Params->FilterCoeffsScale = 4096;
        break;

    case FILT_BOX_S3:
        setUniformValue(Params, 1214); Params->FilterRadius = 4; Params->FilterCoeffsScale = 1365;
        break;

    case FILT_LOG:
        fillFilterCoeffs2D(Params, logNoScale); Params->FilterRadius = 4; Params->FilterCoeffsScale = 4096;
        break;

    case FILT_GAUSS_S1:
        fillFilterCoeffs2D(Params, gaussNoScale); Params->FilterRadius = 4; Params->FilterCoeffsScale = 4096;
        break;

    case FILT_GAUSS_S1_Q4:
        fillFilterCoeffs2D(Params, gaussScale1By4); Params->FilterRadius = 4; Params->FilterCoeffsScale = 1024;
        break;

    case FILT_SHARP_GAUSS_R2:
        fillFilterCoeffs2D(Params, gaussSharpenR2); Params->FilterRadius = 2; Params->FilterCoeffsScale = 8192;
        break;

    case FILT_SHARP_GAUSS_R4:
        fillFilterCoeffs2D(Params, gaussSharpenR4); Params->FilterRadius = 4; Params->FilterCoeffsScale = 8192;
        break;

    case FILT_SHARP_BOX_R4:
        for (int i = 0; i < 81; i++) {
            Params->FilterCoeffs[i] = (i == 40) ? 32566 : -202; Params->FilterCoeffsScale = 8192;
        }
        break;

    case FILT_SHARP_BOX_R1:
        for (int i = 0; i < 81; i++) {
            switch (i) {
            case 30: case 31: case 32: case 39: case 41: case 48: case 49: case 50:
                Params->FilterCoeffs[i] = -1820;
                break;
            case 40:
                Params->FilterCoeffs[i] = 30948;
                break;
            default:
                Params->FilterCoeffs[i] = 0;
            }
        }
        Params->FilterRadius = 1; Params->FilterCoeffsScale = 8192;
        break;
    case FILT_SOBEL_R1: 
        for (size_t i = 0; i < 81; i++) { Params->FilterCoeffs[i] = sobelR1S1[i]; }
        Params->FilterRadius = 4; Params->FilterCoeffsScale = 4096;
        break;
    default:
        break;
    }

    shiftTopLeft(Params);
}

void printSelectedFilter(FilterConfig config)
{
    if ((size_t)config >= (sizeof(filterNames) / sizeof(filterNames[0]))) {
        printf("Selected filter: INVALID\r\n");
        return;
    }
    printf("Selected filter: %s\r\n", filterNames[config]);
}