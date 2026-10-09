# 2026-07-27T22:31:35.405915900
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_u16")

platform = client.get_component(name="sw_test_platform")
status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_block_design_wrapper.xsa")

comp = client.get_component(name="sw_app_ploca")
comp.build()

status = platform.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_block_design_wrapper.xsa")

status = platform.build()

comp.build()

comp.build()

