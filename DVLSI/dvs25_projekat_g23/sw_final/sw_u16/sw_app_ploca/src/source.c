/**
 * @file source.c (main.c)
 * @brief DVS 2025/2026 - Ubrzanje filtriranja slike 
 */

#include "xparameters.h"   
#include <xil_printf.h>
#include <xstatus.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "xtmrctr.h"
#include <xil_cache.h>

#include "acc_image_filter.h"
#include "filter_config.h"

#include "hw_image_filter.h"
#include "generate_coeffs.h"
#include "helper_functions.h"
#include "dma.h"

#define TMR_CNT_0   0
#define Q12         4096.0f

/* Maksimalni parametri slike */
#define C_MAX_IMG_W   65536
#define C_MAX_IMG_H   65536
#define C_MAX_PLANES  3

/* Radius */
#define MIN_RADIUS      0 
#define MAX_RADIUS      4

/* mode + bypass */
#define MIN_BYPASS      0 
#define MAX_BYPASS      1

#define MIN_MODE        0 
#define MAX_MODE        1

#define MIN_FILT_TYPE   0 
#define MAX_FILT_TYPE   5 // 5 = sobel

int main(void)
{
    int               Status;
    int               Stop = 0;

    u8                *DataIn     = NULL, *DataInRaw = NULL;
    u16               *DataOutRaw = NULL, *DataOut   = NULL;
    u16               *ReferentBufferRaw = NULL, *ReferentBuffer = NULL;

    u8                *currentSwPlane    = NULL;
    u16               *currentSwOutPlane = NULL;
    u8                *currentHwPlane    = NULL;
    u16               *currentHwOutPlane = NULL;

    XTmrCtr           AxiTimerInst;
    ProcessingParams  Params;

    u32               InputSize, TotalOutputWords, endVal, outputPlaneWords, inputPlaneSize;
    u32               hwOutPlaneIncrement, swOutPlaneIncrement;
    u16               planes, pixelSize;
    u16               outImgW, outImgH;
    u16               p;

    u64               swTime, hwTime;  
    size_t            dumpBytes;

    float scale, inv_scale;
    filt_type_t filterType = SHARPEN_BOX;

    /* ------------------------------------------------------------------ */
    /* 1. Inicijalizacija AXI Tajmera                                     */
    /* ------------------------------------------------------------------ */
    Status = XTmrCtr_Initialize(&AxiTimerInst, XPAR_AXI_TIMER_0_BASEADDR);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: Inicijalizacija AXI tajmera neuspesna!\r\n");
        return XST_FAILURE;
    }
    XTmrCtr_SetOptions(&AxiTimerInst, TMR_CNT_0, 0);

    /* ------------------------------------------------------------------ */
    /* 2. Glavna petlja za obradu slika i parametara                      */
    /* ------------------------------------------------------------------ */
    while (Stop != 1) {
        DataInRaw = NULL;
        DataOutRaw = NULL;
        ReferentBufferRaw = NULL;

        /* ---------------------------------------------------------------
         * Unos parametara procesiranja (Formatiran unos sa novim redom)
         * ------------------------------------------------------------- */
        xil_printf("\r\n--- Unos parametara procesiranja ---\r\n");
        
        xil_printf("Tip Filtra (0=Box, 1=Gauss, 2=Log, 3=SharpenBox, 4=SharpenGauss, 5=Sobel):");
        filterType = (filt_type_t)validateInt(MIN_FILT_TYPE, MAX_FILT_TYPE);

        xil_printf("\r\nimgH (visina u px):");
        Params.imgH = (u16)validateInt(1, C_MAX_IMG_H);

        xil_printf("\r\nimgW (sirina u px):");
        Params.imgW = (u16)validateInt(1, C_MAX_IMG_W);

        xil_printf("\r\nplanes (broj ravni 1-3):");
        planes = (u16)validateInt(1, C_MAX_PLANES);

        xil_printf("\r\nMode (0 ili 1):");
        Params.Mode = (u8)validateInt(MIN_MODE, MAX_MODE);

        xil_printf("\r\nBypass (0 ili 1):");
        Params.Bypass = (u8)validateInt(MIN_BYPASS, MAX_BYPASS);
        
        xil_printf("\r\nBorder (0/1 = CROP, 2=NEAREST, 3=CONSTANT):");
        Params.Border = (u8)validateInt(0, 3);

        if (Params.Border == 3) {
            xil_printf("\r\nBorder Value (0-255):");
            Params.BorderVal = (u8)validateInt(0, 255);
        } else {
            Params.BorderVal = 0;
        }

        xil_printf("\r\nFilterRadius (0,1,2,3,4):");
        Params.FilterRadius = (u16)validateInt(MIN_RADIUS, MAX_RADIUS);

        xil_printf("\r\nScale (float npr. 2.0):");
        scale = validateFloat(0.0001f, 100.0f);

        /* Generisanje koeficijenata i proračun inv_scale */
        inv_scale = 1.0f / scale;
        Params.FilterCoeffsScale = (uint16_t)(scale * Q12);
        generateCoeffitients(&Params, filterType, 1.0f, 1.0f, inv_scale);

        /* ---------------------------------------------------------------
         * Proračun veličina bafera i izlaznih dimenzija u rečima (u16)
         * ------------------------------------------------------------- */
        inputPlaneSize   = (u32)Params.imgW * (u32)Params.imgH;
        /* izlazne reci u jednoj ravni*/
        calcOutputWords(&outputPlaneWords, &Params);

        /* ulazna velicina cela*/
        InputSize        = planes * inputPlaneSize;
        
        /*izlazna velicina cela*/
        TotalOutputWords = planes * outputPlaneWords;

        /*sracunaj izlazne dimenzije*/
        calcOutputDims(&outImgW, &outImgH, &Params);

        xil_printf("\r\nParametri prihvaceni:\r\n");
        xil_printf("  Ulazna slika : %ux%u px, planes=%u (%u B)\r\n", Params.imgW, Params.imgH, planes, InputSize * (u32)sizeof(u8));
        xil_printf("  Izlazna slika: %ux%u px, planes=%u (%u reci / %u B)\r\n", 
                   outImgW, outImgH, planes, TotalOutputWords, TotalOutputWords * (u32)sizeof(u16));

        /* Alokacija ulaznog bafera pre mwr komande */
        DataInRaw = (u8*) malloc(((size_t)InputSize + 8) * sizeof(u8));
        if (!DataInRaw) {
            xil_printf("ERROR: Alokacija DataInRaw neuspesna!\r\n");
            break;
        }

        xil_printf("\r\n=====================================================\r\n");
        xil_printf("Header (mwr target) = 0x%08X\r\n", (unsigned int)(UINTPTR)DataInRaw);
        xil_printf("Pritisni ENTER da nastavis dalje: \r\n");
        flush_stdin_line();

        /* Invalidate cache za novoučitane podatke sa ploce/DRAM-a */
        Xil_DCacheInvalidateRange((UINTPTR)DataInRaw, (InputSize + 8));

        /* ---------------------------------------------------------------
         * Parsiranje 8-bajtnog zaglavlja slike iz ulaznog bafera
         * ------------------------------------------------------------- */
        u16 loadedH         = (u16)((DataInRaw[1] << 8) | DataInRaw[0]);
        u16 loadedW         = (u16)((DataInRaw[3] << 8) | DataInRaw[2]);
        u16 loadedPlanes    = (u16)((DataInRaw[5] << 8) | DataInRaw[4]);
        u16 loadedPixelSize = (u16)((DataInRaw[7] << 8) | DataInRaw[6]);

        xil_printf("Ucitano iz zaglavlja: %ux%u px, planes=%u, pixelSize=%u B\r\n",
                   loadedW, loadedH, loadedPlanes, loadedPixelSize);

        if (Params.imgH == loadedH && Params.imgW == loadedW && planes == loadedPlanes) {
            Params.imgH = loadedH;
            Params.imgW = loadedW;
            planes      = loadedPlanes;
            inputPlaneSize   = (u32)Params.imgW * (u32)Params.imgH;
            calcOutputWords(&outputPlaneWords, &Params);
            InputSize        = planes * inputPlaneSize;
            TotalOutputWords = planes * outputPlaneWords;
            calcOutputDims(&outImgW, &outImgH, &Params);
        } else {
            /*nisu isti podaci u headeru i korisnik sta je slao*/
            xil_printf("\r\nERROR: Header i uneti parametri se razlikuju!\r\n");
            goto loop_cleanup;
        }

        DataIn = DataInRaw + 8; /* Pikseli počinju odmah iza 8-bajtnog zaglavlja */

        /* Alokacija izlaznih bafera */
        DataOutRaw = (u16 *) malloc(((size_t)TotalOutputWords + 4) * sizeof(u16));
        if (!DataOutRaw) {
            xil_printf("ERROR: Alokacija DataOutRaw neuspesna!\r\n");
            goto loop_cleanup;
        }
        xil_printf("DataOut = 0x%08X\r\n", (unsigned int)(UINTPTR)DataOutRaw);
        
        ReferentBufferRaw = (u16 *) malloc(((size_t)TotalOutputWords + 4) * sizeof(u16));
        if (!ReferentBufferRaw) {
            xil_printf("ERROR: Alokacija ReferentBufferRaw neuspesna!\r\n");
            goto loop_cleanup;
        }
        xil_printf("ReferentBuffer = 0x%08X\r\n", (unsigned int)(UINTPTR)ReferentBufferRaw);
        
        pixelSize = (u16)calculatePxSize(&Params);

        /* Pisanje izlaznog zaglavlja */
        DataOutRaw[0] = outImgH;
        DataOutRaw[1] = outImgW;
        DataOutRaw[2] = planes;
        DataOutRaw[3] = pixelSize;
        memcpy(ReferentBufferRaw, DataOutRaw, 4 * sizeof(u16));

        DataOut        = DataOutRaw + 4;
        ReferentBuffer = ReferentBufferRaw + 4;

        /* ---------------------------------------------------------------
         * SW Filtriranje 
         * ------------------------------------------------------------- */
        Xil_DCacheFlushRange((UINTPTR)DataIn, InputSize * sizeof(u8));

        XTmrCtr_Reset(&AxiTimerInst, TMR_CNT_0);
        XTmrCtr_Start(&AxiTimerInst, TMR_CNT_0);

        currentSwPlane    = DataIn;
        currentSwOutPlane = ReferentBuffer;

        swOutPlaneIncrement = outputPlaneWords;

        for (p = 0; p < planes; p++) {
            imageFilterSW(currentSwPlane, currentSwOutPlane, &Params);
            currentSwPlane    += inputPlaneSize;
            currentSwOutPlane += swOutPlaneIncrement;
        }

        XTmrCtr_Stop(&AxiTimerInst, TMR_CNT_0);
        endVal = XTmrCtr_GetValue(&AxiTimerInst, TMR_CNT_0);
        swTime = ((u64)endVal * 1000000ULL) / (u64)XPAR_AXI_TIMER_0_CLOCK_FREQUENCY;
        xil_printf("SW Vreme filtriranja: %u us\r\n", (unsigned int)swTime);

        /* ---------------------------------------------------------------
         * HW Filtriranje (Akcelerator)
         * ------------------------------------------------------------- */
        XTmrCtr_Reset(&AxiTimerInst, TMR_CNT_0);
        XTmrCtr_Start(&AxiTimerInst, TMR_CNT_0);

        hwOutPlaneIncrement = outputPlaneWords;

        Status = XST_SUCCESS;
        currentHwPlane    = DataIn;
        currentHwOutPlane = DataOut;

        for (p = 0; p < planes; p++) {
            Status = imageFilterHW(currentHwPlane, currentHwOutPlane, Params);
            if (Status != XST_SUCCESS) { xil_printf("GRESKA: HW obrada neuspesna na ravni %u!\r\n", p); break; }
            currentHwPlane    += inputPlaneSize;
            currentHwOutPlane += hwOutPlaneIncrement;
        }

        XTmrCtr_Stop(&AxiTimerInst, TMR_CNT_0);
        endVal = XTmrCtr_GetValue(&AxiTimerInst, TMR_CNT_0);
        hwTime = ((u64)endVal * 1000000ULL) / (u64)XPAR_AXI_TIMER_0_CLOCK_FREQUENCY;
        xil_printf("HW Vreme filtriranja: %u us\r\n", (unsigned int)hwTime);

        if (Status != XST_SUCCESS) { goto loop_cleanup; }

        /* Odsece na 2 cifre tipa 0.14 ako je 0 */
        if (hwTime > 0ULL && swTime > 0ULL) {
            u64 speedup_scaled = (swTime * 100ULL) / hwTime;   
            u32 speedup_int    = (u32)(speedup_scaled / 100ULL);
            u32 speedup_frac   = (u32)(speedup_scaled % 100ULL);

            xil_printf("Ubrzanje: %u.%02u x\r\n", speedup_int, speedup_frac);
        }

        /* Poredjenje rezultata */
        Xil_DCacheInvalidateRange((UINTPTR)DataOut, (size_t)TotalOutputWords * sizeof(u16));

        Status = CheckData(DataOut, ReferentBuffer, Params, TotalOutputWords);
        if (Status != XST_SUCCESS) {
            xil_printf("STATUS: Provera podataka NEUSPESNA (Data check FAILED)!\r\n");
        } else {
            xil_printf("STATUS: Provera podataka USPESNA (Data check OK)!\r\n");
        }

        /* ---------------------------------------------------------------
         * XSDB komande za isčitavanje rezultata sa ploče
         * ------------------------------------------------------------- */
        dumpBytes = ((size_t)TotalOutputWords + 4) * sizeof(u16);
        xil_printf("  mrd -size b -bin -file \"hw_out.bin\" 0x%08X %u\r\n",
                   (unsigned int)(UINTPTR)DataOutRaw, (unsigned int)dumpBytes);
        xil_printf("  mrd -size b -bin -file \"sw_ref.bin\" 0x%08X %u\r\n",
                   (unsigned int)(UINTPTR)ReferentBufferRaw, (unsigned int)dumpBytes);

loop_cleanup:
        /* Oslobađanje sve alocirane memorije na kraju svake iteracije */
        if (DataInRaw)         { free(DataInRaw);         DataInRaw = NULL; }
        if (DataOutRaw)        { free(DataOutRaw);        DataOutRaw = NULL; }
        if (ReferentBufferRaw) { free(ReferentBufferRaw); ReferentBufferRaw = NULL; }

        xil_printf("\r\nPrekinuti testiranje? [y/Y/n/N]:"); // izbaci \r\n na kraju 
        char c = validateChar(); 
        if(c == 'y' || c == 'Y'){Stop = 1;}
        else{Stop = 0;}
    }

    xil_printf("\r\n--- Program zavrsen. Izlazimo iz maina ---\r\n");
    return XST_SUCCESS;
}