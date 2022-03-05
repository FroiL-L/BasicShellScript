#################################
# flunalopez_examine_system.py
#   Program extrapolates information from /proc and ouputs
#   said information to the file 'flunalopez_systemDetails.txt'.
#
# @author: Froilan Luna-Lopez
#   University of Nevada, Reno
#   24 February 2022
#################################

# Libraries
import os

#################################
# Notes
#   - Kernel versions: /proc/versions
#   - Time since last boot: /proc/uptime
#   - Subtract above from current time
#   - Number of requests made to disk (read + write): field 1 and 5: /proc/diskstats
#   - Number of processes since last boot: /proc/stat
#################################

#################################
# getProcessCount()
#   Grabs and returns the number of proccesses made since boot.
# args:
#   None
# return:
#   Integer for number of processes.
#################################
def getProcessCount():
    process_count = None
    with open('/proc/stat', 'r') as df:
        df_lines = df.readlines()
        process_count = int(df_lines[-4].split()[-1])
    return process_count

#################################
# getDiskRequestCount()
#   Grabs and returns the total number of requests made to the disk.
# args:
#   None
# return:
#   Integer with total number of requests made to disk.
#################################
def getDiskRequestCount():
    requestCount = 0
    with open('/proc/diskstats', 'r') as df:
        df_lines = df.readlines()
        for line in df_lines:
            line_split = line.split()
            requestCount += int(line_split[3]) + int(line_split[7])
    return requestCount

#################################
# getTimeOfBoot()
#   Determines and returns the time at boot.
# args:
#   None
# return:
#   String with time at boot.
#################################
def getTimeOfBoot():
    ls_content = os.popen('ls -ld /proc/1').readlines()[0]
    ls_split = ls_content.split()
    return ls_split[-2]

#################################
# getTimeSinceBoot()
#   Grabs and returns the time since the last boot.
# args:
#   None
# return:
#   String with seconds since last boot.
#################################
def getTimeSinceBoot():
    boot_counter = ''
    with open('/proc/stat', 'r') as df:
       df_lines = df.readlines()
       boot_counter = df_lines[-5].split()[-1]
    return boot_counter

#################################
# getKernelVersion()
#   Grabs and returns current linux kernal version.
# args:
#   None
# return:
#   String with current linux version.
#################################
def getKernelVersion():
    version = ''
    with open('/proc/version', 'r') as df:
        df_lines = df.readlines()
        version = df_lines[0].replace('\n', '')
    return version

#################################
# getCPUInfo()
#   Grabs and returns current cpu model and type.
# args:
#   None
# return:
#   List as [type, model].
#################################
def getCPUInfo():
    cpu_info = []
    with open('/proc/cpuinfo', 'r') as df:
        df_lines = df.readlines()
        cpu_type = df_lines[3].split(':')[1].replace('\n', '')
        cpu_model = df_lines[4].split(':')[1].replace('\n', '')
        cpu_info.append(cpu_type)
        cpu_info.append(cpu_model)
    return cpu_info

#################################
# main()
#   Main function for program.
# args:
#   None
# return:
#   None
#################################
def main():
    # Get information
    cpu_info = getCPUInfo()
    kernel_version = getKernelVersion()
    time_since_boot = getTimeSinceBoot()
    time_of_boot = getTimeOfBoot()
    process_count = getProcessCount()
    disk_requests = getDiskRequestCount()

    # Save information
    with open("flunalopez_systemDetails.txt", "w") as df:
        df.write("CPU Type: " + cpu_info[0] + "\n")
        df.write("CPU Model: " + cpu_info[1] + "\n")
        df.write("Kernel Version: " + kernel_version + "\n")
        df.write("Time since boot: " + time_since_boot + " seconds\n")
        df.write("Time of Boot: " + time_of_boot + "\n")
        df.write("Process Count: " + str(process_count) + "\n")
        df.write("Disk Request Count: " + str(disk_requests) + "\n");

# Only run main() if program is a main program.
if __name__ == '__main__':
    main()
