library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.ARR_TYPES.all;
use work.MY_STATES_PKG.ALL;
use work.CONSTANTS.ALL;


-- axi_controller - Top level AXI wrapper
entity axi_controller is
    Generic (
        C_S_AXI_DATA_WIDTH : integer := 32;
        C_S_AXI_ADDR_WIDTH : integer := 10;
        ADDR_LSB           : natural := 2;
        C_MAX_IMG_W        : integer := 1024;
        C_MAX_IMG_H        : integer := 1024
    );
    Port (
        reset : in STD_LOGIC;
        clk   : in STD_LOGIC;

        -- AXI4-Lite Write
        s_axi_ctrl_awaddr  : in  std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
        s_axi_ctrl_awprot  : in  std_logic_vector(2 downto 0);
        s_axi_ctrl_awvalid : in  std_logic;
        s_axi_ctrl_awready : out std_logic;
        
        s_axi_ctrl_wdata   : in  std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        s_axi_ctrl_wstrb   : in  std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
        s_axi_ctrl_wvalid  : in  std_logic;
        s_axi_ctrl_wready  : out std_logic;
        
        s_axi_ctrl_bresp   : out std_logic_vector(1 downto 0);
        s_axi_ctrl_bvalid  : out std_logic;
        s_axi_ctrl_bready  : in  std_logic;

        -- AXI4-Lite Read
        s_axi_ctrl_araddr  : in  std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
        s_axi_ctrl_arprot  : in  std_logic_vector(2 downto 0);
        s_axi_ctrl_arvalid : in  std_logic;
        s_axi_ctrl_arready : out std_logic;
        
        s_axi_ctrl_rdata   : out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
        s_axi_ctrl_rvalid  : out std_logic;
        s_axi_ctrl_rready  : in  std_logic;
        s_axi_ctrl_rresp   : out std_logic_vector(1 downto 0);

        -- AXI4-Stream Input (od DMA TX)
        s_axis_data_in_tdata  : in  std_logic_vector(7 downto 0);
        s_axis_data_in_tvalid : in  std_logic;
        s_axis_data_in_tready : out std_logic;
        s_axis_data_in_tlast  : in  std_logic;

        -- AXI4-Stream Output (ka DMA RX)
        m_axis_data_out_tdata  : out std_logic_vector(15 downto 0);
        m_axis_data_out_tvalid : out std_logic;
        m_axis_data_out_tready : in  std_logic;
        m_axis_data_out_tlast  : out std_logic;

        state               : in  state_t;
        lock_config         : in STD_LOGIC;       
        pipe_en             : out  STD_LOGIC; 
        
        -- input signali za validan izlazni pixel 
        m_tlast             : in  std_logic;
        m_tvalid            : in  std_logic;
        m_tdata             : in  std_logic_vector(15 downto 0);
        
        -- axi signali sacuvani od gubitka 
        reg_input           : out std_logic_vector(7 downto 0);
        reg_tvalid          : out std_logic;
        reg_tlast           : out std_logic;
               
        -- desio se ulazni handshake        
        in_handshake        : out STD_LOGIC;
        
        -- kontrolni signali 
        img_processing      : in std_logic;
        config_mode         : in std_logic;
        skip_mode           : in std_logic;
        
        -- konfiguracioni parametri 
        reg_ctrl_out        : out std_logic_vector(15 downto 0);
        reg_radius_out      : out std_logic_vector(15 downto 0);
        
        reg_img_w_out       : out std_logic_vector(15 downto 0);
        reg_img_h_out       : out std_logic_vector(15 downto 0);
        
        reg_coeff_scale_out : out std_logic_vector(15 downto 0);
        reg_coeffs_out      : out arr16bit(0 to 80)
    ); 
end entity axi_controller;

architecture rtl of axi_controller is

    -- Interne konekcije
    signal reg_waddr_i       : std_logic_vector(C_S_AXI_ADDR_WIDTH-ADDR_LSB-1 downto 0);
    signal reg_raddr_i       : std_logic_vector(C_S_AXI_ADDR_WIDTH-ADDR_LSB-1 downto 0);
    
    signal axi_write_ready_i : std_logic;
    signal rdata_i           : std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
  
    attribute mark_debug     : string;
    attribute mark_debug of reg_waddr_i : signal is "true";
    attribute mark_debug of reg_raddr_i : signal is "true";

