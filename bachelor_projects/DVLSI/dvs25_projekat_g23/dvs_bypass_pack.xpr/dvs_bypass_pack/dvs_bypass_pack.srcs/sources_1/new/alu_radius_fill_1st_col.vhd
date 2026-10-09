library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.ARR_TYPES.ALL;


-- vrati vektor prve kolone u zavisnosti od radiusa 
package alu_radius_fill_pkg is 
    function radius1_fill(
        reg_data    : std_logic_vector(63 downto 0);
        buff_cell   : integer;
        alu         : arr8bit
    )return arr8bit;
    
    function radius2_fill(
        reg_data    : std_logic_vector(63 downto 0);
        buff_cell   : integer;
        alu         : arr8bit
    )return arr8bit;
    
    function full_fill(
        reg_data    : std_logic_vector(63 downto 0);
        win_len     : integer;
        alu         : arr8bit
    )return arr8bit;    
end package alu_radius_fill_pkg;

package body  alu_radius_fill_pkg is 
    -- radius = 1 punjenje prve kolone 
    function radius1_fill(
        reg_data  : std_logic_vector(63 downto 0);
        buff_cell   : integer;
        alu         : arr8bit
    ) return arr8bit is 
        variable tmp_res: arr8bit := alu;
    begin 
        if buff_cell = 0 then
            tmp_res(9)  := reg_data(7 downto 0);
            tmp_res(18) := reg_data(15 downto 8);
    
        elsif buff_cell = 1 then
            tmp_res(9)  := reg_data(23 downto 16);
            tmp_res(18) := reg_data(31 downto 24);
    
        elsif buff_cell = 2 then
            tmp_res(9)  := reg_data(39 downto 32);
            tmp_res(18) := reg_data(47 downto 40);
    
        elsif buff_cell = 3 then
            tmp_res(9)  := reg_data(55 downto 48);
            tmp_res(18) := reg_data(63 downto 56);
        end if;
        
        return tmp_res;
    end function radius1_fill;
    
    -- radius = 2 punjenje prve kolone 
    function radius2_fill(
        reg_data  : std_logic_vector(63 downto 0);
        buff_cell   : integer;
        alu         : arr8bit
    ) return arr8bit is 
        variable tmp_res: arr8bit := alu;
    begin 
        if buff_cell = 0 then
            tmp_res(9)  := reg_data(7 downto 0);
            tmp_res(18) := reg_data(15 downto 8);
            tmp_res(27) := reg_data(23 downto 16);
            tmp_res(36) := reg_data(31 downto 24);
    
        elsif buff_cell = 1 then
            tmp_res(9)  := reg_data(39 downto 32);
            tmp_res(18) := reg_data(47 downto 40);
            tmp_res(27) := reg_data(55 downto 48);
            tmp_res(36) := reg_data(63 downto 56);
        end if;
        
        return tmp_res;
    end function radius2_fill;
    
    -- radius = 0, 3, 4 punjenje prve kolone 
    function full_fill(
        reg_data  : std_logic_vector(63 downto 0);
        win_len   : integer;
        alu         : arr8bit
    ) return arr8bit is 
        variable tmp_res: arr8bit(0 to 80) := alu;
    begin 
        for c in 1 to 8 loop
            tmp_res(c * 9) := reg_data(c * 8 - 1 downto (c-1) * 8);
        end loop;
        
        return tmp_res;
    end function full_fill;
end package body  alu_radius_fill_pkg;