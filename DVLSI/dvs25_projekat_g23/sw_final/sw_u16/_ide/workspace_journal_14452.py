# 2026-07-19T22:05:37.337054700
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_test_ploca")

platform = client.create_platform_component(name = "sw_test_platform",hw_design = "$COMPONENT_LOCATION/../../pokusaj_modularizacije/axi_design_wrapper.xsa",os = "standalone",cpu = "ps7_cortexa9_0",domain_name = "standalone_ps7_cortexa9_0",no_boot_bsp = True,compiler = "gcc")

platform = client.get_component(name="sw_test_platform")
status = platform.build()

comp = client.create_app_component(name="sw_app_ploca",platform = "$COMPONENT_LOCATION/../sw_test_platform/export/sw_test_platform/sw_test_platform.xpfm",domain = "standalone_ps7_cortexa9_0")

component = client.get_component(name="sw_app_ploca")

lscript = component.get_ld_script(path="E:\hw_test_deo\pokusaj_modularizacije.xpr\na_ploci_test\sw_test_ploca\sw_app_ploca\src\lscript.ld")

lscript.set_heap_size("0x2000000")

comp = client.get_component(name="sw_app_ploca")
comp.set_app_config(key = "USER_LINK_LIBRARIES", values = ["m"])

status = platform.build()

status = platform.build()

comp = client.get_component(name="sw_app_ploca")
comp.build()

vitis.dispose()

