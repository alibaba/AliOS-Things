#!/bin/bash

# test coap implementation for the ETSI CoAP Plugtest in March 2012
# with test cases described in Plugtests Guide First Draft V0.0.16

COAP_CLIENT=./coap-client
tcpdump=/usr/sbin/tcpdump
DEFAULTPORT=5683
CLIENTPORT=61701
# directory for logging
LOGDIR=logs
# set client's verbose level
callopts=" -v 5"
longtimeout=180
clienttimeout=$longtimeout
timeoutcmd=/usr/bin/timeout

#URIREGEX=.*\/.*
# resembles approximately an ip address
IPADDRREGEX="^[1-2]?[0-9]{1,2}\.[1-2]?[0-9]{1,2}\.[1-2]?[0-9]{1,2}\.[1-2]?[0-9]{1,2}"
# FIXME IPV6 address
IP6REGEX=".*:.*:.*"

# Testcase groups
CORE=( TD_COAP_CORE_01 TD_COAP_CORE_02 TD_COAP_CORE_03
TD_COAP_CORE_04 TD_COAP_CORE_05 TD_COAP_CORE_06 TD_COAP_CORE_07
TD_COAP_CORE_08 TD_COAP_CORE_09 TD_COAP_CORE_10 TD_COAP_CORE_11
TD_COAP_CORE_12 TD_COAP_CORE_13 TD_COAP_CORE_14 TD_COAP_CORE_15 )
LINK=( TD_COAP_LINK_01 TD_COAP_LINK_02 )
BLOCK=( TD_COAP_BLOCK_01 TD_COAP_BLOCK_02 TD_COAP_BLOCK_03 TD_COAP_BLOCK_04 )
OBS=( TD_COAP_OBS_01 TD_COAP_OBS_02 TD_COAP_OBS_03 TD_COAP_OBS_04 TD_COAP_OBS_05 )

testgroups=( CORE LINK BLOCK OBS )

# if no test cases are specified, we want to run all tests
testnumber=-1
group=''

source `dirname $0`/etsi_testcases.sh

function usage {
echo "Usage:  `basename $0` [-n testnumber] [-g groupname] [-t timeout] [-P server_port] [-p client port] [-d logdir] [-v] -i interface server_address" 1>&2
echo "-n test case to be accomplished" 1>&2
echo "-g group to be tested" 1>&2
echo "-t time in seconds until timout for single test" 1>&2
echo "-i interface to use for tcpdump" 1>&2
echo "-P port of server" 1>&2
echo "-p port client listens on" 1>&2
echo "-d directory for logfiles" 1>&2
echo "-v verbose level" 1>&2
}

function run_test {
  tn=$1
  clientopts=''
  if [ -z $1 ]; then
    echo "missing argument for run_test"
    exit 1
  fi
  echo -e "running test: $tn"
  if [ $(type -t $tn) ] ; then
    $tn $tn
    echo
  else
    echo "not implemented"
    echo
  fi
}

while getopts "n:g:t:i:P:p:d:v" OPTION;
do
# A missing argument for an option leads getopts to take the next
# option as the parameter. We want to prevent that.
case $OPTARG in
-*) echo "Missing argument for option \"-$OPTION\"."
echo $USAGE
exit 1
;;
esac

case $OPTION in
n) # number of test case
testnumber=$((OPTARG-1))
;;

g) # name of test group
# is there a group with that name?
for i in "${testgroups[@]}"
  do
  # group doesn't have to be case sensitive
  tmpgroup=$(echo $OPTARG | tr '[:lower:]' '[:upper:]')
  if [  $i == $tmpgroup ] ; then
    group=$tmpgroup
    break
  fi
done
if [ -z $group ] ; then
  echo "No such group:" $OPTARG". Available groups are: ${testgroups[@]}"
  exit 1
fi
;;

t)
# add timeout to client parameters
clienttimeout=$((OPTARG))
callopts="$callopts -B $clienttimeout"
;;

i)
# interface tcpdump listens on
INTERFACE=$OPTARG
;;

P)
# port the server listens on
SERVERPORT=$((OPTARG))
;;

p)
# port the client listens on
CLIENTPORT=$((OPTARG))
;;

d)
# directory tcpdump writes the logfiles into
LOGDIR=$OPTARG
;;

v)
verbose=1
;;

?)
# any other option is invalid
echo -e $USAGE 1>&2
exit 1
;;
esac
done

# catch last argument: server address
ARGS=$(($#+1))
SERVERADDRESS=${@: -1}

if [[ ! $((ARGS-OPTIND)) -eq 1 ]]; then
  echo -e "\nno server address specified"
  usage
  exit 1
fi

# if no port number was specified by user, the server address for the
# coap-client is $SERVERADDRESS

if [ -z $SERVERPORT ]; then
  SERVERPORT=$DEFAULTPORT
  if [[ $SERVERADDRESS =~ $IP6REGEX ]]; then
    SERVERTUP=\[$SERVERADDRESS\]
  else
    SERVERTUP=$SERVERADDRESS
  fi
else
   if [[ $SERVERADDRESS =~ $IP6REGEX ]]; then
    SERVERTUP=\[$SERVERADDRESS\]:$SERVERPORT
  else
    SERVERTUP=$SERVERADDRESS:$SERVERPORT
  fi
fi

# create directory for logging, if it's not already there
if [[ ! -e $LOGDIR ]]; then
  mkdir -p $LOGDIR
  if [ $? ]; then
    echo created directory \""$LOGDIR"\" for logging
  fi
fi

# the interface for tcpdump is mandatory
if [ -z $INTERFACE ]; then
  echo -e "\nno interface given"
  exit 1
fi

# determine which tests to run
if [ -n "$group" ] ; then
  echo group: $group
  if [[ ! $testnumber -eq -1 ]] ; then
    groupsize=$(eval "echo \${#$(echo $group)[@]}")
    # is there a testcase with number $testnumber in group $group
    if [ $testnumber -ge $groupsize -o $testnumber -lt 0 ] ; then
      echo "No such testcase number: $OPTARG. Test cases numbers are 1 to" $groupsize
      exit 1
    else
      # run test with group $group and number $testnumber
      run_test $(eval "echo \${$(echo $group)[$testnumber]}")
    fi
  else
    # if no testnumber was specified, we want to run all tests in that group
    for i in $(eval "echo \${$(echo $group)[@]}") ; do
      run_test $i
    done
  fi
else
    # run all tests of all groups
    for j in ${testgroups[@]} ; do
      echo "group: $j"
      for k in $(eval "echo \${$(echo $j)[@]}") ; do
        run_test $k
      done
    done
fi
