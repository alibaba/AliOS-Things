#!/bin/sh

if [ -d $1.lock ]; then
    /bin/rm -r $1.lock
fi

exit 0
