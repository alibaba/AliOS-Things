#!/usr/bin/python2
import os, sys, shutil, glob
import subprocess, importlib, time
import traceback, pdb
from os import path

workdir = path.join(path.expanduser('~'), 'CodePublish')
srcdir = path.join(workdir, 'source')
tgtdir = path.join(workdir, 'target')
srcurl = 'git@code.aliyun.com:keepwalking.zeng/aos.git'
#tgturl = 'git@github.com:alibaba/AliOS-Things.git'
tgturl = 'git@code.aliyun.com:alios-things/AliOS-Things.git'
rvwurl = 'git@gitlab.alibaba-inc.com:lc122798/aos_github_sync.git'

def prepare_code(source_branch, target_branch):
    global workdir, srcdir, tgtdir
    global srcurl, tgturl
    if path.isdir(workdir):
        shutil.rmtree(workdir)
    os.mkdir(workdir)

    #clone source repository
    cmd = 'git clone {} {}'.format(srcurl, srcdir)
    ret = subprocess.call(cmd, shell=True)
    if ret != 0:
        print "error: clone source repository failed"
        return 1

    #clone target repository
    cmd = 'git clone {} {}'.format(tgturl, tgtdir)
    ret = subprocess.call(cmd, shell=True)
    if ret != 0:
        print "error: clone target repository failed"
        return 1

    #checkout to desired branch in source repository
    os.chdir(srcdir)
    cmd = 'git checkout -b {0} --track origin/{0}'.format(source_branch)
    ret = 0
    if source_branch != 'master':
        ret = subprocess.call(cmd, shell=True)
    if ret != 0:
        print "error: branch {} does not exist in source repository".format(source_branch)
        return 1
    scons_enable_file = path.join(srcdir, 'build', 'scons_enabled.py')
    if path.exists(scons_enable_file):
        os.remove(scons_enable_file)

    #checkout to desired branch in target repository
    os.chdir(tgtdir)
    cmd = 'git branch -a | grep "{}"'.format(target_branch)
    exist = subprocess.call(cmd, shell=True)
    if exist == 0:
        cmd = 'git checkout -b update_{0} --track origin/{0}'.format(target_branch)
    else:
        cmd = 'git checkout -b update_{0} --track origin/master'.format(target_branch)
    ret = subprocess.call(cmd, shell=True)
    if ret != 0:
        print "error: checkout to branch update_{} failed".format(target_branch)
        return 1

    #clean up target repository
    items = os.listdir(tgtdir)
    for item in items:
        if item == '.git':
            continue
        if path.isfile(item):
            os.remove(item)
            continue
        if path.isdir(item):
            shutil.rmtree(item)
            continue
    return 0

def file_operate(dir, fileopts):
    global srcdir, tgtdir
    for item in fileopts:
        try:
            opt = item[0]
            src = item[1]
            if len(item) > 2:
                tgt = item[2]
                if tgt.endswith('/'): tgt = tgt[:-1]
            if opt not in ['copy', 'delete']:
                raise Exceptions("invalid operation '{}'".format(opt))
        except:
            traceback.print_exc()
            print "error: invalid file operation '{}' in {}/{}/sync.py".format(item, srcdir, dir)
            return 1
        if opt == 'copy':
            source = path.join(srcdir, dir, src)
            target = path.join(tgtdir, dir, tgt)
            if '*' in src:
                sources = glob.glob(source)
                if path.isdir(target) == False:
                    os.makedirs(target, 0775)
                for src in sources:
                    if path.basename(src) == 'sync.py' or src.endswith('.pyc'):
                        continue
                    if path.isfile(src):
                        shutil.copy2(src, target)
                    elif path.isdir(src):
                        shutil.copytree(src, path.join(target, path.basename(src)), True)
                    elif path.islink(path.join(srcdir, dir, f)):
                        os.symlink(os.readlink(src), path.join(target, path.basename(src)))
            elif path.isfile(source) or path.islink(source):
                if path.isdir(path.dirname(target)) == False:
                    os.makedirs(path.dirname(target), 0775)
                if path.isfile(source):
                    shutil.copy2(source, target)
                else:
                    os.symlink(os.readlink(source), target)
            elif path.isdir(path.join(srcdir, dir, src)):
                shutil.copytree(source, target, True)
            continue
        elif opt == 'delete':
            source = path.join(tgtdir, dir, src)
            if path.exists(source) == False:
                print "warning: tring to delete a none exist path '{}'".format(source)
                continue
            if path.isfile(source) or path.islink(source):
                os.remove(source)
            elif path.isdir(source):
                shutil.rmtree(source)
            continue
    return 0

