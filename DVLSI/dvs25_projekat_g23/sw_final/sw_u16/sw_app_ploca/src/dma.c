#include "dma.h"
#include <xil_printf.h>
#include <xstatus.h>
#include <xil_util.h> 

XAxiDma         AxiDma;
volatile u32    TxDone;
volatile u32    RxDone;

static void TxIntrHandler(void *Callback)
{
    u32       IrqStatus;
    XAxiDma  *AxiDmaInst = (XAxiDma *)Callback;

    IrqStatus = XAxiDma_IntrGetIrq(AxiDmaInst, XAXIDMA_DMA_TO_DEVICE);
    XAxiDma_IntrAckIrq(AxiDmaInst, IrqStatus, XAXIDMA_DMA_TO_DEVICE);

    if (IrqStatus & XAXIDMA_IRQ_IOC_MASK) {
        TxDone = 1;
    }
}

static void RxIntrHandler(void *Callback)
{
    u32       IrqStatus;
    XAxiDma  *AxiDmaInst = (XAxiDma *)Callback;

    IrqStatus = XAxiDma_IntrGetIrq(AxiDmaInst, XAXIDMA_DEVICE_TO_DMA);
    XAxiDma_IntrAckIrq(AxiDmaInst, IrqStatus, XAXIDMA_DEVICE_TO_DMA);

    if (IrqStatus & XAXIDMA_IRQ_IOC_MASK) {
        RxDone = 1;
    }
}

int DmaReset(XAxiDma *AxiDmaPtr)
{
    XAxiDma_Reset(AxiDmaPtr);
    u32 timeout = 10000;
    while (timeout--) {
        if (XAxiDma_ResetIsDone(AxiDmaPtr)) {
            return XST_SUCCESS;
        }
    }
    xil_printf("ERROR: DMA Reset timeout!\r\n");
    return XST_FAILURE;
}

int DmaConfigure(XAxiDma_Config *AxiDmaConfigPtr, XAxiDma *AxiDmaPtr)
{
    int Status;

    Status = XAxiDma_CfgInitialize(AxiDmaPtr, AxiDmaConfigPtr);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: DMA init failed %d\r\n", Status);
        return XST_FAILURE;
    }

    if (XAxiDma_HasSg(AxiDmaPtr)) {
        xil_printf("ERROR: DMA configured in SG mode (expected Simple)\r\n");
        return XST_FAILURE;
    }

    Status = XSetupInterruptSystem(AxiDmaPtr, &TxIntrHandler,
                                   AxiDmaConfigPtr->IntrId[0],
                                   AxiDmaConfigPtr->IntrParent,
                                   XINTERRUPT_DEFAULT_PRIORITY);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: DMA TX interrupt setup failed\r\n");
        return XST_FAILURE;
    }

    Status = XSetupInterruptSystem(AxiDmaPtr, &RxIntrHandler,
                                   AxiDmaConfigPtr->IntrId[1],
                                   AxiDmaConfigPtr->IntrParent,
                                   XINTERRUPT_DEFAULT_PRIORITY);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: DMA RX interrupt setup failed\r\n");
        return XST_FAILURE;
    }

    XAxiDma_IntrEnable(AxiDmaPtr, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
    XAxiDma_IntrEnable(AxiDmaPtr, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);

    xil_printf("DMA configured: MaxTransferLen=%u DataWidth=%u\r\n",
               AxiDmaPtr->RxBdRing[0].MaxTransferLen,
               AxiDmaPtr->RxBdRing[0].DataWidth);

    return XST_SUCCESS;
}

int DmaStartTransfers(XAxiDma *AxiDmaPtr,
                      u8  *TxBuffer, u32 TxSize,
                      u16 *RxBuffer, u32 RxSize)
{
    int Status;

    u32 maxLen = AxiDmaPtr->RxBdRing[0].MaxTransferLen;
    if (RxSize > maxLen) {
        xil_printf("ERROR: RxSize=%u > MaxTransferLen=%u — Nedovoljna veličina u AXI dma hardware-u!\r\n",
                   RxSize, maxLen);
        return XST_FAILURE;
    }
    if (TxSize > AxiDmaPtr->TxBdRing.MaxTransferLen) {
        xil_printf("ERROR: TxSize=%u > MaxTransferLen=%u\r\n",
                   TxSize, AxiDmaPtr->TxBdRing.MaxTransferLen);
        return XST_FAILURE;
    }

    /* Da bi slao DMA -> Ram -> Accelerator => Prebaci podatke iz cache => Ram */
    Xil_DCacheFlushRange((UINTPTR)TxBuffer, TxSize);
    
    /* Accelerator => Ram: invalidiraj cache da se ne bi dobila stara vrednost u ramu povucena iz rama */
    Xil_DCacheInvalidateRange((UINTPTR)RxBuffer, RxSize);

    TxDone = 0;
    RxDone = 0;

    /* RX mora biti pokrenut PRE TX */
    Status = XAxiDma_SimpleTransfer(AxiDmaPtr, (UINTPTR)RxBuffer,
                                    RxSize, XAXIDMA_DEVICE_TO_DMA);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: RX DMA start failed=%d CR=0x%08X SR=0x%08X\r\n",
                   Status,
                   XAxiDma_ReadReg(AxiDmaPtr->RegBase, XAXIDMA_RX_OFFSET + XAXIDMA_CR_OFFSET),
                   XAxiDma_ReadReg(AxiDmaPtr->RegBase, XAXIDMA_RX_OFFSET + XAXIDMA_SR_OFFSET));
        return XST_FAILURE;
    }

    Status = XAxiDma_SimpleTransfer(AxiDmaPtr, (UINTPTR)TxBuffer,
                                    TxSize, XAXIDMA_DMA_TO_DEVICE);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: TX DMA start failed=%d CR=0x%08X SR=0x%08X\r\n",
                   Status,
                   XAxiDma_ReadReg(AxiDmaPtr->RegBase, XAXIDMA_TX_OFFSET + XAXIDMA_CR_OFFSET),
                   XAxiDma_ReadReg(AxiDmaPtr->RegBase, XAXIDMA_TX_OFFSET + XAXIDMA_SR_OFFSET));
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

int DmaWaitTransfers(volatile u32 *TxFlag, volatile u32 *RxFlag, u32 Timeout)
{
    int Status;

    Status = Xil_WaitForEventSet(Timeout, 1, TxFlag);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: TX transfer timeout! TX SR=0x%08X\r\n",
                   XAxiDma_ReadReg(AxiDma.RegBase, XAXIDMA_TX_OFFSET + XAXIDMA_SR_OFFSET));
        return XST_FAILURE;
    }

    Status = Xil_WaitForEventSet(Timeout, 1, RxFlag);
    if (Status != XST_SUCCESS) {
        xil_printf("ERROR: RX transfer timeout! RX SR=0x%08X\r\n",
                   XAxiDma_ReadReg(AxiDma.RegBase, XAXIDMA_RX_OFFSET + XAXIDMA_SR_OFFSET));
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}