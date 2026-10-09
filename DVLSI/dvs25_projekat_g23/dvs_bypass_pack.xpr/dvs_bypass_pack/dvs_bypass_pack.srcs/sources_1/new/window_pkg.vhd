-- bonus 2 koji puni prvu kolonu zavisno od radiusa 
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.ARR_TYPES.ALL;
use work.alu_radius_fill_pkg.ALL;

package alu_pkg is  
   
    function fill_1st_col(
        pixel     : std_logic_vector(7 downto 0);
        reg_data  : std_logic_vector(63 downto 0);
        alu       : arr8bit;
        radius    : integer; 
        buff_cell : integer
            
    ) return arr8bit; 
    
end package alu_pkg;

package body alu_pkg is 
    
    -- punjenje prve kolone alu 
    function fill_1st_col(
        pixel     : std_logic_vector(7 downto 0);
        reg_data  : std_logic_vector(63 downto 0);
        alu       : arr8bit;
        radius    : integer;
        buff_cell : integer
    ) return arr8bit is 

    variable res : arr8bit := alu;
    constant WINLEN : natural := 9;
    
    begin
        -- inicijalizacija prve kolone na nula 
        for i in 0 to 8 loop
            res(i*9) := (others => '0');
        end loop;
        
        res(0) := pixel;
        
        case radius is
    
            when 1 => 
                res := radius1_fill(reg_data, buff_cell, res);
    
            when 2 => 
                res := radius2_fill(reg_data, buff_cell, res);
               
            when 0 | 3 | 4 => 
                res := full_fill(reg_data, WINLEN, res);
            
            when others =>
                null;
    
        end case;
    
        return res;
    
    end function;

end package body alu_pkg;


