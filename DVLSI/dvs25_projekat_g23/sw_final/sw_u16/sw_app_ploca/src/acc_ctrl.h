#ifndef ACC_CTRL_H
#define ACC_CTRL_H

#include "xil_types.h"
#include <xil_io.h>
#include "acc_image_filter.h"

/* =============================================================================
 * ADRESNA MAPA
 *
 *   reg_waddr = aw_addr >> 2 (ADDR_LSB)
 *
 *   Da bi reg_waddr pogodila pravu vrednost: (reg_waddr = awaddr >> 2)
 *   ctrl: reg_waddr = 0x00 => awaddr = 0x00 * 4  = 0x00  (0x00 >> 2 = 0x00) ✓
 *   reg_radius: reg_waddr = 0x02 => awaddr  = 0x02 * 4 = 0x08  (0x08>>2 = 0x02) ✓
 *   reg_img_w : reg_waddr = 0x04 => awaddr  = 0x04 * 4 = 0x10  (0x10>>2 = 0x04) ✓
 *   reg_img_h : reg_waddr = 0x06 => awaddr  = 0x06 * 4 = 0x18  (0x18>>2 = 0x06) ✓
 *   reg_coeff_scale: reg_waddr = 0x08 => awaddr  = 0x08 * 4 = 0x20  (0x20>>2 = 0x08) ✓
 *   reg_coeff: reg_coeff[reg_waddr = 10 * i + 2] 
 *
 * Formula: aw_addr = reg_waddr * 4
 * ============================================================================= */

/* ARM adrese = VHDL indeks * 4 */
#define REG_CTRL_ADDR        0x00
#define REG_RADIUS_ADDR      0x08
#define REG_IMG_W_ADDR       0x10 
#define REG_IMG_H_ADDR       0x18 
#define REG_COEFF_SCALE_ADDR 0x20 
#define REG_COEFF_BASE_ADDR  0x28

/* Koeficijent i: acce;erator ocekuje reg_waddr = 10 + i*2, saljemo aw_addr = base + i * 8; */
#define COEFF_ADDR(i)        (REG_COEFF_BASE_ADDR + i * 8)

#define MAX_FILTER_COEFFS    81u

/* Početni bitovi kontrolnog registra neke celine*/
#define CTRL_MODE_BIT        0u
#define CTRL_BYPASS_BIT      1u
#define CTRL_BORD_SHIFT      2u
#define CTRL_BVAL_SHIFT      4u

int AccConfigure(UINTPTR BaseAddress, ProcessingParams Params);

#endif