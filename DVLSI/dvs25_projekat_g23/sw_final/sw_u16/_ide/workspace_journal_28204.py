# 2026-07-29T09:10:24.289710900
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_u16")

comp = client.get_component(name="sw_app_ploca")
comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

comp.build()

vitis.dispose()

