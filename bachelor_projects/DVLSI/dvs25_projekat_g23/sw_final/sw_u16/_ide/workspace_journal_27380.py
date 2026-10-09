# 2026-07-28T15:41:50.460170700
import vitis

client = vitis.create_client()
client.set_workspace(path="sw_u16")

vitis.dispose()

