// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2025 Advanced Micro Devices, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2025.1 (win64) Build 6140274 Thu May 22 00:12:29 MDT 2025
// Date        : Wed Jul 29 20:17:55 2026
// Host        : windows_rip running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               e:/dvs_project_u16/dvs_bypass_pack/dvs_bypass_pack.gen/sources_1/bd/axi_block_design/ip/axi_block_design_acc_image_filter_0_1/axi_block_design_acc_image_filter_0_1_stub.v
// Design      : axi_block_design_acc_image_filter_0_1
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7z020clg400-1
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* CHECK_LICENSE_TYPE = "axi_block_design_acc_image_filter_0_1,acc_image_filter,{}" *) (* core_generation_info = "axi_block_design_acc_image_filter_0_1,acc_image_filter,{x_ipProduct=Vivado 2025.1,x_ipVendor=xilinx.com,x_ipLibrary=module_ref,x_ipName=acc_image_filter,x_ipVersion=1.0,x_ipCoreRevision=1,x_ipLanguage=VHDL,x_ipSimLanguage=MIXED,C_S_AXI_DATA_WIDTH=32,C_S_AXI_ADDR_WIDTH=10}" *) (* downgradeipidentifiedwarnings = "yes" *) 
(* ip_definition_source = "module_ref" *) (* x_core_info = "acc_image_filter,Vivado 2025.1" *) 
module axi_block_design_acc_image_filter_0_1(reset, clk, s_axi_ctrl_awaddr, 
  s_axi_ctrl_awprot, s_axi_ctrl_awvalid, s_axi_ctrl_awready, s_axi_ctrl_wdata, 
  s_axi_ctrl_wstrb, s_axi_ctrl_wvalid, s_axi_ctrl_wready, s_axi_ctrl_bresp, 
  s_axi_ctrl_bvalid, s_axi_ctrl_bready, s_axi_ctrl_araddr, s_axi_ctrl_arprot, 
  s_axi_ctrl_arvalid, s_axi_ctrl_arready, s_axi_ctrl_rdata, s_axi_ctrl_rvalid, 
  s_axi_ctrl_rready, s_axi_ctrl_rresp, s_axis_data_in_tdata, s_axis_data_in_tvalid, 
  s_axis_data_in_tready, s_axis_data_in_tlast, m_axis_data_out_tdata, 
  m_axis_data_out_tvalid, m_axis_data_out_tready, m_axis_data_out_tlast)
