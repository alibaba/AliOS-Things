#!/bin/bash

function start_tcpdump {
  testfunc=$1
  logfile=$LOGDIR/"$testfunc"_$(date +%s).dump
  tcpdumpcmd="$tcpdump -i $INTERFACE host $SERVERADDRESS and (udp port $SERVERPORT or icmp) -s0 -n -w $logfile"
  if [ $verbose ]; then
    echo "$tcpdumpcmd"
    $tcpdumpcmd&
  else
    $tcpdumpcmd 2>/dev/null&
  fi
  tcpdumppid=$!
  # wait until tcpdump is ready to write
  for t in {0..20} ; do
    if [ ! -e $logfile ] ; then
      sleep 0.1
    else
      sleep 1
      if [ $verbose ]; then
        echo "tcpdump started"
      fi
      break
    fi
  done
}

function kill_tcpdump {
  kill $tcpdumppid 2>/dev/null || ERROR=1
  if [ $ERROR ]; then
    echo "tcpdump failed."
    exit 1
  fi
}

function start_coap_test {
  if [ -z $1 ]; then
    echo "missing argument for start_coap_test"
    exit 1
  fi
  start_tcpdump $1
  if [ $verbose ]; then
      echo "client command: $COAP_CLIENT $clientopts $testaddress"
  fi
  echo -e "\nOutput of client:"
  # for OBS
  if [[ ! -z $2 ]]; then
    testfifo=/tmp/tmpfifo$(date +%s)
    mkfifo $testfifo
    $COAP_CLIENT $clientopts "$testaddress" &> $testfifo &
    clientpid=$!
    ($timeoutcmd $clienttimeout cat $testfifo | if read -n 1 char; then echo "output: $char" ; fi) 2>/dev/null
    # when client has written an output to fifo, kill client
    kill $clientpid
    rm $testfifo
    if [[ $2 == "wait" ]] ; then
      echo "Client killed. Waiting.."
      sleep $longtimeout
    fi
  else
    $COAP_CLIENT $clientopts "$testaddress"
  fi
  kill_tcpdump
}


##
# Perform GET (CON mode) on resource /test
#
# pre: resource /test exists and can handle GET with arbitrary payload
#
# client sends GET request with Type=0(CON) and Code=1(GET)
#
# check if sent request contains Type value indicating 0 and Code
# value indicating 1
#
# check if server sends response containing Code = 69(2.05 Content),
# the same Message ID as that of previous request, content type option
#
# verify that server displays received information
#
function TD_COAP_CORE_01 {
  clientopts="$callopts -m get"
  testaddress=coap://$SERVERTUP/test
  echo "perform GET (CON mode) on resource $testaddress"
  start_coap_test $1
}

##
# Perform POST transaction (CON mode) on resource /test: create
# resource /test
#
# pre: resource /test doesn't exist but can be created on /test
#
# client sends POST with Type=0(CON), Code=2(POST), arbitrary payload,
# Content type option
#
# check: client sends request containing Type value indicating 0 and
# Code value indicating 2
#
# verify: Server displays received information
#
# check: server sends response containing Code=69(2.01 Created), same
# message ID as that of the previous request, Content type option
#
# verify: client displays received information
#
function TD_COAP_CORE_02 {
  # -t 0: content type text/plain
  clientopts="$callopts -m post -t 0 -e sometext"
  testaddress=coap://$SERVERTUP/test
  echo "perform POST (CON) on resource $testaddress"
  start_coap_test $1
}

##
# Perform PUT transaction (CON mode) on resource /test
#
# pre: resource /test exists which can handle PUT
#
# Client sends a PUT request with Type=0(CON), Code=3(PUT), arbitrary
# payload, content type option
#
# check: sent request contains Type value indicating 0 and Code value
# indicating 3
#
# verify: server displays received information
#
# check: Server sends response containing Code=68(2.04 Changed), same
# Message ID as that of the previous request
#
# verify: client displays received response
#
function TD_COAP_CORE_03 {
  clientopts="$callopts -m put -t 0 -e sometext"
  testaddress=coap://$SERVERTUP/test
  echo "perform PUT (CON) on resource $testaddress"
  start_coap_test $1
}

