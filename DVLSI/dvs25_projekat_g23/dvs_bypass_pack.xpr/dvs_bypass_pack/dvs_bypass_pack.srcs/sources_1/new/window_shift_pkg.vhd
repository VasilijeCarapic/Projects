library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

-- funkcija za shift prozora 
package window_pkg is

    function shift_insert(
        data     : std_logic_vector;
        in_pixel : std_logic_vector;
        img_r    : integer;
        buff_cell: integer
    ) return std_logic_vector;
    
    
    function select_wr_source(
        buff_cell: integer;
        img_r    : integer;
        rd_data  : std_logic_vector(63 downto 0);
        wr_data  : std_logic_vector(63 downto 0)
    ) return std_logic_vector;
    
end package window_pkg;

package body window_pkg is
    -- radi shift samo za manji radius: 
    -- klasika kao za radius = 4 samo manji 
    -- new_data <= reg_data(buff_cell * 16 - 1 downto (buff_cel - 1) * 16)
    function shift_insert(
        data     : std_logic_vector;
        in_pixel : std_logic_vector;
        img_r    : integer;
        buff_cell: integer
    ) return std_logic_vector is

        variable result : std_logic_vector(data'range) := data;
    begin

        case img_r is 
            when 1 =>
                if buff_cell  = 0 then 
                    result(15 downto 0)   := result(7 downto 0) & in_pixel;
                
                elsif buff_cell = 1 then 
                    result(31 downto 16)  := result(23 downto 16) & in_pixel;
                
                elsif buff_cell = 2 then 
                    result(47 downto 32)  := result(39 downto 32) & in_pixel;
                
                elsif buff_cell  = 3 then 
                    result(63 downto 48)  := result(55 downto 48) & in_pixel;
                
                end if;
            when 2 => 
                if buff_cell  = 0 then 
                    result(31 downto 0)  := result(23 downto 0) & in_pixel;
                
                elsif buff_cell = 1 then 
                    result(63 downto 32)  := result(55 downto 32) & in_pixel;
                    
                end if;
            when others => 
        end case; 

        return result;

    end function;
    
    function select_wr_source(
        buff_cell: integer;
        img_r    : integer;
        rd_data  : std_logic_vector(63 downto 0);
        wr_data  : std_logic_vector(63 downto 0)
    ) return std_logic_vector is 
        variable return_data : std_logic_vector(63 downto 0);
    begin 
        case img_r is  
            when 1 | 2 => 
                if buff_cell = 0 then 
                    return_data := rd_data;
                else 
                    return_data := wr_data;
                end if;
                  
            when 0 | 3 | 4 => 
                return_data := rd_data;
            when others => 
                return_data := rd_data;
             
        end case;
        
        return return_data;
    
    end function select_wr_source; 
    
end package body window_pkg;

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.ARR_TYPES.ALL;

package alu_window_shift_pkg is

    function shift_window_right(
        alu_in      : arr8bit
    ) return arr8bit;

end package alu_window_shift_pkg;

-- shift kolona koje nisu prve nadesno 
package body alu_window_shift_pkg is

    function shift_window_right(
        alu_in  : arr8bit
    ) return arr8bit is

        variable res : arr8bit(0 to 80) := alu_in;

    begin

        -- za svaki red
        for r in 0 to 8 loop

            -- shift desno (od kraja ka početku)
            for c in 8 downto 1 loop
            
                res(r * 9 + c) := alu_in(r * 9 + c - 1);

            end loop;

        end loop;

        return res;

    end function;

end package body alu_window_shift_pkg;