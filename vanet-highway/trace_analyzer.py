#! /usr/bin/python
# A helper script for analyzing VCrash trace data
#
# @author: Kyle Benson

import argparse

def do_plots(picks,args):
    '''
    Plot requested figures, if any.
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
    

##################################################################################
#################      ARGUMENTS       ###########################################
##################################################################################

parser = argparse.ArgumentParser(description='A helper script for analyzing VCrash traces and visualizing the data.',formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('files', metavar='file', type=str, nargs='+',
                    help='files from which to read trace data')
parser.add_argument('--time',action='store',type=int,const=100,nargs='?',
                    help='display histogram of pick times with optional resolution integer value (measured in 1ms increments)')
parser.add_argument('--data','-d',action='store',type=str,nargs='?',const='xyz',
                    help='''\
display histograms for the sensor data, with optional arguments being a subset of
'xyza' where each letter corresponds to whether the readings for that axis should
be plotted, with 'a' meaning combine all 3 on one plot
''')
parser.add_argument('--density','-d',action='store_true',
                    help='display penetration of message in % of vehicles that receive it over time as several plots where the traffic density of each trace file is uesd')
parser.add_argument('--penetration','-pen','-p',action='store_true',
                    help='display penetration of message in % of vehicles that receive it over time');

args = parser.parse_args()

################################# MAIN ####################################

traces = []
for f in args.files:
    traces.extend(f.readLines())

print "Total # of trace lines: %d" % len(traces)

forwards = [x for x in traces if x.

do_plots(picks,args)
