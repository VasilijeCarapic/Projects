#include "helper_functions.h"
#include "hw_image_filter.h"
#include <xstatus.h>
#include <xil_printf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  

int calcOutputDims(u16* outW, u16* outH, ProcessingParams* params)
{
    /* racuna izlazne dimenzije outW i outH slike*/
    return calcOutWH(outW, outH, params);
}

/* racuna pixelSize u zavisnosti od kontrolnih signala*/
int calculatePxSize(ProcessingParams* params){
    int mode   = params -> Mode;
    int bypass = params -> Bypass; 

    if(mode == 0 || bypass == 1){
        return 1; 
    }
    else if(mode == 1){
        return 2; 
    }
    return 1;
}

/* pakuje oba pixela u jednu rec za slucaj bypassa */
void packPixelsBypass(u16 imgW, u16 imgH, u8* DataIn, u16* DataOut) {
    u16 r, c;
    u8 px1, px2;
    u8 *rowIn;
    u32 outIdx = 0;

    for (r = 0; r < imgH; r++) {
        /* trenutni red */
        rowIn = DataIn + (size_t)r * imgW; 
        c = 0;
        /* idi do pretposlednjeg pixela 2 po 2 i pakuj u letu*/
        for (; c < imgW - 1; c += 2) {
            px1 = rowIn[c];
            px2 = rowIn[c + 1];
            DataOut[outIdx++] = (u16)px1 | ((u16)px2 << 8);
        }
        /* Ako je sirina slike neparna onda se radi kupljenje zadnjeg pixela */
        if (c < imgW) {
            px1 = rowIn[c];
            DataOut[outIdx++] = (u16)px1;
        }
    }
}

void flush_stdin_line(void)
{
    int c;
    
    /* citaj karaktere sa kraja da mi ne prave problem za sledecu iteraciju */
    while ((c = getchar()) != '\n' && c != '\r' && c != EOF) { }
}

/* ove 3 funkcije uzmu vrednost i 
   teraju korisnika da unese novu vrednost sve dok vrednost 
   nije u zadatom opsegu */
int validateInt(int minVal, int maxVal){
    int readVal, ok;

    for (;;) {
        ok = (scanf("%d", &readVal) == 1);
        flush_stdin_line();
        if (ok && readVal >= minVal && readVal <= maxVal) {
            return readVal;
        }
        xil_printf("\r\nParametar mora biti u opsegu (%d, %d):", minVal, maxVal);
    }
}

float validateFloat(float minVal, float maxVal){
    float readVal;
    int ok;

    for (;;) {
        ok = (scanf("%f", &readVal) == 1);
        flush_stdin_line();
        if (ok && readVal >= minVal && readVal <= maxVal) {
            return readVal;
        }
        xil_printf("\r\nParametar mora biti u opsegu (%f, %f):", minVal, maxVal);
    }
}

char validateChar(){
    char c; 
    int ok;

    for(;;){
        ok = (scanf("%c", &c) == 1);
        flush_stdin_line();
        if(ok && (c == 'y' || c == 'n' || c == 'Y' || c == 'N')){ return c; } 
        xil_printf("\r\nValidne opcije: [y/n/Y/N]");
    }
}