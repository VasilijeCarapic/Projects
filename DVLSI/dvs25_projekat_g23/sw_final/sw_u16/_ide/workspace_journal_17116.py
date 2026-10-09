# 2026-07-28T08:55:04.031821300
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_u16")

platform = client.get_component(name="sw_test_platform")
status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_wrapper_1.xsa")

status = platform.build()

comp = client.get_component(name="sw_app_ploca")
comp.build()

comp.build()

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_last_fixed_wrapper.xsa")

status = platform.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_stari_wrapper.xsa")

status = platform.build()

comp.build()

comp.build()

vitis.dispose()

vitis.dispose()