##
# Perform DELETE transaction (CON mode) on resource /test
#
# pre: resource /test exists which can handle DELETE
#
# Client sends a DELETE request with Type=0(CON), Code=4(DELETE)
#
# check: sent request contains Type value indicating 0 and Code value
# indicating 4
#
# check: Server sends response containing Code=66(2.02 Deleted), same
# Message ID as that of the previous request
#
# verify: client displays received response
#
function TD_COAP_CORE_04 {
  clientopts="$callopts -m delete"
  testaddress=coap://$SERVERTUP/test
  echo "perform DELETE (CON) on resource $testaddress"
  start_coap_test $1
}

##
# Perform GET transaction (NON mode) on resource /test
#
# pre: resource /test exits which can handle GET
#
# Client sends a GET request with Type=1(NON), Code=1(GET)
#
# check: sent request contains Type value indicating 1 and Code value
# indicating 1
#
# check: Server sends response containing Type=1(NON), Code=69(2.05
# Content), content type option
#
# verify: client displays received response
#
function TD_COAP_CORE_05 {
  # -N: send non-confirmable message
  clientopts="$callopts -m get -N"
  testaddress=coap://$SERVERTUP/test
  echo "perform GET (NON mode) on resource $testaddress"
  start_coap_test $1
}

##
# Perform POST transaction (NON mode), create resource on /test
#
# pre: resource on /test doesn't exist but can be created
#
# Client sends a POST request with Type=1(NON), Code=2(POST),
# arbitrary payload, content type option
#
# check: sent request contains Type value indicating 1 and Code value
# indicating 2
#
# verify: Server displays the received information
#
# check: Server sends response containing Type=1(NON), Code=65(2.01
# Created)
#
# verify: client displays received response
#
function TD_COAP_CORE_06 {
  clientopts="$callopts -m post -t 0 -e sometext -N"
  testaddress=coap://$SERVERTUP/test
  echo "perform POST (NON) on resource $testaddress"
  start_coap_test $1
}

##
# Perform PUT transaction (NON mode) on resource /test
#
# pre: /test exists and can handle PUT
#
# Client sends a PUT request with Type=1(NON), Code=3(PUT),
# arbitrary payload, content type option
#
# check: sent request contains Type value indicating 1 and Code value
# indicating 3
#
# verify: Server displays the received information
#
# check: Server sends response containing Type=1(NON), Code=68(2.04
# Changed)
#
# verify: client displays received response
#
function TD_COAP_CORE_07 {
  clientopts="$callopts -m put -t 0 -e sometext -N"
  testaddress=coap://$SERVERTUP/test
  echo "perform PUT (NON) on resource $testaddress"
  start_coap_test $1
}

##
# Perform DELETE transaction (NON mode) on resource /test
#
# pre: /test exists and can handle DELETE
#
# Client sends a DELETE request with Type=1(NON), Code=4(DELETE)
#
# check: sent request contains Type value indicating 1 and Code value
# indicating 4
#
# check: Server sends response containing Type=1(NON), Code=66(2.02
# Deleted)
#
# verify: client displays received response
#
function TD_COAP_CORE_08 {
  clientopts="$callopts -m delete -N"
  testaddress=coap://$SERVERTUP/test
  echo "perform DELETE (NON) on resource $testaddress"
  start_coap_test $1
}

##
# Perform GET transaction with a separate response on resource /separate
#
# pre: resource /separate exists which cannot be served immediately and which
# cannot be acknowledged in a piggy-backed way
#
# Client sends a confirmable GET request to server's resource
#
# check: sent request contains Type=0(CON), Code=1(GET), client
# generated Message ID
#
# check: Server sends response containing Type=2(ACK), message ID same
# as the request, empty Payload
#
# check: Server sends response containing Type=0(CON), Code=69(2.05
# content), Payload=Content of the requested resource, Content type option
#
# check: Client sends response containing Type=2(ACK), message ID same
# as the response, empty Payload
#
# verify: client displays received response
#
function TD_COAP_CORE_09 {
  clientopts="$callopts -m get"
  testaddress=coap://$SERVERTUP/separate
  echo "perform GET (CON) on resource $testaddress which cannot be served immediately"
  start_coap_test $1
}

##
# Perform GET transaction with Token on resource /test
#
# pre: resource /test exists which can handle GET requests
#
# Client sends a confirmable GET request to server's resource
# including Token option
#
# check: sent request contains Type=0(CON) and Code=1(GET), client
# generated Token value, length of the token which should be between 1
# to 8 B, Option Type=Token
#
# check: Server sends response containing Code=69(2.05 content),
# length of the token should be between 1 to 8 B, Token value same as
# the requested, Payload=Content of the requested resource, Content
# type option
#
# verify: client displays received response
#
function TD_COAP_CORE_10 {
  clientopts="$callopts -m get -T sometok"
  testaddress=coap://$SERVERTUP/test
  echo "perform GET (CON) on resource $testaddress with Token"
  start_coap_test $1
}