def make_library(dir, commands):
    global srcdir, tgtdir
    orgdir = os.getcwd()
    os.chdir(srcdir)
    flog = open('makelib_build.txt', 'wt')
    for item in commands:
        mkcmd = item[0]
        system = item[1]
        tgtpath = path.join(tgtdir, dir, item[2])
        script = None
        if len(item) > 3:
            script = path.join(srcdir, dir, item[3])
        target = None
        for arg in mkcmd.split(' '):
            if '@' not in arg:
                continue
            target = arg
            break
        if target == None:
            print "error: make command '{}' does not inclue a target"
            return 1
        print "running: {}".format(mkcmd)
        subprocess.call('aos make clean', stdout=flog, stderr=flog, shell=True)
        subprocess.call(mkcmd, stdout=flog, stderr=flog, shell=True)
        libpath = path.join('out', target, 'libraries', path.basename(tgtpath).replace('.a', '.stripped.a'))
        if script != None:
            print "running script: {}".format(script)
            os.chdir(path.dirname(libpath))
            ret = subprocess.call('bash {}'.format(script), shell=True)
            if ret != 0:
                print "error: run script {} failed".format(script)
                return 1
            os.chdir(srcdir)
        if path.isfile(libpath) == False:
            print "error: generate {} failed".format(libpath)
            return 1
        if path.isdir(path.dirname(tgtpath)) == False:
            os.makedirs(path.dirname(tgtpath))
        shutil.copy2(libpath, tgtpath)
    flog.close()
    os.remove('makelib_build.txt')
    return 0

def synchronize(dir):
    global srcdir, tgtdir
    sync = path.join(srcdir, dir, 'sync.py')
    if path.isfile(sync) == True:
        print "processing {}".format(sync)
        sys.path.append(path.join(srcdir, dir))
        try:
            sync_info = importlib.import_module('sync')
        except:
            traceback.print_exc()
            print "error: parse sync config file {}/sync.py failed".format(dir)
            return 1
        if hasattr(sync_info, 'sync') == False:
            print "error: invalid sync config file {}/sync.py: 'sync' not found".format(dir)
            return 1
        if sync_info.sync == False:
            sys.modules.pop('sync')
            sys.path.remove(path.join(srcdir, dir))
            return 0
        os.mkdir(path.join(tgtdir, dir))
        if hasattr(sync_info, 'fileopts'):
            ret = file_operate(dir, sync_info.fileopts)
            if ret != 0: return ret
        if hasattr(sync_info, 'makelib'):
            ret = make_library(dir, sync_info.makelib)
            if ret != 0: return ret
        sys.modules.pop('sync')
        sys.path.remove(path.join(srcdir, dir))
        return 0
    else:
        items = os.listdir(path.join(srcdir, dir))
        if path.isdir(path.join(tgtdir, dir)) == False:
            os.mkdir(path.join(tgtdir, dir))
        for item in items:
            src = path.join(srcdir, dir, item)
            tgt = path.join(tgtdir, dir, item)
            if item == '.git':
                continue
            if path.isfile(src):
                shutil.copy2(src, tgt)
            elif path.islink(src):
                os.symlink(os.readlink(src), tgt)
            elif path.isdir(src):
                ret = synchronize(path.join(dir, item))
                if ret != 0: return ret
    return 0

def publish(target_branch, version):
    global tgtdir, rvwurl
    os.chdir(tgtdir)
    change_version_cmd = 'grep -rl "AOS-R-[0-9]\.[0-9]\.[0-9]" | xargs sed -i "s|AOS-R-[0-9].[0-9].[0-9]|AOS-R-{}|g"'.format(version)
    subprocess.call(change_version_cmd, shell=True)
    subprocess.call('git add -A', shell=True)
    commit_message = '"code synchronization at {}"'.format(time.strftime("%Y-%m-%d@%H:%M", time.localtime()))
    subprocess.call('git commit -q -m {}'.format(commit_message), shell=True)
    subprocess.call('git remote add gitlab {}'.format(rvwurl), shell=True)
    subprocess.call('git push -f gitlab update_{}'.format(target_branch), shell=True)
    return 0


def main(source_branch, target_branch, version):
    ret = prepare_code(source_branch, target_branch)
    if ret != 0: return ret

    ret = synchronize('')
    if ret != 0: return ret

    ret = publish(target_branch, version)
    if ret != 0: return ret

    return 0

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print "usage: python {} source_branch target_branch version".format(sys.argv[0])
        sys.exit(1)
    source_branch = sys.argv[1]
    target_branch = sys.argv[2]
    version = sys.argv[3]
    sys.exit(main(source_branch, target_branch, version))
