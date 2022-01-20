from http.client import HTTPConnection


conn = HTTPConnection("localhost")
# conn = HTTPConnection("python.org")
conn.request("GET", "/")
resp = conn.getresponse()
print(resp)
print(resp.read())
