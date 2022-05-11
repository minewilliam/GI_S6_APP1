#!/usr/bin/env python3

from multiprocessing import Pool
import os
import subprocess
import sys

NUM_PROCESSES = 1

def run_process(task_desc):
    p = subprocess.Popen(["./asset_conv", "-t", "{0}".format(sys.argv[2])], stdin=subprocess.PIPE)
    p.communicate(input=task_desc.encode())



if __name__ == "__main__":
    nb_processes = int(sys.argv[1])
    print("Starting {0} processes with {1} threads each".format(sys.argv[1], sys.argv[2]))
    pool = Pool(nb_processes)

    queues = [ [] for i in range(nb_processes) ]
    i = 0
    for line in sys.stdin:
        queues[i].append(line)
        i = (i + 1) % nb_processes

    task_descs = []
    for q in queues:
        task_desc = '\n'.join(q)
        task_descs.append(task_desc)

    pool.map(run_process, task_descs)
    


