#!/bin/sh
export LANG=C

IS_DIAL=1
IS_FULLTEST=0
FULLTESTCNT=1
IS_CMCC=0
IS_CUCC=0
IS_CTCC=0
CARRIER_TYPE=""
LOGDIR="$HOME/sim_log"
LOGSUFFIX=""
MODEL_TYPE=""
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"
RESULTLOG="result_$TIMESTAMP.log"
PASSCNT=0
FAILCNT=0
PINGCNT=20
## LP IP
PINGIP="47.103.184.100"
## meiriyouxian
#PINGIP="106.11.249.67"

usage()
{
   cat << END >&2
Usage:  [options]

Options:
    {--fulltest}
        start full test.
    {--count}
        full test count, default is 1
    {--no-dial}
        start test without dial.
    {-s|--switch}
        switch to carrier.
    {--cmcc}
        carrier: cmcc.
    {--cucc}
        carrier: cucc.
    {--ctcc}
        carrier: ctcc.
    {--type}
        model type: ec200s, ec100y
END
    exit 1
}

switch_to_carrier()
{
    local idx=0

    echo "start to switch (write to phonebook)..."
    if [[ "$IS_CMCC" -eq 1 ]]; then
        idx=1
    elif [[ "$IS_CUCC" -eq 1 ]]; then
        idx=2
    elif [[ "$IS_CTCC" -eq 1 ]]; then
        idx=3
    else
        echo "carrier not set!"
        exit 1
    fi

    echo "mn init mn select$idx mn deinit exit" | LD_LIBRARY_PATH=../modules ./mn_manual_switch_test 2>&1 | tee $LOGDIR/demo_${CARRIER_TYPE}_${TIMESTAMP}_$1.log

    ## check switch result
    cat $LOGDIR/demo_${CARRIER_TYPE}_${TIMESTAMP}_$1.log | grep "after switch, current slot is $idx\|is same, no need to switch"
    if [[ $? != 0 ]]; then
        echo "fail to switch to $CARRIER_TYPE !"
        echo "fail to switch to $CARRIER_TYPE !" >> $LOGDIR/$RESULTLOG
        FAILCNT=$(expr $FAILCNT + 1)
        return 1
    fi

    ## dial
    if [[ "$IS_DIAL" -eq 1 ]]; then
        do_dial
    else
        echo "switch to $CARRIER_TYPE pass !"
        echo "$CARRIER_TYPE test pass" >> $LOGDIR/$RESULTLOG
        PASSCNT=$(expr $PASSCNT + 1)
    fi
}

start_to_ping()
{
    ping $PINGIP -c $PINGCNT -i 0.5 $1 2>&1 | tee $LOGDIR/ping_${CARRIER_TYPE}_${TIMESTAMP}.log

    pingresult01=$(expr $PINGCNT - 1)
    pingresult02=$(expr $PINGCNT - 2)
    cat $LOGDIR/ping_${CARRIER_TYPE}_${TIMESTAMP}.log | grep "$PINGCNT received\|$pingresult01 received\|$pingresult02 received"

    if [[ $? != 0 ]]; then
        echo "fail to ping to $CARRIER_TYPE !"
        echo "fail to ping to $CARRIER_TYPE !" >> $LOGDIR/$RESULTLOG
        FAILCNT=$(expr $FAILCNT + 1)
    else
        echo "$CARRIER_TYPE test pass" >> $LOGDIR/$RESULTLOG
        PASSCNT=$(expr $PASSCNT + 1)
    fi
}

