-- (c) Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- (c) Copyright 2022-2026 Advanced Micro Devices, Inc. All rights reserved.
-- 
-- This file contains confidential and proprietary information
-- of AMD and is protected under U.S. and international copyright
-- and other intellectual property laws.
-- 
-- DISCLAIMER
-- This disclaimer is not a license and does not grant any
-- rights to the materials distributed herewith. Except as
-- otherwise provided in a valid license issued to you by
-- AMD, and to the maximum extent permitted by applicable
-- law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
-- WITH ALL FAULTS, AND AMD HEREBY DISCLAIMS ALL WARRANTIES
-- AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
-- BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
-- INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
-- (2) AMD shall not be liable (whether in contract or tort,
-- including negligence, or under any other theory of
-- liability) for any loss or damage of any kind or nature
-- related to, arising under or in connection with these
-- materials, including for any direct, or any indirect,
-- special, incidental, or consequential loss or damage
-- (including loss of data, profits, goodwill, or any type of
-- loss or damage suffered as a result of any action brought
-- by a third party) even if such damage or loss was
-- reasonably foreseeable or AMD had been advised of the
-- possibility of the same.
-- 
-- CRITICAL APPLICATIONS
-- AMD products are not designed or intended to be fail-
-- safe, or for use in any application requiring fail-safe
-- performance, such as life-support or safety devices or
-- systems, Class III medical devices, nuclear facilities,
-- applications related to the deployment of airbags, or any
-- other applications that could lead to death, personal
-- injury, or severe property or environmental damage
-- (individually and collectively, "Critical
-- Applications"). Customer assumes the sole risk and
-- liability of any use of AMD products in Critical
-- Applications, subject only to applicable laws and
-- regulations governing limitations on product liability.
-- 
-- THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
-- PART OF THIS FILE AT ALL TIMES.
-- 
-- DO NOT MODIFY THIS FILE.

-- IP VLNV: xilinx.com:module_ref:acc_image_filter:1.0
-- IP Revision: 1

LIBRARY ieee;
USE ieee.std_logic_1164.ALL;
USE ieee.numeric_std.ALL;

ENTITY axi_block_design_acc_image_filter_0_1 IS
  PORT (
    reset : IN STD_LOGIC;
    clk : IN STD_LOGIC;
    s_axi_ctrl_awaddr : IN STD_LOGIC_VECTOR(9 DOWNTO 0);
    s_axi_ctrl_awprot : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    s_axi_ctrl_awvalid : IN STD_LOGIC;
    s_axi_ctrl_awready : OUT STD_LOGIC;
    s_axi_ctrl_wdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
    s_axi_ctrl_wstrb : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
    s_axi_ctrl_wvalid : IN STD_LOGIC;
    s_axi_ctrl_wready : OUT STD_LOGIC;
    s_axi_ctrl_bresp : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
    s_axi_ctrl_bvalid : OUT STD_LOGIC;
    s_axi_ctrl_bready : IN STD_LOGIC;
    s_axi_ctrl_araddr : IN STD_LOGIC_VECTOR(9 DOWNTO 0);
    s_axi_ctrl_arprot : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
    s_axi_ctrl_arvalid : IN STD_LOGIC;
    s_axi_ctrl_arready : OUT STD_LOGIC;
    s_axi_ctrl_rdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
    s_axi_ctrl_rvalid : OUT STD_LOGIC;
    s_axi_ctrl_rready : IN STD_LOGIC;
    s_axi_ctrl_rresp : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
    s_axis_data_in_tdata : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
    s_axis_data_in_tvalid : IN STD_LOGIC;
    s_axis_data_in_tready : OUT STD_LOGIC;
    s_axis_data_in_tlast : IN STD_LOGIC;
    m_axis_data_out_tdata : OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
    m_axis_data_out_tvalid : OUT STD_LOGIC;
    m_axis_data_out_tready : IN STD_LOGIC;
    m_axis_data_out_tlast : OUT STD_LOGIC
  );
END axi_block_design_acc_image_filter_0_1;

ARCHITECTURE axi_block_design_acc_image_filter_0_1_arch OF axi_block_design_acc_image_filter_0_1 IS
  ATTRIBUTE DowngradeIPIdentifiedWarnings : STRING;
  ATTRIBUTE DowngradeIPIdentifiedWarnings OF axi_block_design_acc_image_filter_0_1_arch: ARCHITECTURE IS "yes";
  COMPONENT acc_image_filter IS
    GENERIC (
      C_S_AXI_DATA_WIDTH : INTEGER;
      C_S_AXI_ADDR_WIDTH : INTEGER
    );
    PORT (
      reset : IN STD_LOGIC;
      clk : IN STD_LOGIC;
      s_axi_ctrl_awaddr : IN STD_LOGIC_VECTOR(9 DOWNTO 0);
      s_axi_ctrl_awprot : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
      s_axi_ctrl_awvalid : IN STD_LOGIC;
      s_axi_ctrl_awready : OUT STD_LOGIC;
      s_axi_ctrl_wdata : IN STD_LOGIC_VECTOR(31 DOWNTO 0);
      s_axi_ctrl_wstrb : IN STD_LOGIC_VECTOR(3 DOWNTO 0);
      s_axi_ctrl_wvalid : IN STD_LOGIC;
      s_axi_ctrl_wready : OUT STD_LOGIC;
      s_axi_ctrl_bresp : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
      s_axi_ctrl_bvalid : OUT STD_LOGIC;
      s_axi_ctrl_bready : IN STD_LOGIC;
      s_axi_ctrl_araddr : IN STD_LOGIC_VECTOR(9 DOWNTO 0);
      s_axi_ctrl_arprot : IN STD_LOGIC_VECTOR(2 DOWNTO 0);
      s_axi_ctrl_arvalid : IN STD_LOGIC;
      s_axi_ctrl_arready : OUT STD_LOGIC;
      s_axi_ctrl_rdata : OUT STD_LOGIC_VECTOR(31 DOWNTO 0);
      s_axi_ctrl_rvalid : OUT STD_LOGIC;
      s_axi_ctrl_rready : IN STD_LOGIC;
      s_axi_ctrl_rresp : OUT STD_LOGIC_VECTOR(1 DOWNTO 0);
      s_axis_data_in_tdata : IN STD_LOGIC_VECTOR(7 DOWNTO 0);
      s_axis_data_in_tvalid : IN STD_LOGIC;
      s_axis_data_in_tready : OUT STD_LOGIC;
      s_axis_data_in_tlast : IN STD_LOGIC;
      m_axis_data_out_tdata : OUT STD_LOGIC_VECTOR(15 DOWNTO 0);
      m_axis_data_out_tvalid : OUT STD_LOGIC;
      m_axis_data_out_tready : IN STD_LOGIC;
      m_axis_data_out_tlast : OUT STD_LOGIC
    );
  END COMPONENT acc_image_filter;
  ATTRIBUTE X_CORE_INFO : STRING;
  ATTRIBUTE X_CORE_INFO OF axi_block_design_acc_image_filter_0_1_arch: ARCHITECTURE IS "acc_image_filter,Vivado 2025.1";
  ATTRIBUTE CHECK_LICENSE_TYPE : STRING;
  ATTRIBUTE CHECK_LICENSE_TYPE OF axi_block_design_acc_image_filter_0_1_arch : ARCHITECTURE IS "axi_block_design_acc_image_filter_0_1,acc_image_filter,{}";
  ATTRIBUTE CORE_GENERATION_INFO : STRING;
  ATTRIBUTE CORE_GENERATION_INFO OF axi_block_design_acc_image_filter_0_1_arch: ARCHITECTURE IS "axi_block_design_acc_image_filter_0_1,acc_image_filter,{x_ipProduct=Vivado 2025.1,x_ipVendor=xilinx.com,x_ipLibrary=module_ref,x_ipName=acc_image_filter,x_ipVersion=1.0,x_ipCoreRevision=1,x_ipLanguage=VHDL,x_ipSimLanguage=MIXED,C_S_AXI_DATA_WIDTH=32,C_S_AXI_ADDR_WIDTH=10}";
  ATTRIBUTE IP_DEFINITION_SOURCE : STRING;
  ATTRIBUTE IP_DEFINITION_SOURCE OF axi_block_design_acc_image_filter_0_1_arch: ARCHITECTURE IS "module_ref";
  ATTRIBUTE X_INTERFACE_INFO : STRING;
  ATTRIBUTE X_INTERFACE_MODE : STRING;
  ATTRIBUTE X_INTERFACE_PARAMETER : STRING;
  ATTRIBUTE X_INTERFACE_INFO OF clk: SIGNAL IS "xilinx.com:signal:clock:1.0 clk CLK";
  ATTRIBUTE X_INTERFACE_MODE OF clk: SIGNAL IS "slave clk";
  ATTRIBUTE X_INTERFACE_PARAMETER OF clk: SIGNAL IS "XIL_INTERFACENAME clk, ASSOCIATED_BUSIF m_axis_data_out:s_axis_data_in:s_axi_ctrl, ASSOCIATED_RESET reset, FREQ_HZ 1e+08, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, INSERT_VIP 0";
  ATTRIBUTE X_INTERFACE_INFO OF m_axis_data_out_tdata: SIGNAL IS "xilinx.com:interface:axis:1.0 m_axis_data_out TDATA";
  ATTRIBUTE X_INTERFACE_MODE OF m_axis_data_out_tdata: SIGNAL IS "master m_axis_data_out";
  ATTRIBUTE X_INTERFACE_PARAMETER OF m_axis_data_out_tdata: SIGNAL IS "XIL_INTERFACENAME m_axis_data_out, TDATA_NUM_BYTES 2, TDEST_WIDTH 0, TID_WIDTH 0, TUSER_WIDTH 0, HAS_TREADY 1, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 1, FREQ_HZ 1e+08, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, LAYERED_METADATA undef, INSERT_VIP 0";
  ATTRIBUTE X_INTERFACE_INFO OF m_axis_data_out_tlast: SIGNAL IS "xilinx.com:interface:axis:1.0 m_axis_data_out TLAST";
  ATTRIBUTE X_INTERFACE_INFO OF m_axis_data_out_tready: SIGNAL IS "xilinx.com:interface:axis:1.0 m_axis_data_out TREADY";
  ATTRIBUTE X_INTERFACE_INFO OF m_axis_data_out_tvalid: SIGNAL IS "xilinx.com:interface:axis:1.0 m_axis_data_out TVALID";
  ATTRIBUTE X_INTERFACE_INFO OF reset: SIGNAL IS "xilinx.com:signal:reset:1.0 reset RST";
  ATTRIBUTE X_INTERFACE_MODE OF reset: SIGNAL IS "slave reset";
  ATTRIBUTE X_INTERFACE_PARAMETER OF reset: SIGNAL IS "XIL_INTERFACENAME reset, POLARITY ACTIVE_HIGH, INSERT_VIP 0";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_araddr: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARADDR";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_arprot: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARPROT";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_arready: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_arvalid: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARVALID";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_awaddr: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWADDR";
  ATTRIBUTE X_INTERFACE_MODE OF s_axi_ctrl_awaddr: SIGNAL IS "slave s_axi_ctrl";
  ATTRIBUTE X_INTERFACE_PARAMETER OF s_axi_ctrl_awaddr: SIGNAL IS "XIL_INTERFACENAME s_axi_ctrl, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 1e+08, ID_WIDTH 0, ADDR_WIDTH 10, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, NUM_READ" & 
"_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_awprot: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWPROT";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_awready: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_awvalid: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWVALID";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_bready: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl BREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_bresp: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl BRESP";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_bvalid: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl BVALID";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_rdata: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl RDATA";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_rready: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl RREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_rresp: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl RRESP";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_rvalid: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl RVALID";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_wdata: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl WDATA";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_wready: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl WREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_wstrb: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl WSTRB";
  ATTRIBUTE X_INTERFACE_INFO OF s_axi_ctrl_wvalid: SIGNAL IS "xilinx.com:interface:aximm:1.0 s_axi_ctrl WVALID";
  ATTRIBUTE X_INTERFACE_INFO OF s_axis_data_in_tdata: SIGNAL IS "xilinx.com:interface:axis:1.0 s_axis_data_in TDATA";
  ATTRIBUTE X_INTERFACE_MODE OF s_axis_data_in_tdata: SIGNAL IS "slave s_axis_data_in";
  ATTRIBUTE X_INTERFACE_PARAMETER OF s_axis_data_in_tdata: SIGNAL IS "XIL_INTERFACENAME s_axis_data_in, TDATA_NUM_BYTES 1, TDEST_WIDTH 0, TID_WIDTH 0, TUSER_WIDTH 0, HAS_TREADY 1, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 1, FREQ_HZ 1e+08, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, LAYERED_METADATA undef, INSERT_VIP 0";
  ATTRIBUTE X_INTERFACE_INFO OF s_axis_data_in_tlast: SIGNAL IS "xilinx.com:interface:axis:1.0 s_axis_data_in TLAST";
  ATTRIBUTE X_INTERFACE_INFO OF s_axis_data_in_tready: SIGNAL IS "xilinx.com:interface:axis:1.0 s_axis_data_in TREADY";
  ATTRIBUTE X_INTERFACE_INFO OF s_axis_data_in_tvalid: SIGNAL IS "xilinx.com:interface:axis:1.0 s_axis_data_in TVALID";
BEGIN
  U0 : acc_image_filter
    GENERIC MAP (
      C_S_AXI_DATA_WIDTH => 32,
      C_S_AXI_ADDR_WIDTH => 10
    )
    PORT MAP (
      reset => reset,
      clk => clk,
      s_axi_ctrl_awaddr => s_axi_ctrl_awaddr,
      s_axi_ctrl_awprot => s_axi_ctrl_awprot,
      s_axi_ctrl_awvalid => s_axi_ctrl_awvalid,
      s_axi_ctrl_awready => s_axi_ctrl_awready,
      s_axi_ctrl_wdata => s_axi_ctrl_wdata,
      s_axi_ctrl_wstrb => s_axi_ctrl_wstrb,
      s_axi_ctrl_wvalid => s_axi_ctrl_wvalid,
      s_axi_ctrl_wready => s_axi_ctrl_wready,
      s_axi_ctrl_bresp => s_axi_ctrl_bresp,
      s_axi_ctrl_bvalid => s_axi_ctrl_bvalid,
      s_axi_ctrl_bready => s_axi_ctrl_bready,
      s_axi_ctrl_araddr => s_axi_ctrl_araddr,
      s_axi_ctrl_arprot => s_axi_ctrl_arprot,
      s_axi_ctrl_arvalid => s_axi_ctrl_arvalid,
      s_axi_ctrl_arready => s_axi_ctrl_arready,
      s_axi_ctrl_rdata => s_axi_ctrl_rdata,
      s_axi_ctrl_rvalid => s_axi_ctrl_rvalid,
      s_axi_ctrl_rready => s_axi_ctrl_rready,
      s_axi_ctrl_rresp => s_axi_ctrl_rresp,
      s_axis_data_in_tdata => s_axis_data_in_tdata,
      s_axis_data_in_tvalid => s_axis_data_in_tvalid,
      s_axis_data_in_tready => s_axis_data_in_tready,
      s_axis_data_in_tlast => s_axis_data_in_tlast,
      m_axis_data_out_tdata => m_axis_data_out_tdata,
      m_axis_data_out_tvalid => m_axis_data_out_tvalid,
      m_axis_data_out_tready => m_axis_data_out_tready,
      m_axis_data_out_tlast => m_axis_data_out_tlast
    );
END axi_block_design_acc_image_filter_0_1_arch;
