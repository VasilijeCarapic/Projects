library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library work;
use work.MY_STATES_PKG.all;

entity axi_control_gen is
    Port (
        clk                 : in std_logic;
        reset               : in std_logic;
        pipe_en             : in std_logic;
        config_mode         : in std_logic;
        img_processing      : in std_logic;
        skip_mode           : in std_logic;
        state               : in state_t;
        
        imgW                : in std_logic_vector(15 downto 0);
        imgH                : in std_logic_vector(15 downto 0);
        img_r               : in std_logic_vector(2 downto 0);
        
        in_row              : in std_logic_vector(15 downto 0);
        in_col              : in std_logic_vector(15 downto 0);
        reg_tvalid          : in std_logic;
        reg_tlast           : in std_logic;
        
        tag_valid           : out std_logic;
        tag_last            : out std_logic
    );
end entity axi_control_gen;

architecture rtl of axi_control_gen is
    signal W_int, H_int, radius : natural := 0;
begin
    -- Registracija konfiguracionih parametara slike
    convert_proc: process(clk)
    begin
        if rising_edge(clk) then
            if (reset = '1') then 
                W_int  <= 0; 
                H_int  <= 0; 
                radius <= 0; 
            else 
                case state is
                    when Config =>
                        W_int  <= to_integer(unsigned(imgW));
                        H_int  <= to_integer(unsigned(imgH));
                        radius <= to_integer(unsigned(img_r));
                    when others =>
                        null;
                end case;
            end if;
        end if;
    end process convert_proc;

    -- Generisanje unutrašnjih AXI kontrolnih tagova (valid i last) 
    axi_ctrl_signals_proc: process(clk) is 
        variable r_val, c_val  : integer;
        variable is_in_bounds  : boolean;
        variable is_last_pixel : boolean;
    begin 
        if rising_edge(clk) then
            if reset = '1' then 
                tag_valid <= '0';
                tag_last  <= '0';
            elsif pipe_en = '1' or config_mode = '1' then 
                
                tag_valid <= '0';
                tag_last  <= '0';
                
                if config_mode = '0' and reg_tvalid = '1' then
                    r_val := to_integer(unsigned(in_row));
                    c_val := to_integer(unsigned(in_col));
                    
                    is_in_bounds  := (r_val >= 2 * radius) and (r_val < H_int) and 
                                     (c_val >= 2 * radius) and (c_val < W_int);
                                     
                    is_last_pixel := (r_val = H_int - 1 and c_val = W_int - 1) or (reg_tlast = '1');
                    
                    if img_processing = '1' then
                        if is_in_bounds then
                            tag_valid <= '1';
                        end if;
                        
                        if is_last_pixel then
                            tag_last <= '1';
                        end if;
                        
                    elsif skip_mode = '1' then
                        tag_valid <= '1';
                        
                        if is_last_pixel then
                            tag_last <= '1';
                        end if;
                    end if;
                end if;
                            
            end if;
        end if; 
    end process axi_ctrl_signals_proc;
end architecture rtl;