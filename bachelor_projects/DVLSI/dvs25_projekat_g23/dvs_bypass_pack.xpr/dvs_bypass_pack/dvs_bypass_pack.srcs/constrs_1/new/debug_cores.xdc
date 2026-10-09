


create_debug_core u_ila_0 ila
set_property ALL_PROBE_SAME_MU true [get_debug_cores u_ila_0]
set_property ALL_PROBE_SAME_MU_CNT 2 [get_debug_cores u_ila_0]
set_property C_ADV_TRIGGER false [get_debug_cores u_ila_0]
set_property C_DATA_DEPTH 1024 [get_debug_cores u_ila_0]
set_property C_EN_STRG_QUAL true [get_debug_cores u_ila_0]
set_property C_INPUT_PIPE_STAGES 0 [get_debug_cores u_ila_0]
set_property C_TRIGIN_EN false [get_debug_cores u_ila_0]
set_property C_TRIGOUT_EN false [get_debug_cores u_ila_0]
set_property port_width 1 [get_debug_ports u_ila_0/clk]
connect_debug_port u_ila_0/clk [get_nets [list axi_block_design_i/processing_system7_0/inst/FCLK_CLK0]]
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe0]
set_property port_width 16 [get_debug_ports u_ila_0/probe0]
connect_debug_port u_ila_0/probe0 [get_nets [list {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[0]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[1]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[2]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[3]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[4]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[5]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[6]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[7]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[8]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[9]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[10]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[11]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[12]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[13]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[14]} {axi_block_design_i/acc_image_filter_0_m_axis_data_out_TDATA[15]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe1]
set_property port_width 8 [get_debug_ports u_ila_0/probe1]
connect_debug_port u_ila_0/probe1 [get_nets [list {axi_block_design_i/acc_image_filter_0/U0/reg_input[0]} {axi_block_design_i/acc_image_filter_0/U0/reg_input[1]} {axi_block_design_i/acc_image_filter_0/U0/reg_input[2]} {axi_block_design_i/acc_image_filter_0/U0/reg_input[3]} {axi_block_design_i/acc_image_filter_0/U0/reg_input[4]} {axi_block_design_i/acc_image_filter_0/U0/reg_input[5]} {axi_block_design_i/acc_image_filter_0/U0/reg_input[6]} {axi_block_design_i/acc_image_filter_0/U0/reg_input[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe2]
set_property port_width 8 [get_debug_ports u_ila_0/probe2]
connect_debug_port u_ila_0/probe2 [get_nets [list {axi_block_design_i/acc_image_filter_0/s_axis_data_in_tdata[0]} {axi_block_design_i/acc_image_filter_0/s_axis_data_in_tdata[1]} {axi_block_design_i/acc_image_filter_0/s_axis_data_in_tdata[2]} {axi_block_design_i/acc_image_filter_0/s_axis_data_in_tdata[3]} {axi_block_design_i/acc_image_filter_0/s_axis_data_in_tdata[4]} {axi_block_design_i/acc_image_filter_0/s_axis_data_in_tdata[5]} {axi_block_design_i/acc_image_filter_0/s_axis_data_in_tdata[6]} {axi_block_design_i/acc_image_filter_0/s_axis_data_in_tdata[7]}]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe3]
set_property port_width 1 [get_debug_ports u_ila_0/probe3]
connect_debug_port u_ila_0/probe3 [get_nets [list axi_block_design_i/acc_image_filter_0_m_axis_data_out_TLAST]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe4]
set_property port_width 1 [get_debug_ports u_ila_0/probe4]
connect_debug_port u_ila_0/probe4 [get_nets [list axi_block_design_i/acc_image_filter_0_m_axis_data_out_TREADY]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe5]
set_property port_width 1 [get_debug_ports u_ila_0/probe5]
connect_debug_port u_ila_0/probe5 [get_nets [list axi_block_design_i/acc_image_filter_0_m_axis_data_out_TVALID]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe6]
set_property port_width 1 [get_debug_ports u_ila_0/probe6]
connect_debug_port u_ila_0/probe6 [get_nets [list axi_block_design_i/acc_image_filter_0/s_axis_data_in_tlast]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe7]
set_property port_width 1 [get_debug_ports u_ila_0/probe7]
connect_debug_port u_ila_0/probe7 [get_nets [list axi_block_design_i/acc_image_filter_0/s_axis_data_in_tready]]
create_debug_port u_ila_0 probe
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0/probe8]
set_property port_width 1 [get_debug_ports u_ila_0/probe8]
connect_debug_port u_ila_0/probe8 [get_nets [list axi_block_design_i/acc_image_filter_0/s_axis_data_in_tvalid]]
set_property C_CLK_INPUT_FREQ_HZ 300000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets u_ila_0_FCLK_CLK0]
