# 2026-07-23T00:14:07.701345
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_test_ploca")

platform = client.get_component(name="sw_test_platform")
status = platform.build()

comp = client.get_component(name="sw_app_ploca")
comp.build()

status = platform.build()

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../za_hw_test.xpr/pokusaj_modularizacije/axi_design_wrapper.xsa")

status = platform.build()

status = platform.build()

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../za_hw_test.xpr/pokusaj_modularizacije/axi_design_wrapper.xsa")

status = platform.build()

comp.build()

vitis.dispose()

