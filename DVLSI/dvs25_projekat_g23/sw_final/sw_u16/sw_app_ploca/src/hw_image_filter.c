#include "hw_image_filter.h"
#include "acc_ctrl.h"
#include "dma.h"

#include "xparameters.h"
#include <xil_printf.h>
#include <xstatus.h>
#include <xil_cache.h>

/* -----------------------------------------------------------------------
 * calcOutWH
 * Vraca XST_FAILURE ako je 2*FilterRadius >= imgW ili >= imgH kada je Border == 0b00 / 0b01).
 * ----------------------------------------------------------------------- */
int calcOutWH(u16 *outW, u16 *outH, ProcessingParams *params)
{
    u16 R = params->FilterRadius;

    if (params->Bypass == 1 || params->Border == 0b10 || params->Border == 0b11) {
        *outW = params->imgW;
        *outH = params->imgH;
        return XST_SUCCESS;
    }

    switch (params->Border) {
        case 0b00: case 0b01:
            if ((u32)(2 * R) >= (u32)params->imgW || (u32)(2 * R) >= (u32)params->imgH) {
                xil_printf("ERROR: FilterRadius=%u prevelik za sliku %ux%u (Border=0x0)\r\n",
                           R, params->imgW, params->imgH);
                return XST_FAILURE;
            }
            *outW = params->imgW - 2 * R;
            *outH = params->imgH - 2 * R;
            return XST_SUCCESS;
        default:
            xil_printf("WARN: Unknown Border=0x%X, using full size\r\n", params->Border);
            *outW = params->imgW;
            *outH = params->imgH;
            return XST_SUCCESS;
    }
}

/* -----------------------------------------------------------------------
 * calcOutputWords
 * Racuna broj 16-bitnih reci u izlaznoj slici za jednu ravan na osnovu Mode, Bypass,
 * Border i radijusa.
 * ----------------------------------------------------------------------- */
int calcOutputWords(u32 *outputWords, ProcessingParams *params)
{
    u16 outW, outH;

    if (calcOutWH(&outW, &outH, params) != XST_SUCCESS) {
        *outputWords = 0;
        return XST_FAILURE;
    }

    /* ako je Mode = 0 ili bypass imamo duplo manje izlaznih reci */
    if (params->Mode == 0 || params->Bypass == 1) {
        u32 wordsPerRow = ((u32)outW + 1) / 2;
        *outputWords = wordsPerRow * (u32)outH;
    } else {
        *outputWords = (u32)outW * (u32)outH;
    }
    return XST_SUCCESS;
}

int calcOutputSize(u32 *outputSize, ProcessingParams *params)
{
    /* Izracunaj broj izlaznih reci za jednu ravan funkcija */
    return calcOutputWords(outputSize, params);
}

/* -----------------------------------------------------------------------
 * imageFilterHW
 * ----------------------------------------------------------------------- */
int imageFilterHW(u8 *DataIn, u16 *DataOut, ProcessingParams Params)
{
    int             Status;
    XAxiDma_Config *AxiDmaConfigPtr;
    u32             TxSize, RxSize;
    u16             outW, outH;

    TxSize = (u32)Params.imgH * (u32)Params.imgW * (u32)sizeof(u8);

    if (calcOutWH(&outW, &outH, &Params) != XST_SUCCESS) {
        return XST_FAILURE;
    }

    if (Params.Mode == 0 || Params.Bypass == 1) {
        u32 wordsPerRow = ((u32)outW + 1) / 2;
        RxSize = wordsPerRow * (u32)outH * (u32)sizeof(u16);
    } else {
        RxSize = (u32)outW * (u32)outH * (u32)sizeof(u16);
    }

    xil_printf("imageFilterHW: TxSize=%u RxSize=%u\r\n", TxSize, RxSize);

    /* Konfigurisi akcelerator */
    Status = AccConfigure(XPAR_ACC_IMAGE_FILTER_0_BASEADDR, Params);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: AccConfigure failed\r\n");
        return XST_FAILURE;
    }

    /* Lookup i inicijalizacija DMA */
    AxiDmaConfigPtr = XAxiDma_LookupConfig(XPAR_XAXIDMA_0_BASEADDR);
    if (!AxiDmaConfigPtr) {
        xil_printf("ERROR: DMA config lookup failed\r\n");
        return XST_FAILURE;
    }

    Status = DmaConfigure(AxiDmaConfigPtr, &AxiDma);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: DmaConfigure failed\r\n");
        return XST_FAILURE;
    }

    TxDone = 0;
    RxDone = 0;

    /* Zapocni dma transfere */
    Status = DmaStartTransfers(&AxiDma, DataIn, TxSize, DataOut, RxSize);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: DmaStartTransfers failed\r\n");
        return XST_FAILURE;
    }

    Status = DmaWaitTransfers(&TxDone, &RxDone, DMA_TRANSFER_TIMEOUT);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: DmaWaitTransfers failed\r\n");
        return XST_FAILURE;
    }


    Xil_DCacheInvalidateRange((UINTPTR)DataOut, RxSize);


    XDisconnectInterruptCntrl(AxiDmaConfigPtr->IntrId[0], AxiDmaConfigPtr->IntrParent);
    XDisconnectInterruptCntrl(AxiDmaConfigPtr->IntrId[1], AxiDmaConfigPtr->IntrParent);

    return XST_SUCCESS;
}

/* -----------------------------------------------------------------------
 * CheckData
 * Poredi 16-bitne reci iz HW-a sa spakovanim SW referentnim podacima
 * ----------------------------------------------------------------------- */
int CheckData(u16 *DataOut, u16 *ReferentBuffer, ProcessingParams Params, u32 TotalWords)
{
    (void)Params;
    u32 errorCount = 0;

    /* Sracunam totalWords sve ukupne reci i onda rec po rec check  */
    for (u32 i = 0; i < TotalWords; i++) {
        if (DataOut[i] != ReferentBuffer[i]) {
            if (errorCount < 10) {
                xil_printf("MISMATCH index %u: HW=0x%04X SW=0x%04X (HW_dec=%d SW_dec=%d)\r\n",
                           i, DataOut[i], ReferentBuffer[i], (int)DataOut[i], (int)ReferentBuffer[i]);
            }
            errorCount++;
        }
    }

    if (errorCount > 0) {
        xil_printf("Ukupno gresaka: %u od %u reci.\r\n", errorCount, TotalWords);
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}