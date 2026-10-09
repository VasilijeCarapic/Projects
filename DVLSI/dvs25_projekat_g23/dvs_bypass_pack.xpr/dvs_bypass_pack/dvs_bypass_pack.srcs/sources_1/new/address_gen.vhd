library IEEE; 
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;     
use work.RAM_PACKAGE.all;
use work.CONSTANTS.all;

entity address_gen is
Port ( 
    clk   : in std_logic;
    reset : in std_logic;
    
    img_processing : in std_logic;
    skip_mode      : in std_logic;
    ram_en         : in std_logic;  -- = img_processing & ~skip_mode & stream_ready
    img_r          : in std_logic_vector(2 downto 0);
    row, col       : in std_logic_vector(15 downto 0); 
    imgW           : in std_logic_vector(15 downto 0);
    shift          : in std_logic_vector(2 downto 0);
    
    addra          : out std_logic_vector((clogb2(C_MAX_IMG_W)-1) downto 0);     
    addrb          : out std_logic_vector((clogb2(C_MAX_IMG_W)-1) downto 0);
    enb            : out std_logic;
    wea            : out std_logic;
    
    rd_modulo, wr_modulo, data_modulo : out std_logic_vector(1 downto 0)
);
end address_gen;

architecture rtl of address_gen is

-- buffer_ctr -> addrb, d_buffer -> addra
-- rd_ctr, wr_ctr adresa koja je za bonus 2 gde se cita i pise 
signal addrb_ctr, addra_ctr, rd_ctr, wr_ctr : integer range 0 to C_MAX_IMG_W;

signal dd_buffer, d_buffer_ctr   : integer range 0 to C_MAX_IMG_W;
signal ddd_buffer, dd_buffer_ctr : integer range 0 to C_MAX_IMG_W;
signal d_addra, d_addrb : std_logic_vector((clogb2(C_MAX_IMG_W)-1) downto 0); 

signal data_rd_modulo, d_rd_modulo : std_logic_vector(1 downto 0);

