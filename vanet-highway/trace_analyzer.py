#! /usr/bin/python
# A helper script for analyzing VCrash trace data
#
# @author: Kyle Benson

import argparse
from trace import *
'''
def do_plots(picks,args):
'''
   # Plot requested figures, if any.
'''
    nplots = (0 if args.time is None else 1) + (0 if args.data is None else len(args.data))
    if nplots < 1:
        return
    from matplotlib.pyplot import hist,subplot,subplots_adjust
    import matplotlib.pyplot as plt
    from math import ceil,sqrt

    npicks = len(picks)
    #try for smart subplot arrangements
    if nplots > 3:
        nrows = int(sqrt(nplots)+0.5)
        ncols = ceil(nplots/float(nrows))
    else:
        nrows = 1
        ncols = nplots

    next_axes = 1
    if args.time:
        resolution = args.time
        subplot(nrows,ncols,next_axes)
        times = gdata.get_times(picks,resolution)
        hist(times,resolution)
        plt.title("Time histogram in resolution of %dms." % resolution)
        plt.xlabel("Time offset from first event.")
        plt.ylabel("Count")
        plt.plot()
        next_axes += 1
        
    if args.data:
        arg = args.data
        xvalues,yvalues,zvalues = gdata.xvalues,gdata.yvalues,gdata.zvalues
        resolution = 50
        if 'a' in arg:
            xvalues = gdata.get_xvalues(picks)
            yvalues = gdata.get_yvalues(picks)
            zvalues = gdata.get_zvalues(picks)
            subplot(nrows,ncols,next_axes)
            hist([xvalues,yvalues,zvalues],resolution)
            plt.title("Histogram of x-axis PGA readings")
            plt.xlabel("PGA")
            plt.ylabel("Count")
            plt.plot()
            next_axes += 1
        if 'x' in arg:
            xvalues = gdata.get_xvalues(picks)
            subplot(nrows,ncols,next_axes)
            hist(xvalues,resolution)
            plt.title("Histogram of x-axis PGA readings")
            plt.xlabel("PGA (m/s^2)")
            plt.ylabel("Count")
            plt.plot()
            next_axes += 1
        if 'y' in arg:
            yvalues = gdata.get_yvalues(picks)
            subplot(nrows,ncols,next_axes)
            hist(yvalues,resolution)
            plt.title("Histogram of y-axis PGA readings")
            plt.xlabel("PGA (m/s^2)")
            plt.ylabel("Count")
            plt.plot()
            next_axes += 1
        if 'z' in arg:
            zvalues = gdata.get_zvalues(picks)
            subplot(nrows,ncols,next_axes)
            hist(zvalues,resolution)
            plt.title("Histogram of z-axis PGA readings")
            plt.xlabel("PGA (m/s^2)")
            plt.ylabel("Count")
            plt.plot()
            next_axes += 1

    plt.show()
'''    

##################################################################################
#################      ARGUMENTS       ###########################################
##################################################################################

parser = argparse.ArgumentParser(description='A helper script for analyzing VCrash traces and visualizing the data.',
                                 formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('--trace-file','-t', '--traces', type=str, nargs=1, default='../networkTrace.csv',
                    help='file from which to read message trace data')
parser.add_argument('--vehicle-file', '--vehicles', type=str, nargs=1, default='../vehicleTrace.csv',
                    help='file from which to read vehicle trace data')
parser.add_argument('--density','-d',action='store_true',
                    help='display penetration of message in % of vehicles that receive it over time as several plots\
where the traffic density of each trace file is uesd')
parser.add_argument('--penetration','-pen','-p',action='store_true',
                    help='display penetration of message in % of vehicles that receive it over time');

args = parser.parse_args()

################################# MAIN ####################################


with open(args.trace_file) as f:
    traces = f.readlines()

with open(args.vehicle_file) as f:
    vehicle_traces = f.readlines()

print "Total # of trace lines: %d" % len(traces)

forwards = [DataTrace(x) for x in traces if x.startswith('Packet forwarded.')]
ignores = [DataTrace(x) for x in traces if x.startswith('Ignoring packet.')]
created = [DataTrace(x) for x in traces if x.startswith('Packet created.')]

vehicles = {}
for v in (VehicleTrace(x) for x in vehicle_traces):
    if v.id not in vehicles:
        vehicles[v.id] = 0

print "there were %d forwards, %d ignores, and %d vehicles total" % (len(forwards), len(ignores), len(vehicles))

#do_plots(created,forwards,ignores,args)
