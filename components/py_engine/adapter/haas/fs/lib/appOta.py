import uos as os
import uerrno as errno
import ujson as json
import uzlib
import upip_utarfile as tarfile
import gc
import time
import ussl
import usocket
import kv
import time

file_buf = bytearray(128)


def install_pkg(package_url, install_path):
    gzdict_sz = 16 + 15
    f1 = url_open(package_url)
    if (isinstance(f1, (str, bytes, bytearray)) == True):
        return f1
    try:
        f2 = uzlib.DecompIO(f1, gzdict_sz)
        f3 = tarfile.TarFile(fileobj=f2)
        install_tar(f3, install_path)
    except Exception as e:
        print(e)
        return ("UNTAR_FILE_FAIL")
    finally:
        f1.close()
    del f3
    del f2
    gc.collect()

    #print('install_pkg success')
    return 'SUCCESS'


def download_save_file(file_url, fname):
    global file_buf
    f1 = url_open(file_url)

    if (isinstance(f1, (str, bytes, bytearray)) == True):
        #print(f1)
        return f1

    #print(fname)
    _makedirs(fname)
    with open(fname, "wb") as outf:
        while True:
            sz = f1.readinto(file_buf)
            if not sz:
                break
            outf.write(file_buf, sz)
        outf.close()
    f1.close()
    del f1
    #print('download_save_file success')
    return 'SUCCESS'


def url_open(url):
    proto, _, host, urlpath = url.split('/', 3)
    try:
        port = 443
        if ":" in host:
            host, port = host.split(":")
            port = int(port)
        ai = usocket.getaddrinfo(host, port, 0, usocket.SOCK_STREAM)
    except OSError as e:
        print("Error:", "Unable to resolve %s (no Internet?)" % host, e)
        return 'HOST_RESOLVED_FAIL'
    # print("Address infos:", ai)
    ai = ai[0]
    s = usocket.socket(ai[0], ai[1], ai[2])
    try:
        s.connect(ai[-1])
        if proto == "https:":
            s = ussl.wrap_socket(s, server_hostname=host)
        s.write("GET /%s HTTP/1.0\r\nHost: %s:%s\r\n\r\n" %
                (urlpath, host, port))
        l = s.readline()
        protover, status, msg = l.split(None, 2)
        if status != b"200":
            if status == b"404" or status == b"301" or status == b"403":
                return ("Package not found")
            else:
                print("status is {}".format(status))
            return (status)
        while True:
            l = s.readline()
            if not l:
                return ("Unexpected EOF in HTTP headers")
            if l == b'\r\n':
                break
    except Exception as e:
        s.close()
        print(e)
        return ('SOCKET_ERROR')
    return s


def _makedirs(name, mode=0o777):
    ret = False
    s = ""
    comps = name.rstrip("/").split("/")[:-1]
    if comps[0] == "":
        s = "/"
    for c in comps:
        if s and s[-1] != "/":
            s += "/"
        s += c
        try:
            os.mkdir(s)
            ret = True
        except OSError as e:
            if e.args[0] != errno.EEXIST and e.args[0] != errno.EISDIR:
                print(e)
            ret = False
    return ret


def install_tar(f, prefix):
    for info in f:
        #print(info)
        fname = info.name
        #print("fname is {}".format(fname))
        #try:
        #fname = fname[fname.index("/") + 1:]
        #except ValueError:
        #fname = ""
        #print("fname is {}".format(fname))

        outfname = prefix + fname
        #print(outfname)
        if info.type != tarfile.DIRTYPE:
            print("Extracting " + outfname)
            _makedirs(outfname)
            subf = f.extractfile(info)
            save_file(outfname, subf)


def save_file(fname, subf):
    global file_buf
    with open(fname, "wb") as outf:
        while True:
            sz = subf.readinto(file_buf)
            if not sz:
                break
            outf.write(file_buf, sz)
        outf.close()

if __name__ == "__main__":
    url = kv.get('_amp_pyapp_url')
    if isinstance(url, str):
        install_pkg(url, "/data/pyamp/")
        kv.remove('_amp_pyapp_url')
        reboot()
