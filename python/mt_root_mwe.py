#!/bin/env python3
__author__ = "Philipp Oleynik"
"""
This demonstrates how to use the uproot package and multithreading to process multiple files generated 
by the instrument-simulation software. The example below does not compute anything; it just sleeps random delay.
One should rewrite the compute_response function with something that makes data extraction and analysis.
The primary target for this script and its possible derivatives is high-performance computing on a cluster.
It may or may not be faster on a conventional PC than the multiprocessing.pool.Pool, which is easier to use.
"""

from multiprocessing import Process, Queue
import time
import random
import os
import numpy as np
import uproot


MAXTHREADS = 3


def process_sleep(thread_str = None, data_queue = None):
    print(thread_str)
    sleep_time = random.random() * 10
    time.sleep(sleep_time)
    data_queue.put([sleep_time, 0])
    data_queue.close()


def compute_response(*, infile = None, data_queue = None): # A stub analyzing the data in a ROOT file
    if os.path.getsize(infile):
        print(f"Opening {infile}...")
        f = uproot.reading.open(infile)
        detector_data = f["Detector Data"]
        metadata = f['Simulation Data']
        particle_num_total = int(np.sum(metadata['Particle Number'].array(library = 'np')))
        radiation_area = (metadata['Radius'].array(library = 'np')[0] / 10.0) ** 2 * 4 * np.pi
        det_energy_deposit = detector_data['Det_LV_Edep_MeV'].array(library = 'np')

        pass   # Insert your code here
        pass
        pass

        data_queue.put([0, 0, 0]) # replace with analysis results
        data_queue.close()
        pass


def main( ):
    threads = []
    queues = []
    thread_index = 0
    for i in range(8):
        queue = Queue()
        process = Process(target = process_sleep,
                          kwargs = { 'thread_str': "Thread " + str(i),
                                     'data_queue': queue })
        process.start()
        threads.append(process)
        queues.append(queue)
        thread_index += 1

        while thread_index >= MAXTHREADS:
            for idx, queue in enumerate(queues):
                if not queue.empty():
                    (sleep_time, _) = queue.get()
                    print(f"A thread slept for {sleep_time} seconds.")
                    queues.pop(idx)
                    threads[idx].join()
                    threads.pop(idx)
                    thread_index -= 1
                else:
                    time.sleep(0.1)

    print("Joining the last batch.")
    for queue in queues:
        (sleep_time, _) = queue.get()
        print(f"A thread slept for {sleep_time} seconds.")
    for process in threads:
        process.join()
    return


if __name__ == "__main__":
    main()
