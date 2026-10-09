# 2026-07-29T18:23:33.011304900
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_u16")

comp = client.get_component(name="sw_app_ploca")
comp.build()

comp.build()

platform = client.get_component(name="sw_test_platform")
status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_final1_wrapper.xsa")

status = platform.build()

vitis.dispose()

