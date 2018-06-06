import sys
import os
import re

log_line_prefix = "\[netmgrapp\]\>\>\>"
case_started_flag = "started"
case_completed_flag = "completed"
case_passed_flag = "succeed"

case_str_list = (r'WiFi get mac', r'WiFi scan', r'WiFi adv scan', r'WiFi monitor', r'WiFi management frame monitor', r'WiFi 80211 send', r'WiFi start', r'WiFi get ip', r'WiFi channel switch')
case_excuted = []
case_completed = []
case_passed = []

print "Welcome to ", sys.argv[0]

def help_and_exit():
    print "Usage:", sys.argv[0], "[--help|-h|<path to log file>]"
    exit(0)

if len(sys.argv) == 2:
    if sys.argv[1] is "--help" or sys.argv[1] is "-h":
        help_and_exit()
    f_name = sys.argv[1]
else:
    f_name = "./run.log"

# File must exist and readable
if os.path.isfile(f_name) == False or os.access(f_name, os.R_OK) == False:
    help_and_exit()

f = open(f_name)
#content = f.read()
#print "File content is: \n", content
line = f.readline()
while line:
    match_obj = re.match(log_line_prefix, line, 0)
    if (match_obj) is not None:
        for c in case_str_list:
            search_obj = re.search(c, line, 0)
            if search_obj is not None:
                search_obj = re.search(case_started_flag, line, 0)
                if search_obj is not None:
                    case_excuted.append(c)
                    break
                search_obj = re.search(case_completed_flag, line, 0)
                if search_obj is not None and c in case_excuted and c not in case_completed:
                    case_completed.append(c)
                    break
                search_obj = re.search(case_passed_flag, line, 0)
                if search_obj is not None and c in case_excuted and c not in case_passed:
                    case_passed.append(c)
                    break

    line = f.readline()

# |test name <35>|started <12>|completed <12>|pass <10>|
print "-------------------------------------------------------------------------"
print "|Case                              |Started?    |Completed?  |Pass?     |"
print "-------------------------------------------------------------------------"

for c in case_str_list:
    # print case name
    sys.stdout.write('|')
    sys.stdout.write(c)
    for i in range(1, 35 - len(c)):
        sys.stdout.write(' ')
    # print started state
    if c not in case_excuted:
        sys.stdout.write("|No          |N/A         |N/A       |\n")
        continue
    else:
        sys.stdout.write("|Yes         ")
    # print completed state
    if c not in case_completed:
        sys.stdout.write("|No          |N/A       |\n")
        continue
    else:
        sys.stdout.write("|Yes         ")
    # print pass state
    if c not in case_passed:
        sys.stdout.write("|No        |\n")
        continue
    else:
        sys.stdout.write("|Yes       |\n")

print "-------------------------------------------------------------------------"
print "|Summary                           |%-11s" % str(len(case_excuted)), "|%-11s" % str(len(case_completed)), "|%-9s" % str(len(case_passed)), "|"
print "-------------------------------------------------------------------------"

f.close()
