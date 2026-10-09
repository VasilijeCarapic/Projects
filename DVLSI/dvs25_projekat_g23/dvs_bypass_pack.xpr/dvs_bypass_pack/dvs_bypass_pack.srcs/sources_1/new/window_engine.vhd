library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library work;
use work.ARR_TYPES.all;
use work.CONSTANTS.all;
use work.compute_win_pkg.all;
use work.MY_STATES_PKG.all;

entity window_engine is
    port(
        clk            : in std_logic;
        reset          : in std_logic;

        -- kontrola
        state          : in state_t;
        clk_en         : in std_logic;

        -- ulazi
        img_r          : in std_logic_vector(2 downto 0);
        
        ram_ctr_std    : in std_logic_vector(23 downto 0);
        
        rd_modulo      : in std_logic_vector(1 downto 0);
        wr_modulo      : in std_logic_vector(1 downto 0);
        
        data_modulo    : in std_logic_vector(1 downto 0);
        
        pipe           : in arr8bit(0 to PIPE_DEPTH - 1);
        ram_reg_rd     : in std_logic_vector(63 downto 0);

        -- izlazi
        alu_out        : out arr8bit(0 to 80);
        ram_reg_wr     : out std_logic_vector(63 downto 0)
        
    );
end entity window_engine;


architecture rtl of window_engine is

    signal alu_reg : arr8bit(0 to 80) := (others => (others => '0'));
    
    -- ovako 
    signal ram_reg_wr_i : std_logic_vector(63 downto 0) := (others => '0');
    
    signal alu_active           : std_logic := '0';
    signal start_win_processing : std_logic := '0';
    
begin
    
    latch_proc: process(clk) is 
    begin 
        if rising_edge(clk) then 
            if reset = '1' then
                start_win_processing <= '0';
            else
                case state is 
                    when Processing => 
                        if clk_en = '1' and unsigned(ram_ctr_std) >= 3 then
                            start_win_processing <= '1';
                        end if;      
                        
                    when others => 
                        start_win_processing <= '0';     
                end case; 
            end if;
        end if; 
    end process latch_proc;
    
    process(clk)
        variable alu_var      : arr8bit(0 to 80);
        variable rd_data, wr_data, wr_data_in : std_logic_vector(63 downto 0);
        variable buff_cell    : integer;
        variable radius       : integer;
    begin
        if rising_edge(clk) then
            if reset = '1' then
                alu_reg      <= (others => (others => '0'));
                ram_reg_wr_i <= (others => '0');
            else
                -- reset ALU u config stanju
                case state is 
                    when Config => 
                        alu_reg <= (others => (others => '0'));
                    when Processing => 
                        if (start_win_processing = '1' or unsigned(ram_ctr_std) >= 3) and clk_en = '1' then
                            
                            -- ucitaj alu, podatka za citanje 
                            alu_var := alu_reg;
                            rd_data := ram_reg_rd;
                            wr_data := ram_reg_wr_i;
                            wr_data_in := ram_reg_wr_i;  -- sačuvaj pre poziva
            
                            -- ovo je za bonus 2 u koju celiju pisemo 
                            -- na primer za radius = 1 da li pisemo u celiju 1,2,3, ili 4 
                            buff_cell    := to_integer(unsigned(data_modulo));
                            radius       := to_integer(unsigned(img_r));
            
                            -- glavna logika 
                            compute_win(radius, buff_cell, rd_data, wr_data_in, pipe(4), alu_var, alu_var, wr_data); 
            
                            -- upisi nazad
                            alu_reg      <= alu_var;
                            ram_reg_wr_i <= wr_data;
            
                        end if;
                    when others => 
                        null;
                end case;
            end if;
        end if;
    end process;

    -- izlaz
    alu_out <= alu_reg;
    ram_reg_wr <= ram_reg_wr_i;

end architecture rtl;