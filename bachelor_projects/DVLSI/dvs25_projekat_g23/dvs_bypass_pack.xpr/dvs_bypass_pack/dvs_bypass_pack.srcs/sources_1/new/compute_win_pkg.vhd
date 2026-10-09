library IEEE;  
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library work;
use work.RAM_PACKAGE.all;
use work.ARR_TYPES.all; 
use work.MY_STATES_PKG.all;
use work.CONSTANTS.all;

use work.window_pkg.all; 
use work.alu_pkg.all;
use work.alu_window_shift_pkg.all;

package compute_win_pkg is 
    -- citanje rama i shift za 8 ulevo 
    function compute_ram(
        img_r         : integer;
        
        buff_cell     : integer;
        
        reg_rd_data   : std_logic_vector(63 downto 0);
        reg_wr_data   : std_logic_vector(63 downto 0);
        
        pixel         : std_logic_vector(7 downto 0)
    ) return std_logic_vector;
    
    -- racunanje alu 
    function compute_alu(
        img_r         : integer;
        
        buff_cell     : integer;
        
        reg_data      : std_logic_vector(63 downto 0);
        pixel         : std_logic_vector(7 downto 0);
        alu_in        : arr8bit
    ) return arr8bit;
    
    --racunanje prozora 
    procedure compute_win( 
        img_r         : in  integer;
        buff_cell     : in  integer;
        
        -- procitana vrednost, upisana vrednost 
        reg_rd_data   : std_logic_vector(63 downto 0);
        reg_wr_data   : std_logic_vector(63 downto 0);
        
        pixel         : in std_logic_vector( 7 downto 0);
        alu_in        : in arr8bit;
        
        alu_out       : out arr8bit;
        ram_out       : out std_logic_vector(63 downto 0)
    );
end package compute_win_pkg;

package body compute_win_pkg is 
    
    -- ram racunanje funkcija 
    function compute_ram(
        img_r         : integer;
        
        buff_cell     : integer;

        reg_rd_data      : std_logic_vector(63 downto 0);
        reg_wr_data      : std_logic_vector(63 downto 0);
        
        pixel         : std_logic_vector(7 downto 0)
    ) return std_logic_vector is

    variable tmp, wr_reg : std_logic_vector(63 downto 0);
 
    begin
    
        wr_reg := select_wr_source(buff_cell, img_r,  reg_rd_data, reg_wr_data);
               
        case img_r is
    
            when 1 =>
                -- stavimo buff_cell_wr da mi na sledeci takt bude validan signal 
                tmp := shift_insert(wr_reg, pixel, 1, buff_cell);
    
            when 2 =>
                tmp := shift_insert(wr_reg, pixel, 2, buff_cell);
    
            when 0 | 3 | 4 =>
                tmp := reg_rd_data(55 downto 0) & pixel;
    
            when others =>
                tmp := reg_rd_data;
    
        end case;
    
        return tmp;
    
    end function;
    
    
    function compute_alu(
        img_r         : integer;
        buff_cell     : integer;
        
        reg_data   : std_logic_vector(63 downto 0);
        
        pixel      : std_logic_vector(7 downto 0);
        alu_in     : arr8bit
    ) return arr8bit is

    variable tmp : arr8bit(0 to 80) := alu_in;

    begin
    
        -- shift : alu(0)(1) <= alu(0)(0) , alu(0, 2) <= alu(0, 1)... 
        tmp := shift_window_right(tmp);
    
        case img_r is
    
            when 1 =>
                tmp := fill_1st_col(pixel, reg_data, tmp, 1, buff_cell);
    
            when 2 =>
                tmp := fill_1st_col(pixel, reg_data, tmp, 2, buff_cell);
    
            when 0 | 3 | 4 =>
                tmp := fill_1st_col(pixel, reg_data, tmp, img_r, buff_cell);
    
            when others =>
                null;
    
        end case;
    
        return tmp;
    
    end function;
    
    procedure compute_win(
        img_r         : in  integer;
        buff_cell     : in  integer;
             
        reg_rd_data   : std_logic_vector(63 downto 0);
        reg_wr_data   : std_logic_vector(63 downto 0);
        pixel         : in std_logic_vector( 7 downto 0);
 
    
        alu_in        : in arr8bit;
        
        alu_out       : out arr8bit;
        ram_out       : out std_logic_vector(63 downto 0)
    ) is 
    
    begin 

    alu_out := compute_alu(img_r, buff_cell, reg_rd_data, pixel, alu_in);

    ram_out := compute_ram(img_r, buff_cell, reg_rd_data, reg_wr_data, pixel);
    
    end procedure compute_win;

end package body compute_win_pkg;