begin

    -- AXI Write Slave
    AXI_LITE_WRITE_INST: entity work.axi_write_slave(rtl) 
        generic map(
            C_S_AXI_DATA_WIDTH => C_S_AXI_DATA_WIDTH,
            C_S_AXI_ADDR_WIDTH => C_S_AXI_ADDR_WIDTH,
            ADDR_LSB           => ADDR_LSB
        )
        port map(
            clk   => clk,
            reset => reset,

            s_axi_awaddr  => s_axi_ctrl_awaddr,
            s_axi_awvalid => s_axi_ctrl_awvalid,
            s_axi_awready => s_axi_ctrl_awready,
            s_axi_awprot  => s_axi_ctrl_awprot, 

            s_axi_wdata  => s_axi_ctrl_wdata,
            s_axi_wstrb  => s_axi_ctrl_wstrb,
            s_axi_wvalid => s_axi_ctrl_wvalid,
            s_axi_wready => s_axi_ctrl_wready,

            s_axi_bresp  => s_axi_ctrl_bresp,
            s_axi_bvalid => s_axi_ctrl_bvalid,
            s_axi_bready => s_axi_ctrl_bready,

            reg_waddr       => reg_waddr_i,
            axi_write_ready => axi_write_ready_i
        );

    -- AXI Read Slave
    AXI_LITE_READ_INST: entity work.axi_read_slave(rtl)
        generic map(
            C_S_AXI_DATA_WIDTH => C_S_AXI_DATA_WIDTH,
            C_S_AXI_ADDR_WIDTH => C_S_AXI_ADDR_WIDTH,
            ADDR_LSB           => ADDR_LSB
        )
        port map(
            clk   => clk,
            reset => reset,

            s_axi_araddr  => s_axi_ctrl_araddr,
            s_axi_arprot  => s_axi_ctrl_arprot,
            s_axi_arvalid => s_axi_ctrl_arvalid,
            s_axi_arready => s_axi_ctrl_arready,

            s_axi_rdata  => s_axi_ctrl_rdata,
            s_axi_rvalid => s_axi_ctrl_rvalid,
            s_axi_rready => s_axi_ctrl_rready,
            s_axi_rresp  => s_axi_ctrl_rresp,

            reg_raddr => reg_raddr_i,
            reg_rdata => rdata_i
        );

    -- Register File
    REG_FILE_INST: entity work.register_file(rtl)
        generic map(
            C_S_AXI_DATA_WIDTH => C_S_AXI_DATA_WIDTH,
            C_S_AXI_ADDR_WIDTH => C_S_AXI_ADDR_WIDTH,
            ADDR_LSB           => ADDR_LSB
        )
        port map(
            clk   => clk,
            reset => reset,

            reg_waddr           => reg_waddr_i,
            axi_write_ready     => axi_write_ready_i,
            wdata               => s_axi_ctrl_wdata,
            wstrb               => s_axi_ctrl_wstrb,
            lock_config         => lock_config,

            reg_raddr           => reg_raddr_i,
            rdata               => rdata_i,

            reg_ctrl_out        => reg_ctrl_out,
            reg_radius_out      => reg_radius_out,
            reg_img_w_out       => reg_img_w_out,
            reg_img_h_out       => reg_img_h_out,
            reg_coeff_scale_out => reg_coeff_scale_out,
            reg_coeffs_out      => reg_coeffs_out,

            regs_written        => open
        );

    -- AXI Stream Pipeline
    PIPELINE_INST: entity work.axi_stream_io_block(rtl)
        port map(
            clk   => clk,
            reset => reset,

            pipe_en        => pipe_en,
            m_tdata        => m_tdata,
            m_tvalid       => m_tvalid, 
            m_tlast        => m_tlast, 
            
            img_processing => img_processing, 
            config_mode    => config_mode, 
            skip_mode      => skip_mode, 

            in_handshake   => in_handshake, 
            reg_input      => reg_input,    
            reg_tvalid     => reg_tvalid, 
            reg_tlast      => reg_tlast,           
            state          => state,

            s_axis_tdata  => s_axis_data_in_tdata,
            s_axis_tvalid => s_axis_data_in_tvalid,
            s_axis_tready => s_axis_data_in_tready,
            s_axis_tlast  => s_axis_data_in_tlast,

            m_axis_tdata  => m_axis_data_out_tdata,
            m_axis_tvalid => m_axis_data_out_tvalid,
            m_axis_tready => m_axis_data_out_tready,
            m_axis_tlast  => m_axis_data_out_tlast
        );


end architecture rtl; 