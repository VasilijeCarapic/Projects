library IEEE; 
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library work;
use work.RAM_PACKAGE.all;
use work.ARR_TYPES.all; 
use work.MY_STATES_PKG.all;
use work.CONSTANTS.all; 

entity output_controller is
    Port (
        clk                 : in std_logic;
        reset               : in std_logic;

        img_processing      : in std_logic;
        skip_mode           : in std_logic;
        config_mode         : in std_logic; 
        
        state               : in state_t;

        input_pixel         : in std_logic_vector(7 downto 0);
        acc_out             : in std_logic_vector(15 downto 0);

        imgW, imgH          : in std_logic_vector(15 downto 0);
        in_row, in_col      : in std_logic_vector(15 downto 0);
        img_r               : in std_logic_vector(2 downto 0);

        pipe_en             : in std_logic;
        mode                : in std_logic;

        in_handshake        : in std_logic;
        reg_tvalid          : in std_logic;
        reg_tlast           : in std_logic;

        output_enable       : out std_logic;
        processing_done     : out std_logic;
        output_val          : out std_logic_vector(15 downto 0);
        
        last_pixel          : out std_logic
    );
end output_controller;

architecture rtl of output_controller is

signal tag_valid        : std_logic;
signal tag_last         : std_logic;
signal m_tvalid_dly     : std_logic;
signal m_tlast_dly      : std_logic;

signal frame_done_latch : std_logic := '0';
signal input_pixel_dly  : arr8bit(0 to PIPELINE_DELAY + 1) := (others => (others => '0')); 

-- Odabrani piksel za pakovanje (acc_out ili odloženi input_pixel za bypass)
signal in_px_to_packer  : std_logic_vector(7 downto 0);

signal packed_data      : std_logic_vector(15 downto 0);
signal packed_valid     : std_logic;
signal packed_last      : std_logic;

-- Registri za izjednačavanje kašnjenja mode = '1' sa packed režimom (PIPELINE_DELAY + 1)
signal mode1_data_reg   : std_logic_vector(15 downto 0) := (others => '0');
signal mode1_valid_reg  : std_logic := '0';
signal mode1_last_reg   : std_logic := '0';

-- napravim da signal kasni jedan takt i to bude acc_out ili zakasnjeni signal 
signal d_acc_out     : std_logic_vector(15 downto 0);
signal dx2_acc_out   : std_logic_vector(15 downto 0); 

-- finalni acc_out je ili obican acc_out ili zakasnjeni acc_out 
signal final_acc_out    : std_logic_vector(15 downto 0); 


begin
    
 delay_output: process(clk) is 
 begin 
    if(rising_edge(clk)) then 
        if pipe_en = '1' then 
            d_acc_out <= acc_out;
            dx2_acc_out <= d_acc_out;             
        end if;
    end if; 
 end process delay_output; 
 
 -- ovo je finalni acc_out koji se koristi posle u svim ostalim slucajevima ; 
 final_acc_out <= d_acc_out;   

 -- Selektujemo acc_out za obradu, odnosno input_pixel(PIPELINE_DELAY + 1) za bypass
 in_px_to_packer <= final_acc_out(7 downto 0) when (img_processing = '1' and mode = '0') 
                   else input_pixel_dly(PIPELINE_DELAY + 1);

-- Instanciranje AXI Generatora kontrolnih signala
axi_control_gen_inst: entity work.axi_control_gen(rtl)
    port map (
        clk            => clk,
        reset          => reset,
        
        pipe_en        => pipe_en,
        config_mode    => config_mode,
        img_processing => img_processing,
        skip_mode      => skip_mode,
        
        state          => state,
        imgW           => imgW,
        imgH           => imgH,
        img_r          => img_r,
        
        in_row         => in_row,
        in_col         => in_col,
        
        reg_tvalid     => reg_tvalid,
        reg_tlast      => reg_tlast,
        tag_valid      => tag_valid,
        tag_last       => tag_last
    );

m_tvalid_delay_inst: entity work.valid_delay_line(rtl)
    generic map (DEPTH => PIPELINE_DELAY)
    port map (
        clk         => clk,
        reset       => reset,
        config_mode => config_mode,
        pipe_en     => pipe_en,
        tag_in      => tag_valid,
        tag_out     => m_tvalid_dly
    );

m_tlast_delay_inst: entity work.valid_delay_line(rtl)
    generic map (DEPTH => PIPELINE_DELAY)
    port map (
        clk         => clk,
        reset       => reset,
        config_mode => config_mode,
        pipe_en     => pipe_en,
        tag_in      => tag_last,
        tag_out     => m_tlast_dly
    );

-- Instanciranje Pixel Packera (pakuje za mode = '0' ILI skip_mode = '1')
pixel_packer_inst: entity work.pixel_packer(rtl)
    port map (
        clk            => clk,
        reset          => reset,
        
        pipe_en        => pipe_en,
        mode           => mode,
        
        img_processing => img_processing,
        skip_mode      => skip_mode, 
        config_mode    => config_mode, 
        
        m_tvalid_dly   => m_tvalid_dly,
        m_tlast_dly    => m_tlast_dly,
        
        in_px          => in_px_to_packer,
        
        packed_data    => packed_data,
        packed_valid   => packed_valid,
        packed_last    => packed_last
    );

-- Delay linija za sirovi piksel
input_pixel_delay_proc: process(clk) is
begin
    if rising_edge(clk) then
        if (pipe_en = '1') then
            input_pixel_dly(0) <= input_pixel;
            for i in 1 to PIPELINE_DELAY + 1 loop
                input_pixel_dly(i) <= input_pixel_dly(i-1);
            end loop;
        end if;
    end if;
end process input_pixel_delay_proc;

-- Registracija za mode = '1' radi izjednačavanja kašnjenja
delay_match_proc: process(clk)
begin
    if rising_edge(clk) then
        if reset = '1' or config_mode = '1' or state = Config or state = Idle then
            mode1_data_reg  <= (others => '0');
            mode1_valid_reg <= '0';
            mode1_last_reg  <= '0';
        elsif pipe_en = '1' then
            mode1_data_reg  <= final_acc_out;
            mode1_valid_reg <= m_tvalid_dly;
            mode1_last_reg  <= m_tlast_dly;
        end if;
    end if;
end process delay_match_proc;

-- Kombinaciona dodela izlaza
output_mapping_proc: process(
    img_processing, skip_mode, mode, 
    packed_data, packed_valid, packed_last,
    mode1_data_reg, mode1_valid_reg, mode1_last_reg
)
begin
    if img_processing = '1' then
        if mode = '0' then
            output_val      <= packed_data;
            output_enable   <= packed_valid;
            last_pixel      <= packed_last;
            processing_done <= packed_valid and packed_last;
        else
            output_val      <= mode1_data_reg;
            output_enable   <= mode1_valid_reg;
            last_pixel      <= mode1_last_reg;
            processing_done <= mode1_valid_reg and mode1_last_reg;
        end if;
    elsif skip_mode = '1' then
        -- Bypass koristi spakovani izlaz iz pixel_packer-a!
        output_val      <= packed_data;
        output_enable   <= packed_valid;
        last_pixel      <= packed_last;
        processing_done <= packed_valid and packed_last;
    else
        output_val      <= (others => '0');
        output_enable   <= '0';
        last_pixel      <= '0';
        processing_done <= '0';
    end if;
end process output_mapping_proc;

end architecture rtl;    