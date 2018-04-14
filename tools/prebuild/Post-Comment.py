#! /usr/bin/python
# -*- coding: utf8 -*-
# encoding=utf-8

import os, sys, json
import urllib, urllib2

def post(url, params):
    result = None
    response = None
    try:
        postdata = urllib.urlencode(params)
        request = urllib2.Request(url, postdata)
        response = urllib2.urlopen(request)
        result = response.read()
    except urllib2.URLError as e:
        if hasattr(e, 'code'):
            print "%s post error,error code %s" % (str(params), str(e.code))
            return False, str(e.code)
        elif hasattr(e, 'reason'):
            print "%s post error,reason %s" % (str(params), str(e.reason))
            return False, e.reason
    finally:
        if response:
            response.close()
    #print 'post success, result is %s' % result
    return True, result

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
    ret,result = get(url)
    if ret:
        result = json.loads(result)
        #print result
        for r in result:
            if r['id'] and r['web_url'] == project:
                return r['id']
    return ''

def post_comments(project_id,merge_request_id,comments='build pass'):
    url = 'https://code.aliyun.com/api/v3/projects/{}/merge_request/{}/comments'.format(project_id,merge_request_id)
    params = {'note':comments,'private_token':'JzuV6FX2yGGdgybAmzQF'}
    ret,result = post(url,params)
    if ret:
        return 'succeed'
    else:
        return 'failed'

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print "Usage: python {0} merge_request_id comments".format(sys.argv[0])
        sys.exit(1)

    try:
        merge_request_id = int(sys.argv[1])
    except:
        print "error: invalid merge_request_id {0}".format(sys.argv[1])
        sys.exit(1)
    comments = ' '.join(sys.argv[2:])

    project_id = get_project_id('https://code.aliyun.com/keepwalking.zeng/aos')
    if not project_id:
        print "error: can not get project_id"
        sys.exit(1)

    result = post_comments(project_id, merge_request_id, comments)
    print "post comments {0}".format(result)

    sys.exit(0)

