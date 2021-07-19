/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/builtin.h"

const char haas_help_text[] =
    "Welcome to MicroPython on the HaaS!\r\n"
    "\r\n"
    "For generic online docs please visit https://g.alicdn.com/HaaSAI/PythonDoc/quickstart/index.html\r\n"
    "\r\n"
    "Control commands:\r\n"
    "  CTRL-A        -- on a blank line, enter raw REPL mode\r\n"
    "  CTRL-B        -- on a blank line, enter normal REPL mode\r\n"
    "  CTRL-C        -- interrupt a running program\r\n"
    "  CTRL-D        -- on a blank line, do a soft reset of the board\r\n"
    "  CTRL-E        -- on a blank line, enter paste mode\r\n"
    "\r\n"
    "For further help on a specific object, type help(obj)\r\n"
    "For a list of available modules, type help('modules')\r\n"
;
