library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library work;
use work.ARR_TYPES.all; 
use work.CONSTANTS.all; 
use work.MY_STATES_PKG.all;   

entity coordinate_tracker is
Port ( 
    clk              : in std_logic;
    reset            : in std_logic;
    
    state            : in state_t;
    skip_mode        : in std_logic; 
    img_processing   : in std_logic;
    clk_en           : in std_logic;
    reg_valid        : in std_logic; 
    
    imgH             : in std_logic_vector(15 downto 0);
    imgW             : in std_logic_vector(15 downto 0);
    imgR             : in std_logic_vector(2 downto 0);
    
    in_col, in_row   : out std_logic_vector(15 downto 0);
    
    std_ram_ctr      : out std_logic_vector(23 downto 0);
    
    out_en_inter     : in std_logic;
    proc_done_inter  : in std_logic;
    pixel_out_inter  : in std_logic_vector(15 downto 0);
    
    output_enable    : out std_logic;
    processing_done  : out std_logic;
    pixel_output     : out std_logic_vector(15 downto 0);

    start_output     : out std_logic
);  
end coordinate_tracker;

architecture rtl of coordinate_tracker is

constant MAX_CTR_VAL: natural := 1000; -- treba 3, 4 kasnjenje   
-- stavljeno cisto onako 1000 da se osiguramo pre je zavisilo od C_MAX_IMG_W
-- ako je C_MAX_IMG_W = 0  i radius = 0 nece pasti provera u softveru  pa C_MAX_IMG_W = 0 i pade brojac ram_init_ctr 
-- u edge case slucaju 

signal int_row : integer range 0 to C_MAX_IMG_H + 2 * MAX_RADIUS; 
signal int_col : integer range 0 to C_MAX_IMG_W + 2 * MAX_RADIUS; 

signal inW : integer range 0 to C_MAX_IMG_W + 2 * MAX_RADIUS;
signal inH : integer range 0 to C_MAX_IMG_H + 2 * MAX_RADIUS;

signal start_output_reg : std_logic := '0';

signal ram_init_ctr    : natural range 0 to MAX_CTR_VAL - 1; 
signal ram_ctr_std     : std_logic_vector(23 downto 0);

begin
    
    conv_to_integer: process(clk) is 
    begin 
        if(rising_edge(clk)) then 
            if reset = '1' then
                inW <= 0;
                inH <= 0;
            else
                inW  <= to_integer(unsigned(imgW)); 
                inH  <= to_integer(unsigned(imgH));
            end if;
        end if;
    end process conv_to_integer;
 
 
    in_row <= std_logic_vector(to_unsigned(int_row, in_row'length));
    in_col <= std_logic_vector(to_unsigned(int_col, in_col'length));
    
    output_proc : process(clk)
    begin
        if(rising_edge(clk)) then 
            if reset = '1' then
                std_ram_ctr <= (others => '0');
            elsif(clk_en = '1') then 
                std_ram_ctr  <= ram_ctr_std;
            end if;
        end if;
    end process;
    
    curr_cord: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                int_col <= 0;
                int_row <= 0;
            else
                if skip_mode = '1' or img_processing = '1' then 
                    if clk_en = '1' and reg_valid = '1' then  
                        if int_col = inW - 1 then
                            int_col <= 0;
                            int_row <= int_row + 1;
                        else
                            int_col <= int_col + 1;
                        end if;
                    end if;
                else   
                    int_row <= 0; 
                    int_col <= 0; 
                end if;
            end if;
        end if;
    end process;
    
    std_conv: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' then
                ram_ctr_std <= (others => '0');
            elsif img_processing = '1' then 
                if clk_en = '1' then 
                    ram_ctr_std <= std_logic_vector(to_unsigned(ram_init_ctr, ram_ctr_std'length));
                end if;
            end if;     
        end if;
    end process std_conv;

    ram_ctr_proc: process(clk) is 
    begin 
        if rising_edge(clk) then 
            if reset = '1' then
                ram_init_ctr <= 0;
            elsif img_processing = '1' then 
                if clk_en = '1' then 
                    if ram_init_ctr < C_MAX_IMG_W then
                        ram_init_ctr <= ram_init_ctr + 1;
                    end if;
                end if; 
            else 
                ram_init_ctr <= 0;
            end if; 
        end if;
    end process ram_ctr_proc; 

    output_process: process(clk) is 
    begin 
        if(rising_edge(clk)) then 
            if reset = '1' then
                output_enable   <= '0';
                processing_done <= '0';
                pixel_output    <= (others => '0');
            elsif (clk_en = '1') then
                output_enable   <= out_en_inter;
                processing_done <= proc_done_inter;
                pixel_output    <= pixel_out_inter;
            end if;
        end if;
    end process output_process;

    start_latch: process(clk) is
    begin
        if rising_edge(clk) then
            if img_processing = '1' or skip_mode = '1' then 
                 if out_en_inter = '1' then
                    start_output_reg <= '1';
                end if;
            else 
                start_output_reg <= '0';
            end if; 
        end if;
    end process start_latch;

    start_output <= start_output_reg;

end architecture rtl;