##
# Perform GET transaction without Token on resource /test
#
# pre: resource /test exists which can handle GET requests
#
# Client sends a confirmable GET request to server's resource
# not containing Token option
#
# check: sent request contains Type=0(CON) and Code=1(GET), no Token
# option
#
# check: Server sends response containing Code=69(2.05 content), no
# Token option, Payload=Content of the requested resource, Content
# type option
#
# verify: client displays received response
#
function TD_COAP_CORE_11 {
  clientopts="$callopts -m get"
  testaddress=coap://$SERVERTUP/test
  echo "perform GET (CON mode) without Token on resource $testaddress"
  start_coap_test $1
}

##
# Perform GET transaction to resource /seg1/seg2/seg3
#
# pre: resource /seg1/seg2/seg3 exists on server
#
# Client sends a confirmable GET request to server's resource
#
# check: sent request contains Type=0(CON) and Code=1(GET), Option
# type=URI-Path (one for each path segment)
#
# check: Server sends response containing Code=69(2.05 content),
# Payload=Content of the requested resource, Content type option
#
# verify: client displays received response
#
function TD_COAP_CORE_12 {
  clientopts="$callopts -m get"
  testaddress=coap://$SERVERTUP/seg1/seg2/seg3
  echo "perform GET (CON mode) on resource $testaddress"
  start_coap_test $1
}

##
# Perform GET transaction to resource /query
#
# pre: resource /query exists on server
#
# Client sends a confirmable GET request with three Query parameters
# (e.g. ?first=1&second=2&third=3) to server's resource
#
# check: sent request contains Type=0(CON) and Code=1(GET), Option
# type=URI-Query (More than one query parameter)
#
# check: Server sends response containing Type=0/2(Con/ACK),
# Code=69(2.05 content), Payload=Content of the requested resource,
# Content type option
#
# verify: client displays received response
#
function TD_COAP_CORE_13 {
  clientopts="$callopts -m get"
  testaddress=coap://$SERVERTUP/query
  query="?first=1&second=2&third=3"
  echo -e "perform GET (CON mode) on resource $testaddress with query $query"
  testaddress=$testaddress$query
  start_coap_test $1
}

##
# Perform GET transaction to resource /test in lossy context
#
# pre: gateway is introduced and configured to produce packet loss,
# resource /test exists on server
#
# Configuration=CoAP_CFG_02
#
# observe: One dropped request, one dropped request ACK, one dropped
# response, one dropped response ACK and its retransmission, test
# sequence should be executed several times
#
# Client sends a confirmable GET request to server's resource
#
# check: sent request contains Type=0(CON) and Code=1(GET), Client
# generated Message ID
#
# check: Server sends response containing Type=2(ACK), Code=69(2.05
# content), Payload=Content of the requested resource, Content type
# option
#
# verify: client displays received response
#
function TD_COAP_CORE_14 {
  clientopts="$callopts -m get"
  #FIXME: address for lossy context?
  testaddress=coap://$SERVERTUP/test
  echo "perform GET (CON mode) on resource $testaddress in lossy context"
  start_coap_test $1
}

##
# Perform GET transaction to resource /separate in lossy context
#
# pre: gateway is introduced and configured to produce packet loss,
# resource /separate exists which cannot be served immediately and
# which cannot be acknowledged in a piggy-backed way
#
# Configuration=CoAP_CFG_02
#
# observe: One dropped request, one dropped request ACK, one dropped
# response, one dropped response ACK and its retransmission, test
# sequence should be executed several times
#
# Client sends a confirmable GET request to server's resource
#
# check: sent request contains Type=0(CON) and Code=1(GET), Client
# generated Message ID
#
# check: server sends response containing Type=2(ACK), Message ID same
# as the request, empty Payload
#
# check: Server sends response containing Type=0(CON), Code=69(2.05
# content), Payload=Content of the requested resource, Content type
# option
#
# check: Client sends response containing Type=2(ACK), message ID same
# as the response, empty Payload
#
# verify: client displays received response
#
function TD_COAP_CORE_15 {
  clientopts="$callopts -m get"
  #FIXME: address for lossy context?
  testaddress=coap://$SERVERTUP/separate
  echo "perform GET (CON mode) on resource $testaddress in lossy context"
  start_coap_test $1
}

### LINK ###

