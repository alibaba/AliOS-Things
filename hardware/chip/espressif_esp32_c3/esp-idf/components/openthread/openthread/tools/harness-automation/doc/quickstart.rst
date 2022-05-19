Thread Harness Automation Quick Start
=====================================

Setup
-----

#. Install Thread-Test-Harness1.1-Alpha v1.0-Release_40.0
#. Install python 2.7
#. Get the OpenThread and switch to the harness automation path::

    git clone https://github.com/openthread/openthread.git
    cd openthread/tools/harness-automation

#. Install python libraries dependencies::

    pip install -r requirements.txt

#. Update settings.

    Just copy the sample and modify according to the comments carefully::

        cp autothreadharness/settings_sample.py autothreadharness/settings.py

Run single case
---------------

::

    # bash
    ./start.sh Router_5_1_1

    # windows command line
    start.bat Router_5_1_1

Run all cases
-------------

::

    # bash
    ./start.sh

    # windows command line
    start.bat

This will record the results in result.json, so that you can continue running cases once broken. You can also get help information with argument -h or --help.

List devices
------------

::

    # bash
    ./start.sh -l

    # windows command line
    start.bat -l

Check single device
-------------------

::

    # bash
    ./start.sh -l COM28

    # windows command line
    start.bat -l COM28

Get Help
---------

::

    # bash
    ./start.sh -h

    # windows command line
    start.bat -h
