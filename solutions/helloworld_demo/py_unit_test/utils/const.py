#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File       :    const.py
@Description:    file description   
@Date       :    2021/08/12 14:24:27
@Author     :    guoliang.wgl
@version    :    1.0
'''

CONST_LOG_TAG = 'AosAutoTest'
CONST_LOG_START = 'Start'
CONST_LOG_END = 'End'
CONST_LOG_MOD_START = 'ModStart'
CONST_LOG_MOD_END = 'ModEnd'
CONST_LOG_CASE_START = 'CaseStart'
CONST_LOG_CASE_END = 'CaseEnd'
CONST_LOG_RESULT = 'Result'
CONST_LOG_MOD_RESULT = 'ModResult'
CONST_LOG_CASE_RESULT = 'CaseResult'
CONST_LOG_CONDITION = 'Condition'


class Condition(object):
    SET_DEV_PROPS = 1


CONST_LOG_TAG_SEP = ':'


def getAutoTestTag():
    tag = []
    tag.append(CONST_LOG_TAG)
    tag.append(CONST_LOG_TAG_SEP)
    return ''.join(tag)

def getAutoTestLevelTag(level):
    tag = []
    tag.append(CONST_LOG_TAG)
    tag.append(level)
    tag.append('')
    return CONST_LOG_TAG_SEP.join(tag)

def getAutoTestConditionTag(condition):
    tag = []
    tag.append(CONST_LOG_TAG)
    tag.append(CONST_LOG_CONDITION)
    tag.append(str(condition))
    tag.append('')
    return CONST_LOG_TAG_SEP.join(tag)


def getTestInfoByLine(line_str):
    print(line_str)
    log_part2 = []
    log_part2.append(CONST_LOG_START)
    log_part2.append(CONST_LOG_END)
    log_part2.append(CONST_LOG_MOD_START)
    log_part2.append(CONST_LOG_MOD_END)
    log_part2.append(CONST_LOG_CASE_START)
    log_part2.append(CONST_LOG_CASE_END)
    log_part2.append(CONST_LOG_RESULT)
    log_part2.append(CONST_LOG_MOD_RESULT)
    log_part2.append(CONST_LOG_CASE_RESULT)


    strlist = line_str.split(CONST_LOG_TAG_SEP)
    str_len =  len(strlist)
    if str_len < 2:
        return ''
    elif str_len == 2:
        return strlist[1]
    elif strlist == 3:
        if strlist[1] in log_part2:
            return strlist[2]
        else:
            return strlist[1] + CONST_LOG_TAG_SEP +  strlist[2]
    else:
        if CONST_LOG_CONDITION == strlist[1]:
            start_index = 3
        elif strlist[1] in log_part2:
            start_index = 2
        else:
            start_index  = 1
        ret_str = []
        while start_index < str_len:
            ret_str.append(strlist[start_index])
            start_index = start_index + 1
        return CONST_LOG_TAG_SEP.join(ret_str)

TEST_RESULT_FILE = '/data/unittest_result.txt'
TEST_CASE_TIMEOUT = '3600'

SSID = 'KIDS'
PASSWORD = '12345678'


WIFI_CONNECT_TIMEOUT = 60*1000
WIFI_STATUS = (0, 1, 2, 3, 4, 5, 6, 7)


LK_CONNECT_TIMEOUT= 10*1000 
LK_PROPS_TIMEOUT= 60*1000
LK_SERVICE_TIMEOUT= 60*1000 
