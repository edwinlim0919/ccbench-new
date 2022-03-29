# Generates plots from ccbench cache benchmarks based on arguments provided
#     arg1: lower cutoff of range
#     arg2: upper cutoff of range
#
# Sizes for ProtoMegaBoomBaseConfigNoAccel
#     arg1: 32 KB = 32768
#     arg2: 512 KB = 2^19 B = 524288
#     arg3: 2^27 B = 134217728 B
import matplotlib
import matplotlib.pyplot as plt
import re
import sys


result_dir = '../../results'
plot_dir = '../../plots'
result_file = '/run_result.csv'
full_plot_separate = '/full_plot_separate.pdf'
full_plot_together = '/full_plot_together.pdf'
range_plot_separate = '/range_plot_separate.pdf'
range_plot_together = '/range_plot_together.pdf'
configs = ['/direct-mapped', '/2-way-set-associative', '/4-way-set-associative', '/8-way-set-associative']
plt_coords = [[0, 0], [0, 1], [1, 0], [1, 1]]
plt_colors = ['red', 'blue', 'green', 'darkviolet']


# Parses .csv file generated by simulation
def parse_run_result(fdata):
    AppSizes, Times = [], []
    AppSize_pattern = re.compile('AppSize:\[\d*\.*\d*\]')
    Time_pattern = re.compile('Time:\[\d*\.*\d*\]')

    for line in fdata.splitlines():
        AppSize_str = AppSize_pattern.findall(line)[0]
        Time_str = Time_pattern.findall(line)[0]
        AppSize = int(AppSize_str[9:len(AppSize_str)-1])
        Time = float(Time_str[6:len(Time_str)-1])
        AppSizes.append(AppSize)
        Times.append(Time)

    return AppSizes, Times        


def find_median(curr_Times):
    curr_Times.sort()
    midpoint1 = len(curr_Times) // 2 - 1
    midpoint2 = len(curr_Times) // 2

    if len(curr_Times) % 2 == 0:
        median = (curr_Times[midpoint1] + curr_Times[midpoint2]) / 2
    else:
        median = curr_Times[midpoint1]
    
    return median


# Removes outliers from data by recording median run result of each AppSize
def remove_outliers(AppSizes, Times):
    curr_AppSize = AppSizes[0]
    curr_Times = [Times[0]]
    AppSizes_cleaned = []
    Times_cleaned = []

    for i in range(len(AppSizes)-1):
        index = i+1

        if AppSizes[index] != curr_AppSize:
            # We saved all run results for a single AppSize, and now we record the median
            median = find_median(curr_Times)
            AppSizes_cleaned.append(curr_AppSize)
            Times_cleaned.append(median)
            curr_AppSize = AppSizes[index]
            curr_Times.clear()

        curr_Times.append(Times[index])

    # Last AppSize has not been recorded yet
    median = find_median(curr_Times)
    AppSizes_cleaned.append(curr_AppSize)
    Times_cleaned.append(median)

    return AppSizes_cleaned, Times_cleaned 


# Generates individual graphs for different configurations
#     plt_range: a range (in bytes) over which to plot points
#     plt_name: specifies name of graph pdf
def plot_separate(plt_range, plt_name):
    plt.clf()
    figure, axis = plt.subplots(2, 2, sharex=True, sharey=True)
    figure.tight_layout(pad=3.0)
    plt.locator_params(axis='x', nbins=10)
    plt.locator_params(axis='y', nbins=10)
    plt.xlabel('AppSize (bytes)', fontsize=8)
    plt.ylabel('Cycles Per Iteration', fontsize=8)

    for i in range(len(configs)):
        config, plt_coord, plt_color = configs[i], plt_coords[i], plt_colors[i]

        fpath = result_dir + config + result_file
        f = open(fpath, 'r')
        fdata = f.read()
        f.close()
        AppSizes, Times = parse_run_result(fdata)
        AppSizes_cleaned, Times_cleaned = remove_outliers(AppSizes, Times)

        # AppSizes specifies number of unsigned 32-bit ints, so multiply by 4 for bytes
        NumBytes = [size * 4 for size in AppSizes_cleaned]
        NumBytes_plt, Times_plt = [], []

        if len(plt_range) == 0:
            NumBytes_plt, Times_plt = NumBytes, Times_cleaned
        else:
            for i in range(len(NumBytes)):
                if NumBytes[i] >= plt_range[0] and NumBytes[i] <= plt_range[1]:
                    NumBytes_plt.append(NumBytes[i])
                    Times_plt.append(Times_cleaned[i])

        axis[plt_coord[0], plt_coord[1]].plot(NumBytes_plt, Times_plt, color=plt_color)
        axis[plt_coord[0], plt_coord[1]].set_title(config[1:], fontsize=8)

    plt.savefig(plot_dir + plt_name)


# Generates a single graph containing all different configurations
#     plt_range: a range (in bytes) over which to plot points
#     plt_name: specifies name of graph pdf
def plot_together(plt_range, plt_name):
    plt.clf()
    plt.locator_params(axis='x', nbins=10)
    plt.locator_params(axis='y', nbins=10)
    plt.xlabel('AppSize (bytes)', fontsize=8)
    plt.ylabel('Cycles Per Iteration', fontsize=8)

    for i in range(len(configs)):
        config, plt_color = configs[i], plt_colors[i]

        fpath = result_dir + config + result_file
        f = open(fpath, 'r')
        fdata = f.read()
        f.close()
        AppSizes, Times = parse_run_result(fdata)
        AppSizes_cleaned, Times_cleaned = remove_outliers(AppSizes, Times)    

        # AppSizes specifies number of unsigned 32-bit ints, so multiply by 4 for bytes
        NumBytes = [size * 4 for size in AppSizes_cleaned]
        NumBytes_plt, Times_plt = [], []

        if len(plt_range) == 0:
            NumBytes_plt, Times_plt = NumBytes, Times_cleaned
        else:
            for i in range(len(NumBytes)):
                if NumBytes[i] >= plt_range[0] and NumBytes[i] <= plt_range[1]:
                    NumBytes_plt.append(NumBytes[i])
                    Times_plt.append(Times_cleaned[i]) 
        
        plt.plot(NumBytes_plt, Times_plt, color=plt_color, label=config[1:])

    plt.legend()
    plt.savefig(plot_dir + plt_name)


if len(sys.argv) != 3:
    print("Incorrect number of arguments provided.")
else:
    plot_range = [int(sys.argv[1]), int(sys.argv[2])]
    plot_separate(plot_range, range_plot_separate)
    plot_together(plot_range, range_plot_together)
    plot_separate([], full_plot_separate)
    plot_together([], full_plot_together)
