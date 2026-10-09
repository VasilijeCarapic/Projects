library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library work;
use work.filter_constants_pkg.all;

package filter_config is 

-- u filter_constants_pkg, posle svih s16_array_81 konstanti:

type filter_sel_t is (SEL_BOX_S1, SEL_BOX_S3, SEL_LOG, SEL_GAUSS_S1, SEL_GAUSS_Q4, SEL_GAUSS_R0,
                      SEL_SHARP_BOX_R4, SEL_SHARP_BOX_R1, SEL_SHARP_GAUSS_R4, SEL_SHARP_GAUSS_R2);

type radius_lut_t is array(filter_sel_t) of integer;
constant RADIUS_LUT : radius_lut_t := (
    SEL_BOX_S1 => 4, SEL_BOX_S3 => 4, SEL_LOG => 4, SEL_GAUSS_S1 => 4, SEL_GAUSS_Q4 => 4, SEL_GAUSS_R0 => 0, 
    SEL_SHARP_BOX_R4 => 4, SEL_SHARP_BOX_R1 => 1, SEL_SHARP_GAUSS_R4 => 4, SEL_SHARP_GAUSS_R2 => 2
);

type scale_lut_t is array(filter_sel_t) of integer;
constant SCALE_LUT : scale_lut_t := (
    SEL_BOX_S1 => 4096, SEL_BOX_S3 => 1365, SEL_LOG => 4096, SEL_GAUSS_S1 => 4096, SEL_GAUSS_Q4 => 1024, 
    SEL_GAUSS_R0 => 409600, SEL_SHARP_BOX_R4 => 8192, SEL_SHARP_BOX_R1 => 8192, SEL_SHARP_GAUSS_R4 => 8192, 
    SEL_SHARP_GAUSS_R2 => 8192
);

function get_coeffs(sel : in filter_sel_t) return s16_array_81;

end package filter_config;


package body  filter_config is 

function get_coeffs(sel : in filter_sel_t) return s16_array_81 is
begin
    case sel is
        when SEL_BOX_S1         => return FILT_BOX_S1;
        when SEL_BOX_S3         => return FILT_BOX_S3;
        when SEL_LOG            => return LOG_NO_SCALE;
        when SEL_GAUSS_S1       => return GAUSS_NO_SCALE;
        when SEL_GAUSS_Q4       => return GAUSS_SCALE_1BY4;
        when SEL_GAUSS_R0       => return GAUSS_RAD_0;
        when SEL_SHARP_BOX_R4   => return SHARP_BOX_R4;
        when SEL_SHARP_BOX_R1   => return SHARP_BOX_R1;
        when SEL_SHARP_GAUSS_R4 => return GAUSS_SHARPEN_R4;
        when SEL_SHARP_GAUSS_R2 => return GAUSS_SHARPEN_R2;
        
    end case;
end function;

end package body filter_config;
