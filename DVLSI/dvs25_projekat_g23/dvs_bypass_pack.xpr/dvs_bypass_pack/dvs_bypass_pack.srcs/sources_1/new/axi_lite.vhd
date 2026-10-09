library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
library work;
use work.RAM_PACKAGE.all;
use work.ARR_TYPES.all;   
use work.MY_STATES_PKG.ALL;
use work.CONSTANTS.ALL;      

entity acc_image_filter is
    Generic (
        C_S_AXI_DATA_WIDTH : integer := 32;
        C_S_AXI_ADDR_WIDTH : integer := 10
    );
    Port ( reset : in STD_LOGIC;
           clk   : in STD_LOGIC;
           		
           -------- AXI4-Lite interface -------
		   s_axi_ctrl_awaddr  : in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		   s_axi_ctrl_awprot  : in std_logic_vector(2 downto 0);
		   s_axi_ctrl_awvalid : in std_logic;
		   s_axi_ctrl_awready : out std_logic;
		
		   s_axi_ctrl_wdata  : in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		   s_axi_ctrl_wstrb  : in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
		   s_axi_ctrl_wvalid : in std_logic;
		   s_axi_ctrl_wready : out std_logic;
		
		   s_axi_ctrl_bresp  : out std_logic_vector(1 downto 0);
		   s_axi_ctrl_bvalid : out std_logic;
		   s_axi_ctrl_bready : in std_logic;
		
		   s_axi_ctrl_araddr  : in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		   s_axi_ctrl_arprot  : in std_logic_vector(2 downto 0);
		   s_axi_ctrl_arvalid : in std_logic;
		   s_axi_ctrl_arready : out std_logic;
		
		   s_axi_ctrl_rdata  : out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		   s_axi_ctrl_rvalid : out std_logic;
		   s_axi_ctrl_rready : in std_logic;
		   s_axi_ctrl_rresp  : out std_logic_vector(1 downto 0);  

           -- Input AXI Stream interface
           s_axis_data_in_tdata  : in STD_LOGIC_VECTOR (7 downto 0);
           s_axis_data_in_tvalid : in STD_LOGIC;
           s_axis_data_in_tready : out STD_LOGIC;
           s_axis_data_in_tlast  : in STD_LOGIC;
           
           -- Output AXI Stream interface
           m_axis_data_out_tdata  : out STD_LOGIC_VECTOR (15 downto 0);
           m_axis_data_out_tvalid : out STD_LOGIC;
           m_axis_data_out_tready : in STD_LOGIC;
           m_axis_data_out_tlast  : out STD_LOGIC);    
end entity acc_image_filter;   
  
architecture rtl of acc_image_filter is                                                           

    -- AXI Lite controller signali         
    signal reg_ctrl: std_logic_vector  (15 downto 0);
    signal reg_radius: std_logic_vector(15 downto 0);
    signal reg_img_w: std_logic_vector (15 downto 0);
    signal reg_img_h: std_logic_vector (15 downto 0);
    signal reg_coeff_scale: std_logic_vector(15 downto 0);
    
    signal filter_coeffs : arr16bit(0 to 80);
    signal regs_written  : std_logic;
    
    -- state machine 
    signal state      : state_t;
    signal next_state : state_t;
    
    signal imgW: std_logic_vector(15 downto 0);
    signal imgH: std_logic_vector(15 downto 0);
    
    signal s_tready_i : std_logic := '0';
    
	-- Internal signals used for processing 
    signal img_r         : std_logic_vector( 2 downto 0);
    signal border        : std_logic_vector(1 downto 0):= "00";
    signal border_value  : std_logic_vector(7 downto 0) := (others => '0');
	
	signal bypass        : std_logic := '0';
	signal mode          : std_logic := '0';
	
	-- Enable signals  
	signal m_tvalid           : std_logic := '0';
	signal m_tlast            : std_logic := '0';
	
	signal proc_done          : std_logic := '0';
	signal lock_config        : std_logic := '0';
	
    -- MAIN STATE MACHINE signals 
	signal pixel_output: std_logic_vector( 15 downto 0); 
    signal shift       : std_logic_vector(  2 downto 0);
    
    --  Active signals 
    signal img_processing    : std_logic := '0';
    signal skip_mode         : std_logic := '0';
    signal config_mode       : std_logic := '0'; 
    
    -- pipe_en kad je processing
    signal pipe_en           : std_logic := '0';
    signal in_handshake      : std_logic := '0';
    
    -- flagovi za ulazni pixel -> pipeline_inst => axi_controller => axi_lite => img_filter
    signal reg_input   : std_logic_vector(7 downto 0);
    signal reg_tvalid  : std_logic;
    signal reg_tlast   : std_logic;
    
    -- ovo je bukvalno samo reg_output i zato je 11 taktova on posle ali 
    -- ima jos taktova nakon njega ok ok 
    signal m_tdata     : std_logic_vector(15 downto 0) := (others => '0');
    
    attribute max_fanout : integer;
    attribute max_fanout of reset : signal is 150;
    
    attribute mark_debug                : string;
    attribute mark_debug of reg_input   : signal is "true";
 
