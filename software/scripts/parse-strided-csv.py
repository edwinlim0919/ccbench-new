# Plots strided results for given result directories into a file specified by user
#
# Arguments
#     arg1: no LLC result dir
#     arg2: 4 MB 8 way LLC result dir
#     arg3: 4 MB 4 way LLC result dir
#     arg4: 4 MB 2 way LLC result dir
#     arg5: 4 MB direct mapped LLC result dir
#     arg6: 32 MB direct mapped LLC result dir
#
# Example Usage
#

import matplotlib
import matplotlib.pyplot as plt
import re
import sys
import statistics


def remove_outliers(AppSizes, Times):
    curr_AppSize = AppSizes[0]
    curr_Times = [Times[0]]
    AppSizes_cleaned = []
    Times_cleaned = []

    for i in range(len(AppSizes)-1):
        index = i+1
        if AppSizes[index] != curr_AppSize:
            median = statistics.median(curr_Times)
            AppSizes_cleaned.append(curr_AppSize)
            Times_cleaned.append(median)
            curr_AppSize = AppSizes[index]
            curr_Times.clear()    
        curr_Times.append(Times[index])
    
    median = statistics.median(curr_Times)
    AppSizes_cleaned.append(curr_AppSize)
    Times_cleaned.append(median)
    return AppSizes_cleaned, Times_cleaned

def plot_separate(AppSizes_clean, runTimeUs_clean, line_color, stride):
    NumBytes = [size * 4 for size in AppSizes_clean]
    plt.plot(NumBytes, runTimeUs_clean, color=line_color, label=stride)
    
runTimeUs_stride = []
AppSizes_stride = []
AppSize_pattern = re.compile('AppSize:\[\d*\.*\d*\]')
runTimeUs_pattern = re.compile('runTimeUs:\[\d*\.*\d*\]')
plt.locator_params(axis='x', nbins=10)
plt.locator_params(axis='y', nbins=10)
plt.xlabel('AppSize (bytes)', fontsize=8)
plt.ylabel('Time (us)', fontsize=8)


# stride-1
f = open(sys.argv[1], 'r')
fdata = f.read()
for line in fdata.splitlines():
    runTimeUs_stride.append(int(float(runTimeUs_pattern.findall(line)[0][11:-1])))
    AppSizes_stride.append(int(AppSize_pattern.findall(line)[0][9:-1]))

AppSizes_stride_clean, runTimeUs_stride_clean = remove_outliers(AppSizes_stride, runTimeUs_stride)
plot_separate(AppSizes_stride_clean, runTimeUs_stride_clean, 'red', 'stride-1')
f.close()
runTimeUs_stride.clear()
AppSizes_stride.clear()

# stride-4
f = open(sys.argv[2], 'r')
fdata = f.read()
for line in fdata.splitlines():
    runTimeUs_stride.append(int(float(runTimeUs_pattern.findall(line)[0][11:-1])))
    AppSizes_stride.append(int(AppSize_pattern.findall(line)[0][9:-1]))

AppSizes_stride_clean, runTimeUs_stride_clean = remove_outliers(AppSizes_stride, runTimeUs_stride)
plot_separate(AppSizes_stride_clean, runTimeUs_stride_clean, 'blue', 'stride-4')
f.close()
runTimeUs_stride.clear()
AppSizes_stride.clear()

# stride-16
f = open(sys.argv[3], 'r')
fdata = f.read()
for line in fdata.splitlines():
    runTimeUs_stride.append(int(float(runTimeUs_pattern.findall(line)[0][11:-1])))
    AppSizes_stride.append(int(AppSize_pattern.findall(line)[0][9:-1]))

AppSizes_stride_clean, runTimeUs_stride_clean = remove_outliers(AppSizes_stride, runTimeUs_stride)
plot_separate(AppSizes_stride_clean, runTimeUs_stride_clean, 'green', 'stride-16')
f.close()
runTimeUs_stride.clear()
AppSizes_stride.clear()

# stride-64
f = open(sys.argv[4], 'r')
fdata = f.read()
for line in fdata.splitlines():
    runTimeUs_stride.append(int(float(runTimeUs_pattern.findall(line)[0][11:-1])))
    AppSizes_stride.append(int(AppSize_pattern.findall(line)[0][9:-1]))

AppSizes_stride_clean, runTimeUs_stride_clean = remove_outliers(AppSizes_stride, runTimeUs_stride)
plot_separate(AppSizes_stride_clean, runTimeUs_stride_clean, 'darkviolet', 'stride-64')
f.close()
runTimeUs_stride.clear()
AppSizes_stride.clear()

# stride-256
f = open(sys.argv[5], 'r')
fdata = f.read()
for line in fdata.splitlines():
    runTimeUs_stride.append(int(float(runTimeUs_pattern.findall(line)[0][11:-1])))
    AppSizes_stride.append(int(AppSize_pattern.findall(line)[0][9:-1]))

AppSizes_stride_clean, runTimeUs_stride_clean = remove_outliers(AppSizes_stride, runTimeUs_stride)
plot_separate(AppSizes_stride_clean, runTimeUs_stride_clean, 'cyan', 'stride-256')
f.close()
runTimeUs_stride.clear()
AppSizes_stride.clear()


plt.legend()
plt.savefig(sys.argv[6])

