# 2026-07-23T00:04:21.278031400
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_test_ploca")

platform = client.get_component(name="sw_test_platform")
status = platform.build()

comp = client.get_component(name="sw_app_ploca")
comp.build()

vitis.dispose()