dial_one_time()
{
    local cnt=0

    log_name="$LOGDIR/quectel-CM_${CARRIER_TYPE}_${TIMESTAMP}.log"

    echo "start to dial ..."
    if [[ "$IS_CMCC" -eq 1 ]]; then
        sudo quectel-CM -f $log_name &
    elif [[ "$IS_CUCC" -eq 1 ]]; then
        sudo quectel-CM -f $log_name &
    elif [[ "$IS_CTCC" -eq 1 ]]; then
        sudo quectel-CM -f $log_name &
    else
        echo "carrier not set!"
        exit 1
    fi

    while [[ $cnt -lt 90 ]]; do
        cnt=$(expr $cnt + 1)
        sleep 1

        # check if ip obtained
        cat $log_name | grep "Lease of"

        if [[ $? == 0 ]]; then
            break
        fi
    done

    if [[ $cnt -eq 90 ]]; then
        echo "fail to dial to $CARRIER_TYPE !"
        sudo pkill -2 quectel-CM
        return 1
    else
        return 0
    fi
}

dial_to_carrier()
{
    local cnt=0

    while [[ $cnt -lt 2 ]]; do
        dial_one_time
        if [ $? == 0 ]; then
            break;
        fi

        cnt=$(expr $cnt + 1)
    done

    if [[ $cnt -eq 2 ]]; then
        echo "fail to dial to $CARRIER_TYPE !" >> $LOGDIR/$RESULTLOG
    else
        start_to_ping
        sudo pkill -2 quectel-CM
    fi
}

dial_to_ec200s()
{
    sudo sh -c 'echo -e "AT+qnetdevctl=1,1\r" > /dev/ttyUSB1'
    sleep 5
    sudo sh -c 'echo "nameserver 192.168.43.1" > /etc/resolv.conf'
    sudo route add default gw 192.168.43.1
    start_to_ping -Iusb0
}

do_dial()
{
    if [[ "$MODEL_TYPE" == "ec200s" || "$MODEL_TYPE" == "ec100y" ]]; then
        dial_to_ec200s
    else
        dial_to_carrier
    fi
}

set_cmcc_mode()
{
    echo "start cmcc test"
    IS_CMCC=1
    IS_CUCC=0
    IS_CTCC=0
    CARRIER_TYPE="cmcc"
}

set_cucc_mode()
{
    echo "start cucc test"
    IS_CMCC=0
    IS_CUCC=1
    IS_CTCC=0
    CARRIER_TYPE="cucc"
}

set_ctcc_mode()
{
    echo "start ctcc test"
    IS_CMCC=0
    IS_CUCC=0
    IS_CTCC=1
    CARRIER_TYPE="ctcc"
}

print_test_result()
{
    echo "==========="
    echo "test result"
    echo "==========="

    cat $LOGDIR/$RESULTLOG

    echo "pass cnt: $PASSCNT"
    echo "fail cnt: $FAILCNT"
}

full_test()
{
    echo "start full test"
    local idx=0


    while [[ $idx -lt $FULLTESTCNT ]]; do

        set_cucc_mode
        switch_to_carrier $idx

        set_ctcc_mode
        switch_to_carrier $idx

        set_cmcc_mode
        switch_to_carrier $idx

        idx=$(expr $idx + 1)
    done

    print_test_result
}

pre_check()
{
    if [[ ! -d $LOGDIR ]]; then
        mkdir -p $LOGDIR
    fi
}

pre_check

while [ $# != 0 ]; do
    case $1 in
        -h|--help)
            usage
            break;;
        --fulltest)
            IS_FULLTEST=1;;
        --count)
            FULLTESTCNT=$2
            shift;;
        --no-dial)
            IS_DIAL=0;;
        -s|--switch)
            IS_SWITCH=1;;
        --cmcc)
            IS_CMCC=1
            CARRIER_TYPE="cmcc";;
        --cucc)
            IS_CUCC=1
            CARRIER_TYPE="cucc";;
        --ctcc)
            IS_CTCC=1
            CARRIER_TYPE="ctcc";;
        --type)
            MODEL_TYPE=$2
            shift;;
        --)
            usage
            exit 1;;
    esac
    shift
done

if [[ "$IS_FULLTEST" -eq 1 ]]; then
    full_test
    exit 0
fi

if [[ -z "$CARRIER_TYPE" ]]; then
    usage
    exit 1
fi

if [[ "$IS_SWITCH" -eq 1 ]]; then
    switch_to_carrier 0
else
    do_dial
fi
