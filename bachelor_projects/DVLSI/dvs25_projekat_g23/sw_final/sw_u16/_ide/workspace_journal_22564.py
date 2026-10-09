# 2026-07-26T23:30:07.346055800
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_u16")

vitis.dispose()

