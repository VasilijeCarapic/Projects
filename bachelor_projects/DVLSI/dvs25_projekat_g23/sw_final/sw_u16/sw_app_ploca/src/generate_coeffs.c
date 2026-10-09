#include "generate_coeffs.h"
#include <math.h>
#include <stdio.h>
#include "xil_printf.h"

#define PI 3.14159265358979323846
#define Q15 32768

static void generateBoxIdeal(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float coeff_scale);
static void generateGauss(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float sigma, float coeff_scale);
static void generateLog(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float sigma, float coeff_scale);
static void generateSharpenBox(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float k, float coeff_scale);
static void generateSharpenGauss(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float sigma, float k, float coeff_scale);
static void generateSobel(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float sigma, float k, float coeff_scale);

void generateCoeffitients(ProcessingParams* params, filt_type_t FILT_TYPE, float sigma, float k, float inv_scale)
{
    int16_t coeff_matrix[MAX_WIN][MAX_WIN] = { 0 };

    switch (FILT_TYPE) {
    case LOG:
        generateLog(params, coeff_matrix, sigma, inv_scale);
        break;
    case GAUSS:
        generateGauss(params, coeff_matrix, sigma, inv_scale);
        break;
    case BOX_IDEAL:
        generateBoxIdeal(params, coeff_matrix, inv_scale);
        break;
    case SHARPEN_BOX:
        generateSharpenBox(params, coeff_matrix, k, inv_scale);
        break;
    case SHARPEN_GAUSS:
        generateSharpenGauss(params, coeff_matrix, sigma, k, inv_scale);
        break;
    case SOBEL: 
        generateSobel(params, coeff_matrix, sigma, k, inv_scale);
        break;
    default:
        return;
    }

    for (int row = 0; row < MAX_WIN; row++)
        for (int col = 0; col < MAX_WIN; col++)
            params->FilterCoeffs[row * MAX_WIN + col] = coeff_matrix[row][col];
}

static void generateBoxFloat(ProcessingParams* params, float boxf_matrix[MAX_WIN][MAX_WIN]) {
    int radius = params->FilterRadius;
    int size = 2 * radius + 1;
    float val = 1.0f / (float)(size * size);

    for (int x = 0; x < size; x++)
        for (int y = 0; y < size; y++)
            boxf_matrix[x][y] = val;
}

static void generateBoxIdeal(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float coeff_scale)
{
    int radius = params->FilterRadius;
    int size = 2 * radius + 1;
    float boxf_matrix[MAX_WIN][MAX_WIN] = { 0 };

    generateBoxFloat(params, boxf_matrix);

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            int32_t out_val = (int32_t)roundf(boxf_matrix[x][y] * coeff_scale * Q15);
            if (out_val > Q15 - 1) out_val = Q15 - 1;
            if (out_val < -Q15)    out_val = -Q15;
            coeff_matrix[x][y] = (int16_t)out_val;
        }
    }
}

static void generateGaussFloat(ProcessingParams* params, float gauss_f[MAX_WIN][MAX_WIN], float sigma)
{
    int radius = params->FilterRadius;
    int size = 2 * radius + 1;
    float sigma_sq = sigma * sigma;
    float sum = 0.0f;

    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            /* e**(-( x ** 2 + y**2) / 2 * sigma_sq))*/
            float val = expf(-(float)(x * x + y * y) / (2.0f * sigma_sq));
            gauss_f[x + radius][y + radius] = val;
            sum += val;
        }
    }

    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            gauss_f[i][j] /= sum;
}

static void generateGauss(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float sigma, float coeff_scale)
{
    int radius = params->FilterRadius;
    int size = 2 * radius + 1;
    float gauss_f[MAX_WIN][MAX_WIN] = { 0 };

    generateGaussFloat(params, gauss_f, sigma);

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int32_t out_val = (int32_t)roundf(gauss_f[i][j] * coeff_scale * Q15);
            if (out_val > Q15 - 1) out_val = Q15 - 1;
            if (out_val < -Q15)    out_val = -Q15;
            coeff_matrix[i][j] = (int16_t)out_val;
        }
    }
}

