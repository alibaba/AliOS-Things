#!/bin/bash

########################################################################################
# code formatting

function ci_code_formatting_setup {
    sudo apt-add-repository --yes --update ppa:pybricks/ppa
    sudo apt-get install uncrustify
    pip3 install black
    uncrustify --version
    black --version
}

function ci_code_formatting_run {
    tools/codeformat.py -v
}
