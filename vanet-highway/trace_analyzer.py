#! /usr/bin/python
# A helper script for analyzing VCrash trace data
#
# @author: Kyle Benson

import argparse
import sys
from trace import *

def create_timesteps(msgs,resolution):
    #messages should be ordered in time
    start_time = msgs[0].time
    end_time = msgs[-1].time
    times = range(start_time/resolution, end_time/resolution)
    return times

##################################################################################
#################      ARGUMENTS       ###########################################
##################################################################################

parser = argparse.ArgumentParser(description='A helper script for analyzing VCrash traces and visualizing the data.\
\nExample usage:  ./trace_analyzer.py -t ../networkTrace1.csv ../networkTrace2.csv --vehicles ../vehicleTrace1.csv ../vehicleTrace2.csv -l firstLabel secondLabel -p -n',formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('--trace-files','-t', '--traces', nargs='+', default=['../networkTrace.csv'],metavar='trace_files',
                    help='file from which to read message trace data')
parser.add_argument('--vehicle-files', '--vehicles', nargs='+', default=['../vehicleTrace.csv'],metavar='vehicle_files',
                    help='file from which to read vehicle trace data')
parser.add_argument('--penetration','-pen','-p',action='store_true',
                    help='display penetration of message in percent of vehicles that receive it over time')
parser.add_argument('--num-packets','-n',action='store_true',
                    help='display total packets sent in each timestep over time')
parser.add_argument('--labels','-l',nargs='+',
                    help='names given to data sets contained in specified files')

args = parser.parse_args()

if len(args.trace_files) != len(args.vehicle_files):
    print "The number of message and vehicle trace files should be the same!\n"
    sys.exit(-1)

################################# MAIN ####################################

msg_traces = []
for fname in args.trace_files:
    with open(fname) as f:
        msg_traces.append(f.readlines())

veh_traces = []
for fname in args.vehicle_files:
    with open(fname) as f:
        veh_traces.append(f.readlines())

#print "Total # of trace lines: %d" % len()
messages = []
for traces in msg_traces:
    forwarded = [Message(x) for x in traces if x.startswith('Packet forwarded.')]
    ignored = [Message(x) for x in traces if x.startswith('Ignoring packet.')]
    created = [Message(x) for x in traces if x.startswith('Packet created.')]
    all_msgs = [Message(x) for x in traces]
    messages.append(MessageTraceData(all_msgs,forwarded,ignored,created))
    
vehicles = []
for traces in veh_traces:
    vehs = {}
    for v in (Vehicle(x.strip()) for x in traces):
        if v.id not in vehs:
            vehs[v.id] = 1
        vehicles.append(vehs)

nvehicles = [len(v) for v in vehicles]
#print "there were %d forwards, %d ignores, and %d vehicles total" % (len(forwards), len(ignores), len(vehicles))

###################################  PLOTS  ################################

nplots = (0 if args.penetration is False else 1) + (0 if args.num_packets is False else len(messages))
if nplots == 0:
    print "No plots requested."
    sys.exit(0)

from matplotlib.pyplot import subplot,plot,bar,legend
import matplotlib.pyplot as plt
#from math import ceil,sqrt

#try for smart subplot arrangements
if nplots > 3:
    nrows = int(sqrt(nplots)+0.5)
    ncols = ceil(nplots/float(nrows))
else:
    nrows = 1
    ncols = nplots

next_axes = 1
if args.penetration:
    data_to_plot = []
    for (i,msgs) in enumerate(messages):
        vehs_notified = {}

        #vehicles always forward message once they see it
        msgs = msgs.forwarded
        resolution = 100000000 #0.1 second resolution
        times = create_timesteps(msgs,resolution)

        percentages = []
        for t in times:
            while msgs[0].time < t*resolution:
                vehs_notified[msgs[0].id] = vehs_notified.get(msgs[0].id, 0) + 1
                msgs = msgs[1:]

            percentages.append((len(vehs_notified) +1)/float(nvehicles[i]) *100)

        min_time = times[0]
        times = [(t-min_time)/10 for t in times]

        data_to_plot.append(times)
        data_to_plot.append(percentages)

    subplot(nrows,ncols,next_axes)
    plot(*data_to_plot)

    if args.labels is not None:
        legend(args.labels, loc='lower right')

    plt.title("Message coverage over time")
    plt.xlabel("Time offset from crash event (sec)")
    plt.ylabel("% of vehicles that have received the message")
    plt.plot()
    next_axes += 1

if args.num_packets:
    for (i,msgs) in enumerate(messages):
        msgs = sorted(msgs.forwarded + msgs.created)
        resolution = 1000000000 #second resolution
        times = create_timesteps(msgs,resolution)

        npackets = []
        for t in times:
            npacs = 0
            while msgs[0].time < t*resolution:
                npacs += 1
                msgs = msgs[1:]

            npackets.append(npacs)

        min_time = times[0]
        times = [t-min_time for t in times]

        subplot(nrows,ncols,next_axes)
        bar(times, npackets)
        plt.title("Number of packets sent over time")
        plt.xlabel("Time since crash event (sec)")
        plt.ylabel("Packets sent per second")
        plt.plot()
        next_axes += 1

plt.show()
