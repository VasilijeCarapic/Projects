#include "acc_ctrl.h"
#include "xil_types.h"
#include <xil_printf.h>
#include <xstatus.h>

int AccConfigure(UINTPTR BaseAddress, ProcessingParams Params)
{
    u16 ctrlVal = 0;
    int win, numCoeffs;

    if (Params.Mode   == 1) { ctrlVal |= (u16)(1 << CTRL_MODE_BIT);   }
    if (Params.Bypass == 1) { ctrlVal |= (u16)(1 << CTRL_BYPASS_BIT); }
    
    /* maska & 0x03 -> izvuci vrednost << pomeri ulevo na tu poziciju */
    ctrlVal |= (u16)(((u16)Params.Border    & 0x03u) << CTRL_BORD_SHIFT);
    
    /* maska & 0xFF -> izvuci vrednost border << pomeri ulevo na poziciju bordera */
    ctrlVal |= (u16)(((u16)Params.BorderVal & 0xFFu) << CTRL_BVAL_SHIFT);

    /* reg_waddr = aw_addr >> 2 uzmu se adrese za upis 0x00, 0x02 * 4, 0x04 * 4 itd... */
    Xil_Out16(BaseAddress + REG_CTRL_ADDR,        ctrlVal);
    Xil_Out16(BaseAddress + REG_RADIUS_ADDR,      (u16)Params.FilterRadius);
    Xil_Out16(BaseAddress + REG_IMG_W_ADDR,       (u16)Params.imgW);
    Xil_Out16(BaseAddress + REG_IMG_H_ADDR,       (u16)Params.imgH);
    Xil_Out16(BaseAddress + REG_COEFF_SCALE_ADDR, (u16)Params.FilterCoeffsScale);

    win       = (int)(2u * (u32)Params.FilterRadius + 1u);
    numCoeffs = win * win;

    /* ako smo presli max broj koeficijenata = 81 bacimo gresku */
    if (numCoeffs > (int)MAX_FILTER_COEFFS) {
        xil_printf("ERROR: numCoeffs=%d\r\n", numCoeffs);
        return XST_FAILURE;
    }

    /* HW fizicki ima 9x9 prozor koeficijenata. Prima koeficijente u gornji levi ugao HW */
    for (int i = 0; i < (int)MAX_FILTER_COEFFS; i++) {
        Xil_Out16(BaseAddress + COEFF_ADDR(i), (u16)Params.FilterCoeffs[i]);
    }

    /* Ispisi sta smo procitali za debug da se proveri upis u koeficijente */
    xil_printf("CTRL=%04X R=%u W=%u H=%u S=%u\r\n",
        (u32)Xil_In16(BaseAddress + REG_CTRL_ADDR),
        (u32)Xil_In16(BaseAddress + REG_RADIUS_ADDR),
        (u32)Xil_In16(BaseAddress + REG_IMG_W_ADDR),
        (u32)Xil_In16(BaseAddress + REG_IMG_H_ADDR),
        (u32)Xil_In16(BaseAddress + REG_COEFF_SCALE_ADDR));

    return XST_SUCCESS;
}