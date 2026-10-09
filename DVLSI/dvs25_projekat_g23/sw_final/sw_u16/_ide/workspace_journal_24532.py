# 2026-07-24T13:24:04.562501100
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_test_ploca")

platform = client.get_component(name="sw_test_platform")
status = platform.build()

comp = client.get_component(name="sw_app_ploca")
comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../pokusaj_modularizacije/axi_design_wrapper.xsa")

status = platform.build()

comp.build()

proj = client.create_sys_project(name="system_project", platform="$COMPONENT_LOCATION/../sw_test_platform/export/sw_test_platform/sw_test_platform.xpfm", template="empty_accelerated_application" , build_output_type="xsa")

proj = client.get_sys_project(name="system_project")

proj = proj.add_component(name="sw_app_ploca")

proj = client.get_sys_project(name="system_project")

proj.build(comp_name = ["sw_app_ploca"],build_comps = False)

vitis.dispose()

comp = client.get_component(name="sw_app_ploca")
comp.build()

comp.build()

status = client.delete_sys_project(name="system_project")

status = client.delete_sys_project(name="projectName")

platform = client.get_component(name="sw_test_platform")
status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_block_design_wrapper.xsa")

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_block_design_wrapper.xsa")

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_block_design_wrapper.xsa")

comp.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_block_design_wrapper.xsa")

status = platform.build()

status = platform.update_hw(hw_design = "$COMPONENT_LOCATION/../../dvs_bypass_pack/axi_block_design_wrapper.xsa")

comp.build()

vitis.dispose()

