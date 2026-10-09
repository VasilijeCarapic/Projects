library IEEE; 
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library work;
use work.RAM_PACKAGE.all;
use work.ARR_TYPES.all;   
use work.MY_STATES_PKG.ALL;
use work.CONSTANTS.ALL;


entity params_loader is
Generic(
       C_MAX_IMG_W: integer := 1024;
       C_MAX_IMG_H: integer := 1024    
);
Port ( 
      clk            : in std_logic;
      reset          : in std_logic;
      
      -- registri za citanje, upis 
      reg_ctrl       : in std_logic_vector(15 downto 0);
      reg_radius     : in std_logic_vector(15 downto 0);
      reg_img_w      : in std_logic_vector(15 downto 0);
      reg_img_h      : in std_logic_vector(15 downto 0);
      state          : in state_t;
     
      -- koeficijenti , shiftovani koeficijenti, regs_written upisane su pocetne vrednosti u koeficijente 
      filter_coeffs  : in  arr16bit(0 to 80);

      -- parametri sistema izlazni 
      imgW           : out std_logic_vector(15 downto 0);
      imgH           : out std_logic_vector(15 downto 0);

      img_r          : out std_logic_vector(2 downto 0);
      div_shift      : out std_logic_vector(2 downto 0);
      
      nearest_en     : out std_logic;
      border_en      : out std_logic;
      
      border         : out std_logic_vector(1 downto 0):= "00"; -- 0x10 mirror pad, 0x11 border, 0x00 nista 
      border_value   : out std_logic_vector(7 downto 0) := (others => '0');
      bypass         : out std_logic := '1'; -- 0 obradjuj , 1 ne obradjuj
      mode           : out std_logic := '0' -- 0 -> neoznaceni 8 bitni podatak, 1 -> 9 bita ceo deo 7 bita razlomljeni deo
);
end params_loader;

architecture Behavioral of params_loader is

--radius 
signal radius: integer:= 1;

-- pomereni koeficijenti  
-- alu prima u levom uglu , koeficijenti su u srednini mora da se prebace u levi ugao da bi se pogodio rezultat

begin

-- racunanje parametara 
param_basic_proc: process(clk)
begin
    if rising_edge(clk) then
        if reset = '1' then
            img_r  <= (others => '0');
            border <= "00";
            bypass <= '0';
            mode   <= '0';
        elsif state = Config then
            img_r  <= reg_radius(2 downto 0);
            border <= reg_ctrl(3 downto 2);
            bypass <= reg_ctrl(1);
            mode   <= reg_ctrl(0);
        end if;
    end if;
end process;

img_dim_proc: process(clk)
    variable r_int, w_int, h_int, outputSize: integer;
begin
    if rising_edge(clk) then
        if reset = '1' then
            imgW <= (others => '0');
            imgH <= (others => '0');
        elsif state = Config then
            r_int := to_integer(unsigned(reg_radius(2 downto 0)));
            w_int := to_integer(unsigned(reg_img_w));
            h_int := to_integer(unsigned(reg_img_h));

            if reg_ctrl(3 downto 2) = "00" or reg_ctrl(3 downto 2) = "01" then
                imgW <= reg_img_w;
                imgH <= reg_img_h;
            else
                imgW <= std_logic_vector(to_unsigned(w_int + 2*r_int, imgW'length));
                imgH <= std_logic_vector(to_unsigned(h_int + 2*r_int, imgH'length));
            end if;
            
        end if;
    end if;
end process;

mode_proc: process(clk) 
begin 
    if(rising_edge(clk)) then
        if reset = '1' then  
            nearest_en <= '0';
            border_en  <= '0';
        elsif state = Config then 
            case reg_ctrl(3 downto 2) is
                when "00" | "01"  => 
                    nearest_en <= '0';
                    border_en  <= '0';
                when "11"         => 
                    nearest_en <= '0';
                    border_en  <= '1';
                when "10"         => 
                    nearest_en <= '1';
                    border_en  <= '0';
                when others       =>
            end case;
        end if;    
    end if;
end process mode_proc;

shift_val_proc: process(clk)
begin
    if rising_edge(clk) then
        if reset = '1' then
            div_shift <= "000";
        else
            case to_integer(unsigned(reg_radius)) is
                when 1 => div_shift <= "010"; -- deli se sa 4 
                when 2 => div_shift <= "001"; -- deli se sa 2 
                when others => div_shift <= "000";
            end case;
        end if;
    end if;
end process;

    
end Behavioral;
