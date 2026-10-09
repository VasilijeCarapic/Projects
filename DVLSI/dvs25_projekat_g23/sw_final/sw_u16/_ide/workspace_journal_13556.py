# 2026-07-28T23:57:24.410387700
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_u16")

comp = client.get_component(name="sw_app_ploca")
comp.build()

comp.build()

vitis.dispose()

