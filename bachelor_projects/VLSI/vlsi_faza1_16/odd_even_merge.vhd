library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library work;
use work.SwapBitlines.all;  
entity odd_even_merge is
--maximum frequency around 284.5Mhz
    Generic(
        number_of_elements: natural := 8;
        number_of_bits_per_element: natural := 7;
        vector_length: natural := 71
    );
    Port(
        clk: in std_logic;
        reset: in std_logic;
        all_bits_elements: in std_logic_vector(vector_length downto 0);
        s4: out std_logic_vector(number_of_bits_per_element downto 0)
    );  
end odd_even_merge;

architecture Behavioral of odd_even_merge is

    signal r_all_bits_elements: std_logic_vector(vector_length downto 0);
    signal r_s4: std_logic_vector(number_of_bits_per_element downto 0);

    type bit_signals_array is array(0 to number_of_elements) of std_logic_vector(number_of_bits_per_element downto 0);

    signal stage0_array: bit_signals_array;
    signal stage1_array: bit_signals_array;
    signal stage2_array: bit_signals_array;
    signal stage3_array: bit_signals_array;
    signal stage4_array: bit_signals_array;
    signal stage5_array: bit_signals_array;
    signal stage6_array: bit_signals_array;
    signal stage7_array: bit_signals_array;
    signal stage8_array: bit_signals_array;
    signal stage9_array: bit_signals_array;
    signal stage10_array: bit_signals_array;
    
begin

    REG_PROCESS: process(clk) is 
    begin 
        if rising_edge(clk) then 
            if(reset = '1') then
                r_all_bits_elements <= (others => '0'); 
                s4 <= (others => '0');     
            else
                r_all_bits_elements <= all_bits_elements;
                s4 <= r_s4;
            end if;
        end if;    
    end process REG_PROCESS;
    
    
    STAGE0_PROCESS: process(clk)  is 
    begin
    if(rising_edge(clk)) then
        for i in 0 to number_of_elements loop
            stage0_array(i) <= r_all_bits_elements(vector_length- i*8 downto vector_length -i*8 - number_of_bits_per_element);
        end loop;
    end if;        
    end process STAGE0_PROCESS;
    
    
STAGE1_PROCESS: process(clk) is     
    variable min_bitline : std_logic_vector(7 downto 0);
    variable max_bitline : std_logic_vector(7 downto 0);
    variable array1: bit_signals_array;    
begin 
if(rising_edge(clk)) then  
        for i in 0 to number_of_elements  loop
            array1(i) := stage0_array(i);  
        end loop;

        for j in 0 to 3 loop
            swap_vars(array1(2*j + 1), array1(2*j), min_bitline, max_bitline);
            array1(2*j) := min_bitline;
            array1(2*j + 1) := max_bitline;
        end loop;
        
        for k in 0 to number_of_elements  loop
            if (reset = '1') then
                stage1_array(k) <= (others => '0');  -- Reset condition
            else
                stage1_array(k) <= array1(k);  -- Normal assignment
            end if;
        end loop;
end if;        
end process STAGE1_PROCESS;

STAGE2_PROCESS: process(clk) is
    variable array2: bit_signals_array;
    variable min_bitline : std_logic_vector(7 downto 0);
    variable max_bitline : std_logic_vector(7 downto 0);
begin
if rising_edge(clk) then 
            for i in 0 to number_of_elements  loop
                array2(i) := stage1_array(i);   -- Normal assignment
            end loop;
    
            swap_vars(array2(0), array2(2), min_bitline, max_bitline);
            array2(0) := min_bitline;
            array2(2) := max_bitline;
    
            swap_vars(array2(4), array2(6), min_bitline, max_bitline);
            array2(4) := min_bitline;
            array2(6) := max_bitline;
    
            swap_vars(array2(1), array2(3), min_bitline, max_bitline);
            array2(1) := min_bitline;
            array2(3) := max_bitline;
    
            swap_vars(array2(5), array2(7), min_bitline, max_bitline);
            array2(5) := min_bitline;
            array2(7) := max_bitline;
    
            for i in 0 to number_of_elements  loop
                if (reset = '1') then
                    stage2_array(i) <= (others => '0');  -- Reset condition
                else
                    stage2_array(i) <= array2(i);  -- Normal assignment
                end if;
            end loop;
end if;            
end process STAGE2_PROCESS;

STAGE3_PROCESS: process(clk) is
    variable array3: bit_signals_array;
    variable min_bitline : std_logic_vector(7 downto 0);
    variable max_bitline : std_logic_vector(7 downto 0);
begin
if(rising_edge(clk)) then
        for i in 0 to number_of_elements  loop
            array3(i) := stage2_array(i); 
        end loop;

        swap_vars(array3(1), array3(2), min_bitline, max_bitline);
        array3(1) := min_bitline;
        array3(2) := max_bitline;

        swap_vars(array3(5), array3(6), min_bitline, max_bitline);
        array3(5) := min_bitline;
        array3(6) := max_bitline;

        for i in 0 to number_of_elements  loop
            if (reset = '1') then
                stage3_array(i) <= (others => '0'); 
            else
                stage3_array(i) <= array3(i); 
            end if;
        end loop;
end if;        
end process STAGE3_PROCESS;

STAGE4_PROCESS: process(clk) is
    variable array4: bit_signals_array;
    variable min_bitline : std_logic_vector(7 downto 0);
    variable max_bitline : std_logic_vector(7 downto 0);
