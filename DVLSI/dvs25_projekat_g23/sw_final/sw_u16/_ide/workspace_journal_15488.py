# 2026-07-19T22:53:34.741873200
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

vitis.dispose()

