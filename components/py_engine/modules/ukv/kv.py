import uio as io
import ujson as json
import systemAdaptor

def set(key, value):
    kvPath = systemAdaptor.getDataPath() + "kv.json"
    kvList = {}
    try:
        f = open(kvPath, 'r')
        kvList = json.loads(f.read())
        f.close()
    except Exception as e:
        pass

    kvList[key] = value
    try:
        f = open(kvPath, 'w')
        f.write(json.dumps(kvList))
        f.flush()
    except Exception as e:
        print(str(e))
    f.close()
    return 0

def get(key):
    kvPath = systemAdaptor.getDataPath() + "kv.json"
    f = open(kvPath, 'r')
    kvList = json.loads(f.read())
    f.close()
    if key in kvList.keys():
        return kvList[key]
    else:
        return None

def remove(key):
    kvPath = systemAdaptor.getDataPath() + "kv.json"
    kvList = {}
    try:
        f = open(kvPath, 'r')
        kvList = json.loads(f.read())
        f.close()
    except Exception as e:
        print(str(e))
        return

    if key in kvList.keys():
        del kvList[key]
    else:
        return 0

    try:
        f = open(kvPath, 'w')
        f.write(json.dumps(kvList))
        f.flush()
    except Exception as e:
        print(str(e))
    f.close()

    del kvList
    return 0

def list():
    kvPath = systemAdaptor.getDataPath() + "kv.json"
    kvList = {}
    try:
        f = open(kvPath, 'r')
        kvList = json.loads(f.read())
        f.close()
    except Exception as e:
        pass
    return kvList
