#Author: Tristan Bailey
#Date Created: 2/27/2022
#Last Modified: 2/27/2022
#Class: CS446
#Assignment: PA 1
#File: Python3 program for Part 1

#importing of allowed libraries

#check if this library is ok
from datetime import datetime
import os
import sys
import subprocess

def cpu_model():
    try:
        cpu_file = open("/proc/cpuinfo", 'r')

        cpu_type = ""
        cpu_model = ""
             
        lines = cpu_file.read()
        lines = lines.split("\n")

        cpu_type = lines[1].split(":")
        cpu_model = lines[4].split(":")

        cpu_file.close()
        return([cpu_type[1], cpu_model[1]])

    except:
        print("Failed to open cpuinfo file")
        return("")

def kernal_version():
    try:
        version_file = open("/proc/version", 'r')
        temp = version_file.read()
        version_file.close()

        version_info = temp.split(" ")
        return(version_info[0] + " " + version_info[1] + " " + version_info[2])

    except:
        print("Failed to open version file")
        return("")

#returns an inte representing the seconds since the system was last booted
def sys_time_since_boot():
    try:
        uptime_file = open("/proc/uptime", 'r')
        temp = uptime_file.read()
        uptime_file.close()

        #gets the first number in uptime file which is the time since last boot
        #which is in seconds
        times = temp.split(" ")
        return(float(times[0]))

    except:
        print("Failed to open uptime file")
        return(-1)

def last_boot_time():
    try:
        stat_file = open("/proc/stat", 'r')
        lines = stat_file.read()
        stat_file.close()

        lines = lines.replace("\n", " ")
        lines = lines.split(" ")
        
        b_time = 0
        for i in range(len(lines)):
            if(lines[i] == "btime"):
                b_time = int(lines[i + 1])

        #convert from unix epoch to regular date format
        boot_date = datetime.fromtimestamp(b_time)
        return(boot_date)

    except:
        print("Failed to open stat file")
        return(-1)

def num_proc_run():
    try:
        stat_file = open("/proc/stat", 'r')
        lines = stat_file.read()
        stat_file.close()

        lines = lines.replace("\n", " ")
        lines = lines.split(" ")
        
        processes = 0
        for i in range(len(lines)):
            if(lines[i] == "processes"):
                processes = int(lines[i + 1])

        #convert from unix epoch to regular date format
        return(processes)

    except:
        print("Failed to open stat file")
        return(-1)

def num_disk_reqs():
    try:
        #open, read, and close diskstats file
        disk_file = open("/proc/diskstats", 'r')
        lines = disk_file.readlines()
        disk_file.close()

        total = 0
        #go through each line from the diskstats file
        for line in lines:
            tokenize = line.split(" ")
            #removes all empty strings from the tokenized list
            strings = []
            for string in tokenize:
                if (string != ""):
                    strings.append(string)
            total = total + int(strings[3]) + int(strings[7])
        #return the totla number of disk read and writes            
        return total

    except:
        print("Failed to open diskstat file")

try:

    my_file = open("tristanbailey_systemDetails.txt", "w")

    cpu_info = cpu_model()
    my_file.write("CPU type: " + cpu_info[0] + "\t")
    my_file.write("CPU Model: " + cpu_info[1] + "\n")

    my_file.write("Kernal Version: " + kernal_version() + "\n")

    my_file.write("Time Since Last Boot: " + str(sys_time_since_boot()) + " seconds" + "\n")

    my_file.write("Booted: " + str(last_boot_time()) + "\n")

    my_file.write("Disk Requests Made(successful writes + reads): " + str(num_disk_reqs()) + "\n")

    my_file.write("Processes Created Since Last Boot: " + str(num_proc_run()) + "\n")

    my_file.close()

except:
    print("Could not create/write to tristanbailey_systemDetails.txt")