##
# Access to well-known interface for resource discovery
#
# Pre: client supports CoRE Link Format, server supports
# /.well-known/core resource and the CoRE Link Format
#
# Client retrieves Server's list of resource
#
# check: client sends GET request for /.well-known/core resource
#
# check: server sends response containing content-type option
# indicating 40 (application/link-format), payload indicating all the
# links available on Server
#
# client displays the list of resources available on Server
#
function TD_COAP_LINK_01 {
  clientopts="$callopts -m get"
  testaddress=coap://$SERVERTUP/.well-known/core
  echo "retrieve server's list of resource"
  start_tcpdump $1
  if [ $verbose ]; then
    echo "client command: $COAP_CLIENT $clientopts $testaddress"
  fi
  clientoutput=$($COAP_CLIENT $clientopts "$testaddress")
  if [[ ! $(echo $clientoutput | grep rt) ]] ; then
    echo "no resource with attribute rt found on server"
  else
    rt="${clientoutput##*rt=}"
    rt="${rt%%;*}"
  fi
  echo -e "\nOutput of client:"
  echo -e $clientoutput
  echo
  kill_tcpdump
}

##
# Use filtered requests for limiting discovery results
#
# Pre: client supports CoRE Link Format, server supports CoRE Link
# Format and offers different types of resources (Type 1, Type 2
# (extracted from /.well-knwon/core resource
#
# Client retrieves Server's list of resource of a specific Type 1
#
# check: client sends GET request for /.well-known/core resource
# containing URI-Query indicating "rt=Type1"
#
# check: server sends response containing content-type option
# indicating 40 (application/link-format), payload indicating only the
# links of type Type1 available on server
#
# client displays the list of resources of type Type1 available on Server
#
function TD_COAP_LINK_02 {
  clientopts="$callopts -m get"
  echo "retrieve server's list of resource for appropriate type"
  if [[ ! -z $rt ]]; then
    testaddress="coap://$SERVERTUP/.well-known/core?rt=$rt"
  else
    echo "no appropriate resource found. Skipping test"
    return
  fi
  start_coap_test $1
}

### BLOCK ###

##
# Perform GET blockwise transfer for large resource (early negotiation)
#
# pre: Client supports Block transfers, Server supports Block
# transfers, Server offers a large resource /large, client knows
# /large requires block transfer
#
# Client is requested to retrieve resource /large
#
# check: client sends a GET request containing Block2 option indicating block
# number 0 and desired block size
#
# check: Each request contains Block2 option indicating block number
# of the next block and size of the last received block
#
# check: server sends further responses containing Block2 option
# indicating block number and size
#
# verify: client displays received response
#
function TD_COAP_BLOCK_01 {
  clientopts="$callopts -m get -b 1024"
  testaddress=coap://$SERVERTUP/large
  echo "perform GET on large resource $testaddress (early negotiation)"
  start_coap_test $1
}

##
# Perform GET blockwise transfer for large resource (late negotiation)
#
# pre: Client supports Block transfers, Server supports Block
# transfers, Server offers a large resource /large, client does not
# know /large requires block transfer
#
# Client is requested to retrieve resource /large
#
# check: client sends a GET request not containing Block2 option
#
# check: server sends response containing Block2 option indicating
# block number and size
#
# check: Each request contains Block2 option indicating block number
# of the next block and size of the last received block or the desired
# size of the next block
#
# check: server sends further responses containing Block2 option
# indicating block number and size
#
# verify: client displays received response
#
function TD_COAP_BLOCK_02 {
  clientopts="$callopts -m get"
  testaddress=coap://$SERVERTUP/large
  echo "perform GET blockwise transfer for large resource (late negotiation) on resource $testaddress"
  start_coap_test $1 stop
}

##
# Perform PUT blockwise transfer for large resource
#
# pre: Client supports Block transfers, Server supports Block
# transfers, Server offers a large updatable resource /large-update
#
# Client is requested to retrieve resource /large-update on server
#
# check: client sends a PUT request containing Block1 option
# indicating block number 0 and block size
#
# check: client sends further request containing Block1 option
# indicating block number and size
#
# verify: server indicates presence of the complete updated resource
# /large-update
#
function TD_COAP_BLOCK_03 {
  clientopts="$callopts -m put -b 1024"
  testaddress=coap://$SERVERTUP/large-update
  echo "perform PUT on large resource $testaddress"
  start_coap_test $1
}

