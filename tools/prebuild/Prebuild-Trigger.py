#! /usr/bin/python
# -*- coding: utf8 -*-
# encoding=utf-8

import os, sys, json, time
import urllib2, requests, traceback
import sqlite3 as sql
from os import path

def get(url):
    result = None
    response = None
    try:
        request = urllib2.Request(url)
        response = urllib2.urlopen(request)
        result = response.read()
    except urllib2.URLError as e:
        if hasattr(e, 'code'):
            print "post error,error code %s" % (str(e.code))
            return False, str(e.code)
        elif hasattr(e, 'reason'):
            print "post error,reason %s" % (str(e.reason))
            return False, e.reason
    finally:
        if response:
            response.close()
    #print 'get success, result is %s' % result
    return True, result


'''
1. 查询项目project id：                  GET /projects
2. 查询merge requests 列表：             GET /projects/:id/merge_requests
3. 查询单个merge_request详情：           GET /projects/:id/merge_request/:merge_request_id
4. 修改comments：                        POST /projects/:id/merge_request/:merge_request_id/comments
5. 查询comment，会根据comments来触发 ：  GET /projects/:id/merge_request/:merge_request_id/comments
6. 查询changes：                         GET /projects/:id/merge_request/:merge_request_id/changes
7. 查询commits                           GET /projects/:id/merge_request/:merge_request_id/commits
'''
def get_project_id(project):
    url = 'https://code.aliyun.com/api/v3/projects/?private_token=JzuV6FX2yGGdgybAmzQF'
    ret, result = get(url)
    if ret:
        result = json.loads(result)
        #print result
        for r in result:
            if r['id'] and r['web_url'] == project:
                return r['id']
    return ''

def get_patch_list(project_id,state):
    source_branches   = []
    merge_request_ids = []
    target_branches   = []
    url = 'https://code.aliyun.com/api/v3/projects/{}/merge_requests/?private_token=JzuV6FX2yGGdgybAmzQF&state={}'.format(project_id,state)
    ret, result = get(url)
    if ret:
        result = json.loads(result)
        for r in result:
            print r
            if r['source_branch']:
                source_branches.append(r['source_branch'])
            if r['id']:
                merge_request_ids.append(r['id'])
            if r['target_branch']:
                target_branches.append(r['target_branch'])
    return source_branches,merge_request_ids,target_branches

def get_latest_commit(project_id,merge_request_id):
    url = 'https://code.aliyun.com/api/v3/projects/{}/merge_request/{}/commits/?private_token=JzuV6FX2yGGdgybAmzQF'.format(project_id,merge_request_id)
    ret,result = get(url)
    if ret:
        result = json.loads(result)
        if result and result[0] and result[0]['id']:
            commit = result[0]['id']
            author_email = result[0]['author_email']
            #print commit,author_email
            return commit,author_email
    return '',''

def database_excute_sqlcmd(dbase, sqlcmd):
    with dbase:
        cur = dbase.cursor()
        try:
            cur.execute(sqlcmd)
        except:
            traceback.print_exc()
            ret = None
        else:
            ret = cur.fetchall()
    return ret

def patch_is_building(dbase, patch):
    sqlcmd = "SELECT * From Prebuild where info = '{0}'".format(patch)
    rows = database_excute_sqlcmd(dbase, sqlcmd)
    if rows == None or len(rows) < 1:
        return False
    else:
        return True

def add_build_patch(base, patch):
    sqlcmd = "INSERT INTO Prebuild VALUES('{0}', 'queued')".format(patch)
    ret = database_excute_sqlcmd(dbase, sqlcmd)
    if ret == None:
        return False
    else:
        return True

def trigger_prebuild(merge_request_id, target_branch, commit):
    base_url='http://10.101.107.47:8080/job/AliOS-Things-Build/buildWithParameters?token=Prebuild'
    mrid_param = "MRID={0}".format(str(merge_request_id))
    branch_param = "BRANCH={0}".format(str(target_branch))
    commit_param = "COMMIT={0}".format(str(commit))
    trigger_url = '&'.join([base_url, mrid_param, branch_param, commit_param])
    print "trigger_url:", trigger_url
    ret = requests.get(trigger_url, auth=('licheng', 'SWc-z9R-Yo9-NPH'))
    if ret.status_code == 201:
        return True
    else:
        return False

if __name__ == '__main__':
    dbfile = path.join(path.expanduser('~'), 'tools', 'Prbuild.db')
    dbase = sql.connect(dbfile, check_same_thread = False)
    sqlcmd = 'CREATE TABLE IF NOT EXISTS Prebuild(info TEXT, status TEXT)'
    database_excute_sqlcmd(dbase, sqlcmd)

    project_id = get_project_id('https://code.aliyun.com/keepwalking.zeng/aos')
    print project_id
    if not project_id:
        print "error: can not get project_id"
    while True:
        source_branches,merge_request_ids,target_branches = get_patch_list(project_id,'opened')
        print 'project_id:',project_id
        print 'source_branches:',source_branches
        print 'target_branches:',target_branches
        print 'merge_request_ids:',merge_request_ids
        for i in range(0,len(merge_request_ids)):
            print '=================================================================',i
            commit, author_email = get_latest_commit(project_id, merge_request_ids[i])
            print 'commit:', commit
            print 'author_email:', author_email
            comb = str(commit) + '#' + str(project_id) + '#' +str(merge_request_ids[i]) + '#' + str(source_branches[i]) + '#' + str(target_branches[i])
            if patch_is_building(dbase, comb):
                print 'this patch {} have been built, skip'.format(comb)
                continue
            ret = trigger_prebuild(merge_request_ids[i], target_branches[i], commit)
            if ret == False:
                print "trigger prebuild failed"
            else:
                add_build_patch(dbase, comb)
                print "trigger prebuild succeed"
        time.sleep(300)

