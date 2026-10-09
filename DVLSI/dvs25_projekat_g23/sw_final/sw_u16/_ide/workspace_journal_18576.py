# 2026-07-22T23:42:45.001602200
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_test_ploca")

platform = client.get_component(name="sw_test_platform")
status = platform.build()

comp = client.get_component(name="sw_app_ploca")
comp.build()

status = platform.build()

comp.build()

status = comp.clean()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

status = platform.build()

comp.build()

vitis.dispose()

