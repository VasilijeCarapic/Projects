# 2026-07-27T15:21:38.479067500
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_u16")

comp = client.get_component(name="sw_app_ploca")
comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

platform = client.get_component(name="sw_test_platform")
status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_block_design_wrapper.xsa")

status = platform.build()

comp.build()

vitis.dispose()

