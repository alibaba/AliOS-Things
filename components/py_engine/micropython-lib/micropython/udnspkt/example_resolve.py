import uio
import usocket

import udnspkt


s = usocket.socket(usocket.AF_INET, usocket.SOCK_DGRAM)
dns_addr = usocket.getaddrinfo("127.0.0.1", 53)[0][-1]


def resolve(domain, is_ipv6):
    buf = uio.BytesIO(48)
    udnspkt.make_req(buf, "google.com", is_ipv6)
    v = buf.getvalue()
    print("query: ", v)
    s.sendto(v, dns_addr)

    resp = s.recv(1024)
    print("resp:", resp)
    buf = uio.BytesIO(resp)

    addr = udnspkt.parse_resp(buf, is_ipv6)
    print("bin addr:", addr)
    print("addr:", usocket.inet_ntop(usocket.AF_INET6 if is_ipv6 else usocket.AF_INET, addr))


resolve("google.com", False)
print()
resolve("google.com", True)
