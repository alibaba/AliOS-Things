#!/bin/bash
## this following setting is only for desktop
CURRDIR=`dirname $0`
gnome-terminal --working-directory=$CURRDIR --command="sh -c 'export PATH=$CURRDIR/bin:$PATH;bash'"