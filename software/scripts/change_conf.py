# Python script to change custom runtime configuration parameters
#     
# Arguments:
#     arg1: filename within ~/firesim/sim/custom-runtime-configs/
#     arg2: name of parameter you want to change
#     arg3: new value of parameter you want to change
#
# Example:
#     python3 change_conf.py fased_sweep.conf mm_llc_wayBits_0 2
import sys

# replaces parameter specified by string param with newval in fdata and returns modifed data
def change_param(param, newval, fdata):
    newdata = fdata
    full_string = '+' + param + '='
    for line in fdata.splitlines():
        if full_string in line:
            newline = line[0:len(full_string)] + newval
            newdata = newdata.replace(line, newline)

    return newdata

if len(sys.argv) != 4:
    print('Insufficient arguments provided.')
    quit()

custom_runtime_configs = '~/firesim/sim/custom-runtime-configs/'
f = open(custom_runtime_configs + sys.argv[1], 'r')
filedata = f.read()
f.close()

write_data = change_param(sys.argv[2], sys.argv[3], filedata)

f = open(custom_runtime_configs + sys.argv[1], 'w')
f.write(write_data)
f.close()
