# 2026-07-29T16:51:11.189019400
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_u16")

platform = client.get_component(name="sw_test_platform")
status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_final_wrapper.xsa")

status = platform.build()

comp = client.get_component(name="sw_app_ploca")
comp.build()

comp.build()

vitis.dispose()

