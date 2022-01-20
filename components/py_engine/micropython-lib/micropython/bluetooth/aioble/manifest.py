import os

_files = (
    "__init__.py",
    "core.py",
    "device.py",
)

options.defaults(peripheral=True, server=True)

if options.central:
    _files += ("central.py",)

if options.client:
    _files += ("client.py",)

if options.peripheral:
    _files += ("peripheral.py",)

if options.server:
    _files += ("server.py",)

if options.l2cap:
    _files += ("l2cap.py",)

if options.security:
    _files += ("security.py",)

freeze(
    ".",
    tuple(os.path.join("aioble", f) for f in _files),
    opt=3,
)
