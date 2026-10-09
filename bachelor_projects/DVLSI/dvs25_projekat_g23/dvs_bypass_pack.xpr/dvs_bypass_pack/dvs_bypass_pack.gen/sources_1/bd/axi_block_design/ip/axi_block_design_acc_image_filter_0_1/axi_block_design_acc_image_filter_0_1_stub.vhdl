-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
-- Date        : Wed Jul 29 20:17:55 2026
-- Host        : windows_rip running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub
--               e:/dvs_project_u16/dvs_bypass_pack/dvs_bypass_pack.gen/sources_1/bd/axi_block_design/ip/axi_block_design_acc_image_filter_0_1/axi_block_design_acc_image_filter_0_1_stub.vhdl
-- Design      : axi_block_design_acc_image_filter_0_1
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xc7z020clg400-1
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity axi_block_design_acc_image_filter_0_1 is
  Port ( 
    reset : in STD_LOGIC;
    clk : in STD_LOGIC;
    s_axi_ctrl_awaddr : in STD_LOGIC_VECTOR ( 9 downto 0 );
    s_axi_ctrl_awprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axi_ctrl_awvalid : in STD_LOGIC;
    s_axi_ctrl_awready : out STD_LOGIC;
    s_axi_ctrl_wdata : in STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_ctrl_wstrb : in STD_LOGIC_VECTOR ( 3 downto 0 );
    s_axi_ctrl_wvalid : in STD_LOGIC;
    s_axi_ctrl_wready : out STD_LOGIC;
    s_axi_ctrl_bresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axi_ctrl_bvalid : out STD_LOGIC;
    s_axi_ctrl_bready : in STD_LOGIC;
    s_axi_ctrl_araddr : in STD_LOGIC_VECTOR ( 9 downto 0 );
    s_axi_ctrl_arprot : in STD_LOGIC_VECTOR ( 2 downto 0 );
    s_axi_ctrl_arvalid : in STD_LOGIC;
    s_axi_ctrl_arready : out STD_LOGIC;
    s_axi_ctrl_rdata : out STD_LOGIC_VECTOR ( 31 downto 0 );
    s_axi_ctrl_rvalid : out STD_LOGIC;
    s_axi_ctrl_rready : in STD_LOGIC;
    s_axi_ctrl_rresp : out STD_LOGIC_VECTOR ( 1 downto 0 );
    s_axis_data_in_tdata : in STD_LOGIC_VECTOR ( 7 downto 0 );
    s_axis_data_in_tvalid : in STD_LOGIC;
    s_axis_data_in_tready : out STD_LOGIC;
    s_axis_data_in_tlast : in STD_LOGIC;
    m_axis_data_out_tdata : out STD_LOGIC_VECTOR ( 15 downto 0 );
    m_axis_data_out_tvalid : out STD_LOGIC;
    m_axis_data_out_tready : in STD_LOGIC;
    m_axis_data_out_tlast : out STD_LOGIC
  );

  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of axi_block_design_acc_image_filter_0_1 : entity is "axi_block_design_acc_image_filter_0_1,acc_image_filter,{}";
  attribute core_generation_info : string;
  attribute core_generation_info of axi_block_design_acc_image_filter_0_1 : entity is "axi_block_design_acc_image_filter_0_1,acc_image_filter,{x_ipProduct=Vivado 2025.1,x_ipVendor=xilinx.com,x_ipLibrary=module_ref,x_ipName=acc_image_filter,x_ipVersion=1.0,x_ipCoreRevision=1,x_ipLanguage=VHDL,x_ipSimLanguage=MIXED,C_S_AXI_DATA_WIDTH=32,C_S_AXI_ADDR_WIDTH=10}";
  attribute downgradeipidentifiedwarnings : string;
  attribute downgradeipidentifiedwarnings of axi_block_design_acc_image_filter_0_1 : entity is "yes";
  attribute ip_definition_source : string;
  attribute ip_definition_source of axi_block_design_acc_image_filter_0_1 : entity is "module_ref";
end axi_block_design_acc_image_filter_0_1;

