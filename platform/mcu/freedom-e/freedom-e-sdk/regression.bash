#!/bin/bash

unset board
unset manual
while [[ "$1" != "" ]]
do
  case "$1" in
  "--board")
    board="$2"
    shift
    shift
    ;;
  "--manual")
    manual="true"
    shift
    ;;
  esac
done

if [[ "$board" == "" ]]
then
  echo "Specify a --board to test" >&2
  exit 1
fi

if [[ "$manual" == "true" ]]
then
  echo "You're running in manual test mode.  Please connect a $board to continue..."
  read -p "Press ENTER to continue"
fi

set -e

find software -mindepth 1 -maxdepth 2 -type d | cut -d/ -f2 | while read program
do
  # Allow examples to specify the boards on which they won't run.
  if test -f software/$program/.unsupported-boards
  then
    if grep -q $board software/$program/.unsupported-boards
    then
      continue
    fi
  fi

  # Build the example.
  make BOARD=$board PROGRAM=$program software

  # We require user interaction in order to verify the tests succeed.  If the
  # user hasn't specified the manual flag then don't bother running the tests.
  if [[ "$manual" == "true" ]]
  then
    echo "We're about to run $program on $board, waiting 10 seconds"
    sleep 1s

    echo "flashing $program"
    make BOARD=$board PROGRAM=$program upload

    echo "$program has been flashed to your $board, please test it.  GDB will open in 2 seconds"
    urxvt -hold -e make BOARD=$board PROGRAM=$program run_openocd &
    sleep 2s
    urxvt -hold -e make BOARD=$board PROGRAM=$program run_gdb

    echo "Close the running OpenOCD"
    wait
  fi
done

if [[ "$manual" != "true" ]]
then
  echo "You haven't actually verified the tests run, please don't release!" >&2
fi
