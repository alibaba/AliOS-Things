import os, sys
import re

def check_url(url):
    """ Check if the url available """
    ret = 0
    code = 404

    try:
        from urllib2 import urlopen
    except:
        from urllib.request import urlopen

    try:
        resp = urlopen(url)
        code = resp.getcode()
    except:
        pass

    if code != 200:
        ret = 1

    return ret

def download_tools(url, dest_dir):
    """ Download tools to dest_dir """
    ret = 0

    print ("kconfig tools missing, start download ...")
    print ("%s -> %s" % (url, dest_dir))
    cmd = "git clone %s %s" % (url, dest_dir)
    ret = os.system(cmd)

    return ret

def main():
    if len(sys.argv) < 3:
        print ("Usage: %s <url> <dest_dir>" % sys.argv[0])
        return 1

    url = sys.argv[1]
    dest_dir = sys.argv[2]

    if check_url(url):
        print ("\nCan't reach url: %s" % url)
        print ("Please check your network and download it manually:\n")
        print ("  $ git clone %s %s\n" % (url, dest_dir))
        return 1

    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)

    return download_tools(url, dest_dir)

if __name__ == "__main__":
    ret = main()
    sys.exit(ret)
