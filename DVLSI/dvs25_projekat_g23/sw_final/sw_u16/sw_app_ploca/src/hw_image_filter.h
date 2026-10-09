#ifndef IMAGE_FILTER_HW_H
#define IMAGE_FILTER_HW_H

#include "xil_types.h"
#include <xstatus.h>
#include "acc_image_filter.h"

/**
 * Vraca XST_FAILURE ako je 2 * radius >= imgW ili >= imgH kada je Border == 0b00 / 0b01 .
 */
int calcOutWH(u16 *outW, u16 *outH, ProcessingParams *params);

/**
 * Konfiguracija akceleratora i DMA-a, pokretanje HW filtriranja.
 */
int imageFilterHW(u8 *DataIn, u16 *DataOut, ProcessingParams Params);

/**
 * Uporedjivanje HW izlaza sa SW referentnim rezultatom.
 */
int CheckData(u16 *DataOut, u16 *ReferentBuffer, ProcessingParams Params, u32 TotalWords);

/**
 * Racuna broj piksela/reci izlazne slike na osnovu Border moda, Mode, Bypass i radijusa.
 */
int calcOutputSize(u32 *outputSize, ProcessingParams *params);
int calcOutputWords(u32 *outputWords, ProcessingParams *params);

#endif 