/* synthesis syn_black_box black_box_pad_pin="reset,s_axi_ctrl_awaddr[9:0],s_axi_ctrl_awprot[2:0],s_axi_ctrl_awvalid,s_axi_ctrl_awready,s_axi_ctrl_wdata[31:0],s_axi_ctrl_wstrb[3:0],s_axi_ctrl_wvalid,s_axi_ctrl_wready,s_axi_ctrl_bresp[1:0],s_axi_ctrl_bvalid,s_axi_ctrl_bready,s_axi_ctrl_araddr[9:0],s_axi_ctrl_arprot[2:0],s_axi_ctrl_arvalid,s_axi_ctrl_arready,s_axi_ctrl_rdata[31:0],s_axi_ctrl_rvalid,s_axi_ctrl_rready,s_axi_ctrl_rresp[1:0],s_axis_data_in_tdata[7:0],s_axis_data_in_tvalid,s_axis_data_in_tready,s_axis_data_in_tlast,m_axis_data_out_tdata[15:0],m_axis_data_out_tvalid,m_axis_data_out_tready,m_axis_data_out_tlast" */
/* synthesis syn_force_seq_prim="clk" */;
  (* max_fanout = "150" *) (* x_interface_info = "xilinx.com:signal:reset:1.0 reset RST" *) (* x_interface_mode = "slave reset" *) 
  (* x_interface_parameter = "XIL_INTERFACENAME reset, POLARITY ACTIVE_HIGH, INSERT_VIP 0" *) input reset;
  (* x_interface_info = "xilinx.com:signal:clock:1.0 clk CLK" *) (* x_interface_mode = "slave clk" *) (* x_interface_parameter = "XIL_INTERFACENAME clk, ASSOCIATED_BUSIF m_axis_data_out:s_axis_data_in:s_axi_ctrl, ASSOCIATED_RESET reset, FREQ_HZ 1e+08, FREQ_TOLERANCE_HZ 0, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, INSERT_VIP 0" *) input clk /* synthesis syn_isclock = 1 */;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWADDR" *) (* x_interface_mode = "slave s_axi_ctrl" *) (* x_interface_parameter = "XIL_INTERFACENAME s_axi_ctrl, DATA_WIDTH 32, PROTOCOL AXI4LITE, FREQ_HZ 1e+08, ID_WIDTH 0, ADDR_WIDTH 10, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE READ_WRITE, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 1, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1, MAX_BURST_LENGTH 1, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input [9:0]s_axi_ctrl_awaddr;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWPROT" *) input [2:0]s_axi_ctrl_awprot;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWVALID" *) input s_axi_ctrl_awvalid;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl AWREADY" *) output s_axi_ctrl_awready;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl WDATA" *) input [31:0]s_axi_ctrl_wdata;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl WSTRB" *) input [3:0]s_axi_ctrl_wstrb;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl WVALID" *) input s_axi_ctrl_wvalid;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl WREADY" *) output s_axi_ctrl_wready;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl BRESP" *) output [1:0]s_axi_ctrl_bresp;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl BVALID" *) output s_axi_ctrl_bvalid;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl BREADY" *) input s_axi_ctrl_bready;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARADDR" *) input [9:0]s_axi_ctrl_araddr;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARPROT" *) input [2:0]s_axi_ctrl_arprot;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARVALID" *) input s_axi_ctrl_arvalid;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl ARREADY" *) output s_axi_ctrl_arready;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl RDATA" *) output [31:0]s_axi_ctrl_rdata;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl RVALID" *) output s_axi_ctrl_rvalid;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl RREADY" *) input s_axi_ctrl_rready;
  (* x_interface_info = "xilinx.com:interface:aximm:1.0 s_axi_ctrl RRESP" *) output [1:0]s_axi_ctrl_rresp;
  (* x_interface_info = "xilinx.com:interface:axis:1.0 s_axis_data_in TDATA" *) (* x_interface_mode = "slave s_axis_data_in" *) (* x_interface_parameter = "XIL_INTERFACENAME s_axis_data_in, TDATA_NUM_BYTES 1, TDEST_WIDTH 0, TID_WIDTH 0, TUSER_WIDTH 0, HAS_TREADY 1, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 1, FREQ_HZ 1e+08, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, LAYERED_METADATA undef, INSERT_VIP 0" *) input [7:0]s_axis_data_in_tdata;
  (* x_interface_info = "xilinx.com:interface:axis:1.0 s_axis_data_in TVALID" *) input s_axis_data_in_tvalid;
  (* x_interface_info = "xilinx.com:interface:axis:1.0 s_axis_data_in TREADY" *) output s_axis_data_in_tready;
  (* x_interface_info = "xilinx.com:interface:axis:1.0 s_axis_data_in TLAST" *) input s_axis_data_in_tlast;
  (* x_interface_info = "xilinx.com:interface:axis:1.0 m_axis_data_out TDATA" *) (* x_interface_mode = "master m_axis_data_out" *) (* x_interface_parameter = "XIL_INTERFACENAME m_axis_data_out, TDATA_NUM_BYTES 2, TDEST_WIDTH 0, TID_WIDTH 0, TUSER_WIDTH 0, HAS_TREADY 1, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 1, FREQ_HZ 1e+08, PHASE 0.0, CLK_DOMAIN axi_block_design_processing_system7_0_1_FCLK_CLK0, LAYERED_METADATA undef, INSERT_VIP 0" *) output [15:0]m_axis_data_out_tdata;
  (* x_interface_info = "xilinx.com:interface:axis:1.0 m_axis_data_out TVALID" *) output m_axis_data_out_tvalid;
  (* x_interface_info = "xilinx.com:interface:axis:1.0 m_axis_data_out TREADY" *) input m_axis_data_out_tready;
  (* x_interface_info = "xilinx.com:interface:axis:1.0 m_axis_data_out TLAST" *) output m_axis_data_out_tlast;
endmodule
