library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

library work;
use work.ARR_TYPES.all;
use work.MY_STATES_PKG.all;

entity grid_system is
    Port (
        clk : in std_logic;
        reset : in std_logic;

        input_pixel : in std_logic_vector(7 downto 0);
        reg_tvalid  : in std_logic;
        reg_tlast   : in std_logic;
        last_pixel  : out std_logic;

        img_r : in std_logic_vector(2 downto 0);
        filter_coeffs : in arr16bit(0 to 80);
        reg_coeff_scale : in std_logic_vector(15 downto 0);

        imgW, imgH : in std_logic_vector(15 downto 0);
        mode       : in std_logic;

        img_processing : in std_logic;
        skip_mode      : in std_logic;
        config_mode    : in std_logic; 
        in_handshake   : in std_logic;
        
        ram_ctr        : in std_logic_vector(23 downto 0);
        state          : in state_t;

        alu          : in arr8bit(0 to 80);
        in_col       : in std_logic_vector(15 downto 0);
        in_row       : in std_logic_vector(15 downto 0);
        
        clk_en       : in std_logic;
        start_output : in std_logic;
        
        output_enable   : out std_logic;
        processing_done : out std_logic;
        output_val      : out std_logic_vector(15 downto 0)
        
    );
end grid_system;

architecture rtl of grid_system is

    signal acc_out_sig : std_logic_vector(15 downto 0);

begin
    
    core_inst : entity work.convolution_core
    port map (
        clk             => clk,
        reset           => reset,
        
        img_processing  => img_processing,
        
        state           => state, 
        
        mode            => mode,
        alu             => alu,
        
        filter_coeffs   => filter_coeffs,
        reg_coeff_scale => reg_coeff_scale,
        pipe_en         => clk_en, 
        
        acc_out         => acc_out_sig
    );
    
    ctrl_inst : entity work.output_controller
    port map (
        clk             => clk,
        reset           => reset,
        
        state           => state,
        pipe_en         => clk_en, 
        in_handshake    => in_handshake, 
        
        input_pixel     => input_pixel,
        reg_tvalid      => reg_tvalid, 
        reg_tlast       => reg_tlast, 
        acc_out         => acc_out_sig,      
        
        imgW            => imgW,             
        imgH            => imgH,
        img_r           => img_r,
                    
        img_processing  => img_processing, 
        skip_mode       => skip_mode, 
        config_mode     => config_mode,
        mode            => mode, 
        
        in_row          => in_row, 
        in_col          => in_col,         
        
        output_enable   => output_enable,
        processing_done => processing_done,
        output_val      => output_val,
        
        last_pixel      => last_pixel 
    );
    
end architecture rtl;