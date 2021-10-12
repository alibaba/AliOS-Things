#!/usr/bin/env python
# -*- coding:utf-8 -*-
###
# Filename: /Users/guoliang.wgl/Downloads/PyTest/modtest.py
# Path: /Users/guoliang.wgl/Downloads/PyTest
# Created Date: Thursday, July 1st 2021, 5:40:58 pm
# Author: guoliang.wgl
# 
# Copyright (c) 2021 AliBaBa
###
import unittest


class Testing(unittest.TestCase):
    def test_string(self):
        a = 'some'
        b = 'some'
        self.assertEqual(a, b)

    def test_boolean(self):
        a = True
        b = True
        self.assertEqual(a, b)

if __name__ == '__main__':
    unittest.main()