static void generateLog(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float sigma, float coeff_scale)
{
    int radius = params->FilterRadius;
    float factor_f = -(1 / (PI * powf(sigma, 4.0f)));

    for (int x = -radius; x <= radius; x++) {
        for (int y = -radius; y <= radius; y++) {
            float dst_norm = (float)(x * x + y * y) / (2.0f * sigma * sigma);
            float res = factor_f * (1.0f - dst_norm) * expf(-dst_norm);

            int32_t out_val = (int32_t)roundf(res * coeff_scale * Q15);
            if (out_val > Q15 - 1) out_val = Q15 - 1;
            if (out_val < -Q15)    out_val = -Q15;
            coeff_matrix[x + radius][y + radius] = (int16_t)out_val;
        }
    }
}

static void generateSharpenBox(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float k, float coeff_scale)
{
    int radius = params->FilterRadius;
    int size = 2 * radius + 1;
    float boxf_matrix[MAX_WIN][MAX_WIN] = { 0 };

    generateBoxFloat(params, boxf_matrix);

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            float dirac = (x == radius && y == radius) ? 1.0f : 0.0f;
            float val = ((1.0f + k) * dirac) - (k * boxf_matrix[x][y]);

            int32_t out_val = (int32_t)roundf(val * coeff_scale * Q15);
            if (out_val > Q15 - 1) out_val = Q15 - 1;
            if (out_val < -Q15)    out_val = -Q15;

            coeff_matrix[x][y] = (int16_t)out_val;
        }
    }
}

static void generateSharpenGauss(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float sigma, float k, float coeff_scale)
{
    int radius = params->FilterRadius;
    int size = 2 * radius + 1;
    float gauss_f[MAX_WIN][MAX_WIN] = { 0 };

    generateGaussFloat(params, gauss_f, sigma); // <-- float, bez prolaska kroz int16

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            float dirac = (x == radius && y == radius) ? 1.0f : 0.0f;
            float val = ((1.0f + k) * dirac) - (k * gauss_f[x][y]);

            int32_t out_val = (int32_t)roundf(val * coeff_scale * Q15);
            if (out_val > Q15 - 1) out_val = Q15 - 1;
            if (out_val < -Q15)    out_val = -Q15;

            coeff_matrix[x][y] = (int16_t)out_val;
        }
    }
}

static float binomial(int n, int k)
{
    if (k < 0 || k > n) return 0.0f;
    float res = 1.0f;
    for (int i = 0; i < k; i++)
        res = res * (n - i) / (i + 1);
    return res;
}
static void generateSobel(ProcessingParams* params, int16_t coeff_matrix[MAX_WIN][MAX_WIN], float sigma, float k, float coeff_scale)
{
    (void)sigma; (void)k; // scale = 8 , inv_scale = 1/8 da se matchuje sa hardcodovanim 

    int radius = params->FilterRadius;
    int size = 2 * radius + 1;

    for (int x = 0; x < size; x++) {
        /* zamenim x i y za drugi smer operatora sobela */
        float smooth = binomial(size - 1, x);
        for (int y = 0; y < size; y++) {
            /*zamenim x i y za drugi smer operatora sobela*/
            float deriv = binomial(size - 2, y - 1) - binomial(size - 2, y);
            
            /* ako ubacim - ispred invertovace se signali*/
            float base_val = smooth * deriv;

            // Ista logika kao kod Gauss-a: base_val * scale * Q15
            float val = base_val * coeff_scale * Q15;

            // Dodaj saturaciju da ne pukne int16 ako scale pretera
            if (val > 32767.0f) val = 32767.0f;
            if (val < -32768.0f) val = -32768.0f;

            coeff_matrix[x][y] = (int16_t)roundf(val);
        }
    }
}

void printCoeffs(int16_t s16_arr[MAX_WIN * MAX_WIN])
{
    for (size_t i = 0; i < MAX_WIN; i++)
    {
        for (size_t j = 0; j < MAX_WIN; j++) {
            // Dodao sam %5d da poravnaš brojeve ako su različitih dužina
            xil_printf("%5d ", s16_arr[i * MAX_WIN + j]);
        }
        xil_printf("\r\n"); // Neki terminali zahtevaju \r\n za novi red
    }
    xil_printf("\r\n");
}