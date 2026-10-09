# 2026-07-21T01:30:44.215760800
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_app_ploca")

vitis.dispose()

