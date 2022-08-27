import re
import os


filepath_8_way            = '/home/centos/firesim/deploy/results-workload/2022-06-16--12-07-29-strided-br/strided-br-strided/run_result.csv'
filepath_4_way            = '/home/centos/firesim/deploy/results-workload/2022-06-16--03-30-26-strided-br/strided-br-strided/run_result.csv' 
filepath_2_way            = '/home/centos/firesim/deploy/results-workload/2022-06-15--18-53-20-strided-br/strided-br-strided/run_result.csv'
filepath_direct_mapped    = '/home/centos/firesim/deploy/results-workload/2022-06-15--10-16-19-strided-br/strided-br-strided/run_result.csv'
filepath_direct_mapped_32 = '/home/centos/firesim/deploy/results-workload/2022-06-15--01-39-08-strided-br/strided-br-strided/run_result.csv'
filepath_no_LLC           = '/home/centos/firesim/deploy/results-workload/2022-06-14--17-44-44-strided-br/strided-br-strided/run_result.csv'

temppaths_8_way, temppaths_4_way, temppaths_2_way, temppaths_direct_mapped, temppaths_direct_mapped_32, temppaths_no_LLC = [], [], [], [], [], []
strides = ['1', '4', '16', '64', '256']

for elem in strides:
    print(elem)
    temppaths_8_way.append('/home/centos/firesim/deploy/results-workload/2022-06-16--12-07-29-strided-br/strided-br-strided/' + elem + 'run_result.csv')
    temppaths_4_way.append('/home/centos/firesim/deploy/results-workload/2022-06-16--03-30-26-strided-br/strided-br-strided/' + elem + 'run_result.csv')
    temppaths_2_way.append('/home/centos/firesim/deploy/results-workload/2022-06-15--18-53-20-strided-br/strided-br-strided/' + elem + 'run_result.csv')
    temppaths_direct_mapped.append('/home/centos/firesim/deploy/results-workload/2022-06-15--10-16-19-strided-br/strided-br-strided/' + elem + 'run_result.csv')
    temppaths_direct_mapped_32.append('/home/centos/firesim/deploy/results-workload/2022-06-15--01-39-08-strided-br/strided-br-strided/' + elem + 'run_result.csv')
    temppaths_no_LLC.append('/home/centos/firesim/deploy/results-workload/2022-06-14--17-44-44-strided-br/strided-br-strided/' + elem + 'run_result.csv')

AppStride_pattern = re.compile('AppStride:\[\d*\.*\d*\]')


def reformat_strided(filepath, temppaths):
    file = open(filepath)
    filedata = file.read()
    file.close()

    #for path in temppaths:
    #    if not os.path.exists(path):
    #        with open(path, 'w') as newfile:

    for line in filedata.splitlines():
        AppStride_list = AppStride_pattern.findall(line)
        AppStride_str = AppStride_list[0]
        AppStride = int(AppStride_str[11:len(AppStride_str)-1])

reformat_strided(filepath_8_way, temppaths_8_way)