begin
if rising_edge(clk) then 
        for i in 0 to number_of_elements  loop
            array4(i) := stage3_array(i);   
        end loop;

        swap_vars(array4(0), array4(4), min_bitline, max_bitline);
        array4(0) := min_bitline;
        array4(4) := max_bitline;

        swap_vars(array4(1), array4(5), min_bitline, max_bitline);
        array4(1) := min_bitline;
        array4(5) := max_bitline;

        swap_vars(array4(2), array4(6), min_bitline, max_bitline);
        array4(2) := min_bitline;
        array4(6) := max_bitline;

        swap_vars(array4(3), array4(7), min_bitline, max_bitline);
        array4(3) := min_bitline;
        array4(7) := max_bitline;

        for i in 0 to number_of_elements  loop
            if (reset = '1') then
                stage4_array(i) <= (others => '0');  
            else
                stage4_array(i) <= array4(i);  
            end if;
        end loop;
end if;        
end process STAGE4_PROCESS;

STAGE5_PROCESS: process(clk) is
    variable array5: bit_signals_array;
    variable min_bitline : std_logic_vector(7 downto 0);
    variable max_bitline : std_logic_vector(7 downto 0);
begin
if(rising_edge(clk)) then 
        for i in 0 to number_of_elements  loop
            array5(i) := stage4_array(i);
        end loop;

        swap_vars(array5(2), array5(4), min_bitline, max_bitline);
        array5(2) := min_bitline;
        array5(4) := max_bitline;

        swap_vars(array5(3), array5(5), min_bitline, max_bitline);
        array5(3) := min_bitline;
        array5(5) := max_bitline;

        for i in 0 to number_of_elements loop
            if (reset = '1') then
                stage5_array(i) <= (others => '0');
            else
                stage5_array(i) <= array5(i); 
            end if;
        end loop;
end if;        
end process STAGE5_PROCESS;

STAGE6_PROCESS: process(clk) is
    variable array6: bit_signals_array;
    variable min_bitline : std_logic_vector(7 downto 0);
    variable max_bitline : std_logic_vector(7 downto 0);
begin
if(rising_edge(clk)) then        
        for i in 0 to number_of_elements  loop
            array6(i) := stage5_array(i);   
        end loop;

        swap_vars(array6(0), array6(8), min_bitline, max_bitline);
        array6(0) := min_bitline;
        array6(8) := max_bitline;

        swap_vars(array6(1), array6(2), min_bitline, max_bitline);
        array6(1) := min_bitline;
        array6(2) := max_bitline;

        swap_vars(array6(3), array6(4), min_bitline, max_bitline);
        array6(3) := min_bitline;
        array6(4) := max_bitline;

        swap_vars(array6(5), array6(6), min_bitline, max_bitline);
        array6(5) := min_bitline;
        array6(6) := max_bitline;

        for i in 0 to number_of_elements  loop
            if (reset = '1') then
                stage6_array(i) <= (others => '0'); 
            else
                stage6_array(i) <= array6(i); 
            end if;
        end loop;
end if;        
end process STAGE6_PROCESS;

STAGE7_PROCESS: process(clk) is
    variable min_bitline : std_logic_vector(7 downto 0);
    variable max_bitline : std_logic_vector(7 downto 0);
    variable array7: bit_signals_array;
begin
if(rising_edge(clk)) then 
        for i in 0 to number_of_elements  loop
                array7(i) := stage6_array(i);   
        end loop;
        
        swap_vars(array7(4), array7(8), min_bitline, max_bitline);
        array7(4) := min_bitline;
        array7(8) := max_bitline;
        
        
        for i in 0 to number_of_elements  loop
            if (reset = '1') then
                stage7_array(i) <= (others => '0');  -- Reset condition
            else
                stage7_array(i) <= array7(i);  -- Normal assignment
            end if;
        end loop;
end if;        
end process STAGE7_PROCESS;

STAGE8_PROCESS: process(clk) is 
    variable min_bitline : std_logic_vector(7 downto 0);
    variable max_bitline : std_logic_vector(7 downto 0);
    variable array8: bit_signals_array;
begin
if(rising_edge(clk)) then 
        for i in 0 to number_of_elements  loop 
           array8(i) := stage7_array(i);
        end loop;

        -- Perform swaps for 2-4 and 3-5
        swap_vars(array8(2), array8(4), min_bitline, max_bitline);
        array8(2) := min_bitline;
        array8(4) := max_bitline;

        swap_vars(array8(3), array8(5), min_bitline, max_bitline);
        array8(3) := min_bitline;
        array8(5) := max_bitline;

        for i in 0 to number_of_elements  loop
            if (reset = '1') then
                stage8_array(i) <= (others => '0'); 
            else
                stage8_array(i) <= array8(i);  
            end if;
        end loop;
end if;        
end process STAGE8_PROCESS;

STAGE9_PROCESS: process(clk) is 
    variable min_bitline : std_logic_vector(7 downto 0);
    variable max_bitline : std_logic_vector(7 downto 0);
    variable array9: bit_signals_array;
begin
if(rising_edge(clk)) then
        for i in 0 to number_of_elements loop 
           array9(i) := stage8_array(i); 
        end loop;

        swap_vars(array9(3), array9(4), min_bitline, max_bitline);
        array9(3) := min_bitline;
        array9(4) := max_bitline;

        for i in 0 to number_of_elements loop
            if (reset = '1') then
                stage9_array(i) <= (others => '0');  
            else
                stage9_array(i) <= array9(i);  
            end if;
        end loop;
end if;         
end process STAGE9_PROCESS;
    
STAGE10_PROCESS:process(stage9_array) is 
    variable min_bitline : std_logic_vector(7 downto 0);
    variable max_bitline : std_logic_vector(7 downto 0); 
    begin                
         r_s4 <= stage9_array(4);
    end process STAGE10_PROCESS;
end Behavioral;