##
# Perform POST blockwise transfer for large resource
#
# pre: Client supports Block transfers, Server supports Block
# transfers, Server accepts creation of new resources on /large-create
#
# Client is requested to create a new resource on server
#
# check: client sends a POST request containing Block1 option
# indicating block number 0 and block size
#
# check: client sends further requests containing Block1 option
# indicating block number and size
#
# verify: server indicates presence of the complete new resource
#
function TD_COAP_BLOCK_04 {
  clientopts="$callopts -m post -b 1024"
  testaddress=coap://$SERVERTUP/large-create
  echo "perform POST on large resource $testaddress"
  start_coap_test $1
}

# # OBS

##
# Handle observe option
#
# pre: client supports Observe option, server supports observe option,
# server offers an observable resource /obs which changes periodically
# (e.g. every 5s.)
#
# client is requested to observe resource /obs on server
#
# check: client sends a GET request containing observe option
#
# verify: client displays the received information
#
# check: server sends response containing observe option indicating
# increasing values, as resource changes
#
# verify: client displays the updated information
#
function TD_COAP_OBS_01 {
  # we need some time to verify the correct behavior
  clientopts="$callopts -s 0" #"-v 5 -B $longtimeout -s 0"
  testaddress=coap://$SERVERTUP/obs
  echo "observe resource $testaddress"
  start_coap_test $1
}

##
# Stop resource observation
#
# pre: client supports Observe option, server supports observe option,
# server offers an observable resource /obs which changes periodically
# (e.g. every 5s.), client is observing /obs on server
#
# client is requested to stop observing resource /obs on server
#
# check: client sends a GET request not containing observe option
#
# verify: client displays the received information
#
# check: server does not send further response
#
# verify: client does not display the updated information
#
# function TD_COAP_OBS_02 {
#   #FIXME: client does not support stopping yet
  # we need some time to verify the correct behavior
#   clientopts="-v 5 -B $longtimeout -s 1"
#   testaddress=coap://$SERVERTUP/obs
#   echo "stop observing resource $testaddress"
#   start_coap_test $1
# }

##
# client detection of deregistration (Max-Age)
#
# pre: client supports Observe option, server supports observe option,
# server offers an observable resource /obs which changes periodically
# (e.g. every 5s.), client is observing /obs on server
#
# Server is rebooted
#
# check: Server does not send notifications
#
# verify: Client does not display updated information
#
# verify: After Max-Age expiration, client sends a new GET with observe
# option for Server's observable resource
#
# check: Sent request contains Observe option indicating 0
#
# check: Server sends response containing Observe option
#
# verify: client displays the received information
#
# check: Server sends response containing Observe option indicating
# increasing values, as resource changes
#
# verify: Client displays the updated information
#
function TD_COAP_OBS_03 {
  clientopts="$callopts -s 0"#"-v5 -B $longtimeout -s 0"
  testaddress=coap://$SERVERTUP/obs
  echo "client detection of deregistration (Max-Age)"
  start_coap_test $1
}

##
# Server detection of deregistration (client OFF)
#
# pre: client supports Observe option, server supports observe option,
# server offers an observable resource /obs which changes periodically
# (e.g. every 5s.), client is observing /obs on server
#
# Client is switched off
#
# check: Server’s confirmable responses are not acknowledged
#
# verify: After some delay, Server does not send further responses
#
function TD_COAP_OBS_04 {
  clientopts="$callopts -s 0"
#"-v 5 -B $longtimeout -s 0"
  testaddress=coap://$SERVERTUP/obs
  echo "server detection of deregistration (client off)"
  start_coap_test $1 wait
}

##
# Server detection of deregistration (explicit RST)
#
# pre: client supports Observe option, server supports observe option,
# server offers an observable resource /obs which changes periodically
# (e.g. every 5s.), client is observing /obs on server
#
# Client is rebooted
#
# check: Server sends response containing Observe option
#
# verify: Client discards response and does not display information
#
# check: Client sends RST to Server
#
# check: Server does not send further response
#
function TD_COAP_OBS_05 {
  clientopts="$callopts -s 0 -p $CLIENTPORT"
#"-v 5 -B $longtimeout -p $CLIENTPORT -s 0"
  testaddress=coap://$SERVERTUP/obs
  echo "server detection of deregistration (explicit RST)"
  start_coap_test $1 stop
  clientopts="$callopts -p $CLIENTPORT -s 0 -N"
#"-v 5 -B $clienttimeout -p $CLIENTPORT -s 0 -N"
  testaddress=coap://[::1]/obs
  start_coap_test $1
}
