library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

package SwapBitlines is 
procedure swap_vars(
    variable var1 : inout std_logic_vector(7 downto 0);  
    variable var2 : inout std_logic_vector(7 downto 0);  
    variable min : inout std_logic_vector(7 downto 0);     
    variable max : inout std_logic_vector(7 downto 0)
);
 end package;

package body SwapBitlines is
    procedure swap_vars(
    variable var1 : inout std_logic_vector(7 downto 0);  
    variable var2 : inout std_logic_vector(7 downto 0);  
    variable min : inout std_logic_vector(7 downto 0);     
    variable max : inout std_logic_vector(7 downto 0)      
) is
begin
    if to_integer(unsigned(var1)) > to_integer(unsigned(var2)) then
        min := var2;
        max := var1;
    else
        min := var1;
        max := var2;
    end if;
end procedure;
end package body SwapBitlines;   
