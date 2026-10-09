#ifndef DMA_UTILS_H
#define DMA_UTILS_H

#include "xaxidma.h"
#include "xinterrupt_wrap.h"
#include <xil_types.h>
#include <xil_cache.h>

#define DMA_TRANSFER_TIMEOUT 100000u

extern XAxiDma          AxiDma; /* Negde je */
extern volatile u32     TxDone;
extern volatile u32     RxDone;

int DmaConfigure(XAxiDma_Config *AxiDmaConfigPtr, XAxiDma *AxiDmaPtr);
int DmaReset(XAxiDma *AxiDmaPtr);
int DmaStartTransfers(XAxiDma *AxiDmaPtr, u8 *TxBuffer, u32 TxSize, u16 *RxBuffer, u32 RxSize);
int DmaWaitTransfers(volatile u32 *TxFlag, volatile u32 *RxFlag, u32 Timeout);

#endif 