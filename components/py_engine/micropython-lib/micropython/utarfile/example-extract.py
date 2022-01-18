import sys
import os
import shutil
import utarfile

t = utarfile.TarFile(sys.argv[1])
for i in t:
    print(i)
    if i.type == utarfile.DIRTYPE:
        os.makedirs(i.name)
    else:
        f = t.extractfile(i)
        shutil.copyfileobj(f, open(i.name, "wb"))
