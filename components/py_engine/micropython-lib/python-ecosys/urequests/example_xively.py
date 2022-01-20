try:
    import urequests as requests
except ImportError:
    import requests

r = requests.get("http://api.xively.com/")
print(r)
print(r.content)
print(r.text)
print(r.content)
print(r.json())

# It's mandatory to close response objects as soon as you finished
# working with them. On MicroPython platforms without full-fledged
# OS, not doing so may lead to resource leaks and malfunction.
r.close()
