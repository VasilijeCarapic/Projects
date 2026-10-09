# 2026-07-21T01:13:14.415692700
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_test_ploca")

vitis.dispose()

platform = client.get_component(name="sw_test_platform")
status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../pokusaj_modularizacije/axi_design_wrapper.xsa")

status = platform.build()

comp = client.get_component(name="sw_app_ploca")
comp.build()

status = platform.build()

comp.build()

vitis.dispose()

