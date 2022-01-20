import os.path

for cur_path, dirs, files in os.walk('.'):
    if cur_path == '.':
        for f in files:
            if f.endswith('.h'):
                print f
                fo = open(f, 'w')
                fo.write('#include "_fake_defines.h"\n')
                fo.write('#include "_fake_typedefs.h"\n')
                fo.close()


