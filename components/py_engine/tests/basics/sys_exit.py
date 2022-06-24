# test sys module's exit function

try:
    import usys as sys
except ImportError:
    import sys

try:
    sys.exit
except AttributeError:
    print("SKIP")
    raise SystemExit

try:
    raise SystemExit
except SystemExit as e:
    print("SystemExit", e.args)

try:
    sys.exit()
except SystemExit as e:
    print("SystemExit", e.args)

try:
    sys.exit(42)
except SystemExit as e:
    print("SystemExit", e.args)