begin

    -- BRAM enable/write gejtovani sa ram_en:
    -- kad stream_ready = '0', ram_en = '0'
    enb <= ram_en;
    wea <= ram_en;
    
    addrb_ctr_proc: process(clk)
    begin 
        if rising_edge(clk) then 
            if reset = '1' or img_processing = '0' then
                addrb_ctr <= 0;
            elsif ram_en = '1' then
                if addrb_ctr < to_integer(unsigned(imgW)) - 1 then 
                    addrb_ctr <= addrb_ctr + 1;   
                else     
                    addrb_ctr <= 0;
                end if;
            end if;
            -- ram_en='0' AND img_processing='1': stoji (backpressure)
        end if;
    end process addrb_ctr_proc;
    
    addra_ctr_proc: process(clk) is 
    begin 
        if rising_edge(clk) then 
            if reset = '1' or img_processing = '0' then
                addra_ctr <= 0;
            elsif ram_en = '1' then
                if addrb_ctr >= 2 then 
                    addra_ctr <= addrb_ctr - 2;
                else 
                    addra_ctr <= addrb_ctr - 2 + to_integer(unsigned(imgW));
                end if;
            end if;
            -- ram_en='0' AND img_processing='1': stoji (backpressure)
        end if;
    end process addra_ctr_proc;
    
    set_addr: process(clk) 
    begin 
        if rising_edge(clk) then 
            if reset = '1' or img_processing = '0' then
                d_addra <= (others => '0');
                d_addrb <= (others => '0');
            elsif ram_en = '1' then
                if img_r = "001" or img_r = "010" then 
                    d_addra <= std_logic_vector(to_unsigned(wr_ctr, d_addra'length));
                    d_addrb <= std_logic_vector(to_unsigned(rd_ctr, d_addrb'length));
                elsif img_r = "000" or img_r = "011" or img_r = "100" then 
                    d_addra <= std_logic_vector(to_unsigned(dd_buffer,    d_addra'length));
                    d_addrb <= std_logic_vector(to_unsigned(d_buffer_ctr, d_addrb'length));
                end if;
            end if;
        end if;
    end process set_addr;
    
    -- calc_addra: registruje d_addra na izlaz, gejtovano sa ram_en
    wr_addr: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' or img_processing = '0' then
                addra <= (others => '0');
            elsif ram_en = '1' then
                addra <= d_addra;
            end if;
        end if;
    end process wr_addr;

    -- rd_addr: registruje d_addrb na izlaz, gejtovano sa ram_en
    rd_addr: process(clk)
    begin
        if rising_edge(clk) then
            if reset = '1' or img_processing = '0' then
                addrb <= (others => '0');
            elsif ram_en = '1' then
                addrb <= d_addrb;
            end if;
        end if;
    end process rd_addr;
    
    -- addr_counters: bonus2 shift logika, stoji tokom backpressure
    addr_counters: process(clk) 
    variable div_shift: integer;
    begin 
        if rising_edge(clk) then 
            if reset = '1' or img_processing = '0' then
                rd_ctr <= 0;
                wr_ctr <= 0;
            elsif ram_en = '1' then 
                if img_r = "001" or img_r = "010" then 
                    div_shift := to_integer(unsigned(shift));
                    rd_ctr <= to_integer(shift_right(to_unsigned(addrb_ctr, 32), div_shift));
                    wr_ctr <= to_integer(shift_right(to_unsigned(addra_ctr, 32), div_shift));
                end if;
            end if;
        end if;
    end process addr_counters;


    delay_proc: process(clk) 
    begin 
        if rising_edge(clk) then 
            if reset = '1' or img_processing = '0' then
                dd_buffer     <= 0;
                ddd_buffer    <= 0;
                d_buffer_ctr  <= 0;
                dd_buffer_ctr <= 0;
            elsif ram_en = '1' then
                dd_buffer     <= addra_ctr;
                ddd_buffer    <= dd_buffer;
                d_buffer_ctr  <= addrb_ctr;
                dd_buffer_ctr <= d_buffer_ctr;
            end if;
        end if;
    end process delay_proc;

    -- modulo process: gejtovano sa ram_en
    modulo_proc: process(clk)
    variable rd_tmp : std_logic_vector(15 downto 0);
    variable wr_tmp : std_logic_vector(15 downto 0);
    begin
        if rising_edge(clk) then
            if reset = '1' or img_processing = '0' then
                rd_modulo      <= (others => '0');
                wr_modulo      <= (others => '0');
                data_rd_modulo <= (others => '0');
            elsif ram_en = '1' then
                rd_modulo <= (others => '0');
                wr_modulo <= (others => '0');
                rd_tmp := std_logic_vector(to_unsigned(dd_buffer_ctr, rd_tmp'length));
                wr_tmp := std_logic_vector(to_unsigned(ddd_buffer,    wr_tmp'length));
                case img_r is
                    when "001" =>
                        rd_modulo      <= rd_tmp(1 downto 0);
                        wr_modulo      <= wr_tmp(1 downto 0);
                        data_rd_modulo <= rd_tmp(1 downto 0); 
                    when "010" =>
                        rd_modulo      <= "0" & rd_tmp(0);
                        wr_modulo      <= "0" & wr_tmp(0);
                        data_rd_modulo <= "0" & rd_tmp(0);
                    when others =>
                        rd_modulo      <= (others => '0');
                        wr_modulo      <= (others => '0');
                        data_rd_modulo <= (others => '0');
                end case;
            end if;
        end if;
    end process modulo_proc;

    -- data_modulo_proc: gejtovano sa ram_en
    data_modulo_proc: process(clk) 
    begin 
        if rising_edge(clk) then
            if reset = '1' or img_processing = '0' then
                d_rd_modulo <= (others => '0');
                data_modulo <= (others => '0');
            elsif ram_en = '1' then
                d_rd_modulo <= data_rd_modulo; 
                data_modulo <= d_rd_modulo;
            end if;
        end if;
    end process data_modulo_proc;


end architecture rtl;