begin

    s_axis_data_in_tready <= s_tready_i; 

    --  instanciranja 
    IMG_FiLTER: entity work.alu_logic(rtl)
    port map(
        reset => reset, 
        clk   => clk, 
        
        img_r => reg_radius(2 downto 0), 
        mode => mode, 
        
        reg_input       => reg_input, 
        reg_tvalid      => reg_tvalid, 
        reg_tlast       => reg_tlast, 
        
        img_processing  => img_processing,
        processing_done => proc_done, 
        
        skip_mode       => skip_mode,
        config_mode     => config_mode,
        
        pipe_en         => pipe_en,
        in_handshake    => in_handshake, 
        
        imgW            => imgW, 
        imgH            => imgH,     
        shift           => shift, 
        
        reg_coeff_scale => reg_coeff_scale,
        filter_coeffs   => filter_coeffs,
        
        state           => state, 
        output_enable   => m_tvalid,
        last_pixel      => m_tlast,
        pixel_output    => m_tdata 
        
    );
    
    FSM_CONTROLLER: entity work.filter_fsm(rtl) 
    generic map(
        C_S_AXI_ADDR_WIDTH => C_S_AXI_ADDR_WIDTH
    )
    port map(
        clk => clk, 
        reset => reset, 
        
        bypass_in           => bypass,
        border_in           => border, 

        s_axis_tvalid       => s_axis_data_in_tvalid,
        s_axis_tready       => s_tready_i,
        
        regs_written        => regs_written, 
        
        state               => state, 
        next_state          => next_state, 
        
        img_processing      => img_processing,
        skip_mode           => skip_mode,
        
        config_mode         => config_mode, 
        
        processing_done     => proc_done,
        lock_config         => lock_config
        
    );
    
    PL_INST: entity work.params_loader(Behavioral)
    port map(
        clk         => clk,
        reset       => reset,
                         
         -- registri iz kojih citam
        reg_ctrl    => reg_ctrl,
        reg_radius  => reg_radius, 
        
        reg_img_w   => reg_img_w,
        reg_img_h   => reg_img_h,
        
        state       => state, 
                         
         -- koeficijenti 
        filter_coeffs => filter_coeffs, 
 
         -- parametri sistema
        imgW          => imgW,
        imgH          => imgH,
        
        div_shift     => shift, 
        img_r         => img_r,
        
        border        => border, 
        border_value  => border_value,
        
        bypass        => bypass,
        mode          => mode
    );
    
    -- axi controller instanciranje
    AXI_CTRL: entity work.axi_controller(rtl)          
    generic map(
        C_S_AXI_DATA_WIDTH => C_S_AXI_DATA_WIDTH,
        C_S_AXI_ADDR_WIDTH => C_S_AXI_ADDR_WIDTH
    )
    port map(
        clk       => clk,
        reset     => reset,
        
        -- AXI-Lite ports
        s_axi_ctrl_awaddr  => s_axi_ctrl_awaddr,
        s_axi_ctrl_awprot  => s_axi_ctrl_awprot,
        s_axi_ctrl_awvalid => s_axi_ctrl_awvalid,
        s_axi_ctrl_awready => s_axi_ctrl_awready,
        
        s_axi_ctrl_wdata   => s_axi_ctrl_wdata,
        s_axi_ctrl_wstrb   => s_axi_ctrl_wstrb,
        s_axi_ctrl_wvalid  => s_axi_ctrl_wvalid,
        s_axi_ctrl_wready  => s_axi_ctrl_wready,
        
        s_axi_ctrl_bresp   => s_axi_ctrl_bresp,
        s_axi_ctrl_bvalid  => s_axi_ctrl_bvalid,
        s_axi_ctrl_bready  => s_axi_ctrl_bready,
        
        s_axi_ctrl_araddr  => s_axi_ctrl_araddr,
        s_axi_ctrl_arprot  => s_axi_ctrl_arprot,
        s_axi_ctrl_arvalid => s_axi_ctrl_arvalid,
        s_axi_ctrl_arready => s_axi_ctrl_arready,
        
        s_axi_ctrl_rdata   => s_axi_ctrl_rdata,
        s_axi_ctrl_rvalid  => s_axi_ctrl_rvalid,
        s_axi_ctrl_rready  => s_axi_ctrl_rready,
        s_axi_ctrl_rresp   => s_axi_ctrl_rresp,
        
        -- AXI Stream
        s_axis_data_in_tdata  => s_axis_data_in_tdata,
        s_axis_data_in_tvalid => s_axis_data_in_tvalid,
        s_axis_data_in_tready => s_tready_i,
        s_axis_data_in_tlast  => s_axis_data_in_tlast,
        
        m_axis_data_out_tdata  => m_axis_data_out_tdata,
        m_axis_data_out_tvalid => m_axis_data_out_tvalid,
        m_axis_data_out_tready => m_axis_data_out_tready,
        m_axis_data_out_tlast  => m_axis_data_out_tlast,
        
        -- interne veze sa filterom
        state              =>  state,
        
        img_processing     => img_processing, 
        skip_mode          => skip_mode, 
        config_mode        => config_mode, 
        
        lock_config        => lock_config,
        
        -- izlazni podaci filtra 
        pipe_en        => pipe_en,
        m_tdata        => m_tdata, 
        m_tvalid       => m_tvalid,
        m_tlast        => m_tlast, 
            
        in_handshake   => in_handshake, 
        reg_input      => reg_input,        
        reg_tvalid     => reg_tvalid, 
        reg_tlast      => reg_tlast, 
        
        -- registri: axi_interface -> top_module 
        reg_ctrl_out        => reg_ctrl,
        reg_img_w_out       => reg_img_w,
        reg_img_h_out       => reg_img_h,
        reg_radius_out      => reg_radius,
        reg_coeff_scale_out => reg_coeff_scale,
        reg_coeffs_out      => filter_coeffs
    );
  
end architecture rtl;