architecture stub of axi_block_design_acc_image_filter_0_1 is
  attribute syn_black_box : boolean;
  attribute black_box_pad_pin : string;
  attribute syn_black_box of stub : architecture is true;
  attribute black_box_pad_pin of stub : architecture is "reset,clk,s_axi_ctrl_awaddr[9:0],s_axi_ctrl_awprot[2:0],s_axi_ctrl_awvalid,s_axi_ctrl_awready,s_axi_ctrl_wdata[31:0],s_axi_ctrl_wstrb[3:0],s_axi_ctrl_wvalid,s_axi_ctrl_wready,s_axi_ctrl_bresp[1:0],s_axi_ctrl_bvalid,s_axi_ctrl_bready,s_axi_ctrl_araddr[9:0],s_axi_ctrl_arprot[2:0],s_axi_ctrl_arvalid,s_axi_ctrl_arready,s_axi_ctrl_rdata[31:0],s_axi_ctrl_rvalid,s_axi_ctrl_rready,s_axi_ctrl_rresp[1:0],s_axis_data_in_tdata[7:0],s_axis_data_in_tvalid,s_axis_data_in_tready,s_axis_data_in_tlast,m_axis_data_out_tdata[15:0],m_axis_data_out_tvalid,m_axis_data_out_tready,m_axis_data_out_tlast";
  attribute max_fanout : string;
  attribute max_fanout of reset : signal is "150";
  attribute x_interface_info : string;
  attribute x_interface_info of reset : signal is "xilinx.com:signal:reset:1.0 reset RST";
  attribute x_interface_mode : string;
  attribute x_interface_mode of reset : signal is "slave reset";
  attribute x_interface_parameter : string;
  attribute x_interface_parameter of reset : signal is "XIL_INTERFACENAME reset, POLARITY ACTIVE_HIGH, INSERT_VIP 0";
  attribute x_interface_info of clk : signal is "xilinx.com:signal:clock:1.0 clk CLK";
  attribute x_interface_mode of clk : signal is "slave clk";
  attribute x_interface_parameter of clk : signal is "XIL_INTERFACENAME clk, ASSOCIATED_BUSIF m_axis_data_out:s_axis_data_in:s_axi_ctrl, ASSOCIATED_RESET reset, FREQ_HZ 1e+08, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, INSERT_VIP 0";
  attribute x_interface_info of s_axi_ctrl_awaddr : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWADDR";
  attribute x_interface_mode of s_axi_ctrl_awaddr : signal is "slave s_axi_ctrl";
  attribute x_interface_parameter of s_axi_ctrl_awaddr : signal is "XIL_INTERFACENAME s_axi_ctrl, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 1e+08, ID_WIDTH 0, ADDR_WIDTH 10, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0";
  attribute x_interface_info of s_axi_ctrl_awprot : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWPROT";
  attribute x_interface_info of s_axi_ctrl_awvalid : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWVALID";
  attribute x_interface_info of s_axi_ctrl_awready : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWREADY";
  attribute x_interface_info of s_axi_ctrl_wdata : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl WDATA";
  attribute x_interface_info of s_axi_ctrl_wstrb : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl WSTRB";
  attribute x_interface_info of s_axi_ctrl_wvalid : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl WVALID";
  attribute x_interface_info of s_axi_ctrl_wready : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl WREADY";
  attribute x_interface_info of s_axi_ctrl_bresp : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl BRESP";
  attribute x_interface_info of s_axi_ctrl_bvalid : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl BVALID";
  attribute x_interface_info of s_axi_ctrl_bready : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl BREADY";
  attribute x_interface_info of s_axi_ctrl_araddr : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARADDR";
  attribute x_interface_info of s_axi_ctrl_arprot : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARPROT";
  attribute x_interface_info of s_axi_ctrl_arvalid : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARVALID";
  attribute x_interface_info of s_axi_ctrl_arready : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARREADY";
  attribute x_interface_info of s_axi_ctrl_rdata : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl RDATA";
  attribute x_interface_info of s_axi_ctrl_rvalid : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl RVALID";
  attribute x_interface_info of s_axi_ctrl_rready : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl RREADY";
  attribute x_interface_info of s_axi_ctrl_rresp : signal is "xilinx.com:interface:aximm:1.0 s_axi_ctrl RRESP";
  attribute x_interface_info of s_axis_data_in_tdata : signal is "xilinx.com:interface:axis:1.0 s_axis_data_in TDATA";
  attribute x_interface_mode of s_axis_data_in_tdata : signal is "slave s_axis_data_in";
  attribute x_interface_parameter of s_axis_data_in_tdata : signal is "XIL_INTERFACENAME s_axis_data_in, TDATA_NUM_BYTES 1, TDEST_WIDTH 0, TID_WIDTH 0, TUSER_WIDTH 0, HAS_TREADY 1, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 1, FREQ_HZ 1e+08, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, LAYERED_METADATA undef, INSERT_VIP 0";
  attribute x_interface_info of s_axis_data_in_tvalid : signal is "xilinx.com:interface:axis:1.0 s_axis_data_in TVALID";
  attribute x_interface_info of s_axis_data_in_tready : signal is "xilinx.com:interface:axis:1.0 s_axis_data_in TREADY";
  attribute x_interface_info of s_axis_data_in_tlast : signal is "xilinx.com:interface:axis:1.0 s_axis_data_in TLAST";
  attribute x_interface_info of m_axis_data_out_tdata : signal is "xilinx.com:interface:axis:1.0 m_axis_data_out TDATA";
  attribute x_interface_mode of m_axis_data_out_tdata : signal is "master m_axis_data_out";
  attribute x_interface_parameter of m_axis_data_out_tdata : signal is "XIL_INTERFACENAME m_axis_data_out, TDATA_NUM_BYTES 2, TDEST_WIDTH 0, TID_WIDTH 0, TUSER_WIDTH 0, HAS_TREADY 1, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 1, FREQ_HZ 1e+08, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, LAYERED_METADATA undef, INSERT_VIP 0";
  attribute x_interface_info of m_axis_data_out_tvalid : signal is "xilinx.com:interface:axis:1.0 m_axis_data_out TVALID";
  attribute x_interface_info of m_axis_data_out_tready : signal is "xilinx.com:interface:axis:1.0 m_axis_data_out TREADY";
  attribute x_interface_info of m_axis_data_out_tlast : signal is "xilinx.com:interface:axis:1.0 m_axis_data_out TLAST";
  attribute x_core_info : string;
  attribute x_core_info of stub : architecture is "acc_image_filter,Vivado 2025.1";
begin
end;
