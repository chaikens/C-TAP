#!/usr/bin/env python
# coding: utf-8

# In[ ]:


def helpVis():
    print(""" This program gets the data from and puts plots in a RESULTS dir.
After import Phase1bVisualizer as vs
Veuillez coder, s'il vous plait:
vs.SetResultsDir(_)
vs.SetMovieName(_)
[ vs.SetRunNumber(_) ] 
   Use _="" if runs aren't numbered. Omit or _=lastrunn() to automatically pick largest run number.
vs.GetData()
Then, any numbers of
doit(<start frame number>, <number of frames""")


# In[ ]:


def howto(what="see_frames"):
    """
    How to view the movie frames corresponding to Phase1bVisualizer output:

    This sofware visualizes FLIR's processing of data from frame diffs to the lines in FLIR's .int file.

    These are the frames in the movie input to Phase1a. That movie is NOT the original from the camera.

    When preprocessing is done, these frames belong to the movie in the PP directory.
    They
         (1) result from decimating (and usually scaling) the original movie
         (2) have the original movie frame number drawn in the upper left corner
    The advantages of preprocessing are:
         (1) The decimation and scaling, an expensive ffmpeg operation, can be overlapped in 
             another computer with FLIR processing of already preprocessed movies.
         (2) It is space-wise efficient for FLIR to use the PP movie twice: Input to Phase1a, and
             then to supply the frames selected by Phase1b to go into the baby movie.
         (3) Ffmpeg is called with a draw filter called before the decimate filter to draw the 
             frame number from the original, non-decimated movie in the upper left corner.

    The PP input frame number [ipp] is indexed by the horizontal axis ticks.

    This number can be entered into the small bottom left corner box of our recommended viewer YUView.
    (Fix for YUView minor annoyance: when opening a file, in the file selector window's lower left corner,
    instead of "All supported file format", select "Any files" 

    Frames that comprise the baby movie (with .MOV) suffix are flagged by transparent white columns.
    To view them in the baby movie, look up ipp in the printed list of triples, or call whereinbaby(ipp).
    The baby movie frame number is the third of your found triple, or returned by whereinbaby(ipp).
    """
    #if what = "see_frames":
    print("Read Phase1bVisualizer source or in jupyter, do howto? and read the documentation")


# In[ ]:


import numpy as np
import math
import matplotlib.pyplot as plt
import matplotlib.lines as lines
from scipy import optimize as opt


# In[ ]:


# We save this list instead of just passing it to np.dtype(_) so we can iterate thru the fields of our data type.
Ph1adtList=[('frame', np.int32), 
               ('Rd','<i8'),('Rx',np.int16),('Ry',np.int16),
               ('Gd','<i8'),('Gx',np.int16),('Gy',np.int16),
               ('Bd','<i8'),('Bx',np.int16),('By',np.int16),
               ('rd','<i8'),('rx',np.int16),('ry',np.int16),
               ('gd','<i8'),('gx',np.int16),('gy',np.int16),
               ('bd','<i8'),('bx',np.int16),('by',np.int16),
               ('Rn',np.int32),('Gn',np.int32),('Bn',np.int32),
               ('rn',np.int32),('gn',np.int32),('bn',np.int32),
               ('stn',np.int32)]

Ph1adt = np.dtype(Ph1adtList)
"""
For Phase1a .int file: from intdata

dtype([('frame', '<i4'), 
('Rd', '<i8'), ('Rx', '<i2'), ('Ry', '<i2'), 
('Gd', '<i8'), ('Gx', '<i2'), ('Gy', '<i2'), 
('Bd', '<i8'), ('Bx', '<i2'), ('By', '<i2'), 

('rd', '<i8'), ('rx', '<i2'), ('ry', '<i2'), 
('gd', '<i8'), ('gx', '<i2'), ('gy', '<i2'), PHASE_1b_VERBOSE=yes

('bd', '<i8'), ('bx', '<i2'), ('by', '<i2'),  endl; 

('Rn', '<i4'), ('Gn', '<i4'), ('Bn', '<i4'), 
('rn', '<i4'), ('gn', '<i4'), ('bn', '<i4'), 

('stn', '<i4')])
"""


def makeDiffMats(v):
    PixIntDiffExtremal=np.vstack([v['Rd'],v['Gd'],v['Bd'],-v['rd'],-v['gd'],-v['bd']])
    NsPixInThr=np.vstack([v['Rn'],v['Gn'],v['Bn'],v['rn'],v['gn'],v['bn']])
    NPixInSThr=v['stn']
    return { 'PixIntDiffExtremal':PixIntDiffExtremal, 'NsPixInThr':NsPixInThr, 'NPixInSThr':NPixInSThr}


# In[ ]:


# develop an extension of Ph1adt for --verbose-file output
# 
# We save this list instead of just passing it to np.dtype(_) so we can iterate thru the fields of our data type.
Ph1bVdtList = [('frame', np.int32), 
               ('Rd','<i8'),('Rx',np.int16),('Ry',np.int16),
               ('Gd','<i8'),('Gx',np.int16),('Gy',np.int16),
               ('Bd','<i8'),('Bx',np.int16),('By',np.int16),
               ('rd','<i8'),('rx',np.int16),('ry',np.int16),
               ('gd','<i8'),('gx',np.int16),('gy',np.int16),
               ('bd','<i8'),('bx',np.int16),('by',np.int16),
               ('Rn',np.int32),('Gn',np.int32),('Bn',np.int32),
               ('rn',np.int32),('gn',np.int32),('bn',np.int32),
               ('stn',np.int32),   #NumPixAbvSubThrSum in C++ code
               #end of originals
               ('AbsMax',np.single),     #Vs (verboses) index = 0
               ('AbsStdDev',np.single),  # 1
               ('prob',np.single),       # 2
               ('probGTlevel',np.int16),  # 3
               ('AbsMaxGTMinThr',np.int16),  # 4
               ('AbsMaxLTMaxThr',np.int16),  # 5
               ('NumPixAbvSubThrSumGTMinPix',np.int16),     # 6
               ('NumPixAbvSubThrSumLTMaxPix',np.int16),     # 7
               ('NumPixAbvSubThrSum',np.int32),            # 8
               ('stnGTNumPixAbvThrSumMin',np.int16),        # 9
               ('stnLTNumPixAbvThrSumMax',np.int16),        # 10
               ('SignalTruth', np.int16),                   # 11
               ('NumBools',np.int32),                      # 12
               ('NumBoolsDivFRange',np.single),            # 13
               ('ManyBools',np.int16),                      # 14
               ('GoldE',np.int16),                         # 15     
               ('EvtN',np.int32),                          # 16
                    # Events are numbered beginning at 0
                   ]
Ph1bVdt = np.dtype(Ph1bVdtList)
# Originally bool values are np.int16 because some must be multipled by larger factors for plotting.


# In[ ]:


# We save this list instead of just passing it to np.dtype(_) so we can iterate thru the fields of our data type.
Ph1bVglobaldtList = [
               ('SubThr',np.int16),  #only used in Phase1a                                                       # 0
               ('RewFram',np.int16),('ForFram',np.int16),('FramBeNew',np.int16),            #1, 2, 3
               ('FracYes',np.single),('smallestThr',np.int16),('biggestThr',np.int16),      #4, 5, 6 
                #                                               MaxThr:=biggestThr in Phase1b
               ('smallestPix',np.int16),('biggestPix',np.int16),                            #7,8
               ('SkewGaussAmpl',np.single),('SkewGaussXi',np.single),                       #9, 10
               ('SkewGaussOmega',np.single),('SkewGaussAlpha',np.single),                   #11, 12
               ('level',np.single),                                                         #13
               ("NumPixAbvThrMin",np.int16),("NumPixAbvThrMax",np.int16),                   #14, 15
               ('OverallAverage',np.single),('OverallStdDev',np.single), #of Extremal PixIntDiff vals. #16, 17
               ('forMinThr',np.single),('MinThr',np.int16),                                 #18, 19
               ('GlobalPixSigma',np.single),                                                #20
               ('GmeanForMinPix',np.single),                                                #21
               ('MinPix',np.int16),('MaxPix',np.int16),                                     #22, 23
               ('MainThr',np.int16)                                                         # 24
]
Ph1bVglobaldt =  np.dtype(Ph1bVglobaldtList)

def printglobals(v1bglobaldata):
    i = 0
    print("Variable = value   ( Key output by Phase1b C++ program, please check consistency. )")
    for e in Ph1bVglobaldtList:
        print(f"{e[0]}  =  {v1bglobaldata[e[0]]:.3f}    ({v1bglobalkeydata[i]} )")
        i = i + 1


# In[ ]:


ResultsDir = None
def SetResultsDir(x):
    global ResultsDir
    ResultsDir=x+"/"
MovieName = None
def SetMovieName(x):
    global MovieName
    MovieName=x

import glob
import fnmatch
import re
def lastrunn():
    glret=glob.glob(MovieName+'.v1b.*', root_dir=ResultsDir)
    fmatret=fnmatch.filter(glret,'*.v1b.[1-9]')+fnmatch.filter(glret,'*.v1b.[1-9][0-9]')
    nlist=[]
    for n in fmatret:
        matob=re.match(".*\\.([1-9][0-9]*)",n)
        nlist.append(matob.group(1))

    nlist.sort(key=int)
    print("Automatic last run number " + nlist[-1] + " will be used.")
    return nlist[-1]

RunNumber = None
def SetRunNumber(x):
    global RunNumber
    RunNumber=x

#Used to overlay a transparent white bar for frames in the events output by Phase1b,
#and obtain the output movie frame number from the EvtN array
def notneg(x):
    if x < 0:
        return 0
    else:
        return 1

notnegv = np.vectorize(notneg)

def GetData(): #common for any frame number range
    global IntFilePath
    global v1bFilePath
    global v1bkeyFilePath
    global v1bglobalFilePath
    global v1bglobalkeyFilePath
    global intdata
    global outsfn
    global isinbaby
    global newevtfrm, oldevtfrm

    global v1bdata, v1bdatakey, v1bglobaldata, v1bglobalkeydata
    v1bdata=None        #frame by frame phase 1b used and intermediate data
    v1bdatakey=None     #its keys
    v1bglobaldata=None  #settings and data computed from all frames and then used
    v1bglobalkeydata=None #its keys

    if (ResultsDir is None) :
        print( "Please do SetResultsDir( your results dir )")
        PleaseSetResultsDir() #call an undefined function to make Jupyter report an error and stop here.

    if (MovieName is None): 
        print( "Please do SetMovieName( your movie name )")
        PleaseSetMovieName() #call an undefined function to make Jupyter report an error and stop here.

    MovieNamePath=ResultsDir+MovieName

    global RunNumberStr #Used to open data files and label the plots.
    if RunNumber != "":
        #RunNumber was set by SetRunNumber(a number) or SetRunNumber was not called.
        if RunNumber is None:
            #activate automatic selection of the greatest (i.e., last) run number
            SetRunNumber(lastrunn())
        RunNumberStr = "." + str(RunNumber)
    else:  
        #.int and .v1b. file names do not end with run numbers
        # The user can program this by SetRunNumber("")
        RunNumberStr = ""

    IntFilePath=ResultsDir+MovieName+".int"+RunNumberStr
    v1bFilePath=MovieNamePath+".v1b"+RunNumberStr
    v1bkeyFilePath=MovieNamePath+".v1b.key"+RunNumberStr
    v1bglobalFilePath=MovieNamePath+".v1b.global"+RunNumberStr
    v1bglobalkeyFilePath=MovieNamePath+".v1b.global.key"+RunNumberStr

    intdata=np.loadtxt(IntFilePath,converters=float,dtype=Ph1adt)
    v1bglobaldata=np.genfromtxt(v1bglobalFilePath,unpack=False,dtype=Ph1bVglobaldt,delimiter=",",missing_values="",filling_values=0.0)
    v1bdata=np.genfromtxt(v1bFilePath,unpack=False,dtype=Ph1bVdt,delimiter=",",missing_values="",filling_values=0.0)
    v1bglobalkeydata=np.loadtxt(v1bglobalkeyFilePath,dtype=str,delimiter=",")

    #For those diff frames [i] reported in .out as in events, and the baby movie
    #maker puts frame [i] in the movie, isinbaby[i]==1 and 
    #outfn[i] is the index of that frame in the
    isinbaby = notnegv(v1bdata['EvtN'])
    outsfn = isinbaby.cumsum()

    newevtfrm = np.zeros_like(v1bdata['EvtN'],dtype=int)
    oldevtfrm = np.zeros_like(v1bdata['EvtN'],dtype=int)

    fi = 0
    #find first event
    while v1bdata['EvtN'][fi] < 0:
        fi += 1
    newevtfrm[fi] = 1
    oldEvtN = 0
    #do the rest
    for i in range(fi+1,len(v1bdata['EvtN'])):
        if v1bdata['EvtN'][i] == oldEvtN:
            oldevtfrm[i] = 1
        elif v1bdata['EvtN'][i] > 0:
            newevtfrm[i] = 1
            oldEvtN = v1bdata['EvtN'][i]

    #put our SkewGauss params into our skewgauss function, and plot it
    global SkewGauss
    SkewGauss=np.array([v1bglobaldata['SkewGaussAmpl'],
                        v1bglobaldata['SkewGaussXi'],
                        v1bglobaldata['SkewGaussOmega'], 
                        v1bglobaldata['SkewGaussAlpha']])
    plotprob(0,10,v1bglobaldata['level'])


# In[ ]:


#Later, getdata() replaces these with params from 1b global data, and calls for the plot.
SkewGaussAmpl= 0.633
SkewGaussXi= 1.97
SkewGaussOmega= 1.89
SkewGaussAlpha= 2.5

SkewGauss=np.array([SkewGaussAmpl, SkewGaussXi, SkewGaussOmega, SkewGaussAlpha])
def SG(AbsStdDev):
    ans=1. - SkewGauss[0]*math.exp( -0.5*(AbsStdDev-SkewGauss[1])*(AbsStdDev-SkewGauss[1])
	    /(SkewGauss[2]*SkewGauss[2]))*( 1. + math.erf(SkewGauss[3]*(AbsStdDev-SkewGauss[1])/(SkewGauss[2]*math.sqrt(2.))) )
    return (ans)
vSG=np.vectorize(SG)

def mysdranges(level):
    #We use scipy.optimize.root to find two roots of--
    fun = lambda sd: (vSG(sd) - level)
    #so we can show the std-dev values in included in some cases of event detection.

    xtry = 0.0
    while fun(xtry) > 0.0:
        xtry = xtry + 0.2
    sol1 = opt.root(fun, xtry)
    #print(sol1)
    xtry = sol1.x
    while fun(xtry) <= 0.0:
        xtry = xtry + 0.2
    sol2 = opt.root(fun, xtry)
    #print(sol2)
    return [sol1.x, sol2.x]

def plotprob(start,finish,level=0):
    fig, ax = plt.subplots()
    xs=np.linspace(start,finish)
    ys=vSG(xs)
    #print(xs)
    #print(ys)
    ax.plot(xs,ys, label='1-(skewGauss density)')
    if level > 0:
        ax.hlines(level,start,finish,label='level',color='red')
        [sdlo, sdhi] = mysdranges(level)
        ax.vlines(sdlo,0,1.0)
        ax.vlines(sdhi,0,1.0)
    ax.legend()
    #plt.show()
    plt.savefig(ResultsDir+"ProbFunction.jpg")
    if __name__ == "__main__" :
        plt.show(block=False)
#for testing
#plotprob(0.0,20,0.5)
#plotprob(0.0,20,0.98)


# In[ ]:


def plotsetup(fns, w, movn):
    fig, ax = plt.subplots(figsize=(14,10.25))
    ax.set_title(movn)
    ax.set_facecolor('black')
    plt.xticks(np.arange(fns,fns+w,w/20))
    return fig, ax


# In[ ]:


#for plotting Differences
PIDNames =['Rd','Gd','Bd','rd','gd','bd'] #d for diffs, CapRGB for extr. Pos diffs, lcrgb for Neg diffs.
PIDColor={ 'Rd' : (1,0,0), 'Gd' : (0,1,0), 'Bd' : (0,0,1),
    'rd' : (0,1,1), 'gd' : (1,0,1), 'bd' : (1,1,0) }
def plotDiffs(fns, w, movn):
    """
    fns : starting frame diff num
    w   : number of frame diffs (width)
    movn: movie name to label the plot
    """

    fig, ax = plotsetup(fns, w, movn+RunNumberStr+" Pix Color Intensity Differences by frame pair")
    frseph=1 #frame separators (vertical line) heights, max data y-values to be computed, ensure bottom is 0.

    #Plot the 6 extremal pixel-color-intensity differences for each frame difference.
    for name in PIDNames[0:3]:  
        ax.scatter(intdata['frame'][fns-1:fns-1+w],intdata[name][fns-1:fns-1+w],color=PIDColor[name],alpha=0.6,s=50)
        frseph=max(frseph,intdata[name][fns-1:fns-1+w].max())
    for name in PIDNames[3:6]:
        fresph=max(frseph,intdata[name][fns-1:fns-1+w].max())
        ax.scatter(intdata['frame'][fns-1:fns-1+w],-intdata[name][fns-1:fns-1+w],color=PIDColor[name],alpha=0.8,s=15)

    Ms=makeDiffMats(intdata)
    PIDE=Ms['PixIntDiffExtremal']
    def PIDEstd(PIDE,n):
        return PIDE[:,n].std(ddof=1)
    def PIDEmean(PIDE,n):
        return PIDE[:,n].mean()
    def funstd(n):
        return PIDEstd(PIDE,n-1)
    def funmean(n):
        return PIDEmean(PIDE,n-1)
    vfunstd=np.vectorize(funstd)
    vfunmean=np.vectorize(funmean) 

    xes=np.arange(fns,fns+w)

    ax.hlines(v1bglobaldata['biggestThr'],fns,fns-1+w,label='MaxThr:=biggestThr param',color='green')
    frseph=max(frseph,v1bglobaldata['biggestThr'])

    ax.hlines(v1bglobaldata['smallestThr'],fns,fns-1+w,label='smallestThr(smtThr) param',color='blue')
    frseph=max(frseph,v1bglobaldata['smallestThr'])

    ax.hlines(v1bglobaldata['MinThr'],fns,fns-1+w,label='MinThr(overall avg,std)but>=smtThr',color='red')
    frseph=max(frseph,v1bglobaldata['smallestThr'])

    yes=vfunstd(xes)*2
    ax.plot(xes,yes,label='std*2')
    frseph=max(frseph,yes.max())

    [sdlo,sdhi]=mysdranges(v1bglobaldata['level'])
    ax.hlines(sdhi*2,fns,fns-1+w,label='prob>level if stdev>upper line')
    ax.hlines(sdlo*2,fns,fns-1+w,label='prob>level if stdev<lower line')

    yes=vfunmean(xes)
    ax.plot(xes,yes,label='mean')

    ax.plot(v1bdata['frame'][fns-1:fns-1+w],v1bdata['AbsMax'][fns-1:fns-1+w],label='AbsMax')
    frseph=max(frseph,v1bdata['AbsMax'][fns-1:fns-1+w].max())


    #We no longer plot the "prob[i]" because we now plot 
    #the ranges for std-dev for which prob[i]>level
    #yes=vSG(vfunstd(xes))*100
    #ax.plot(xes,yes,label='prob*100')

    #dont plot these here because they are counts
    #NPixInSThr=Ms['NPixInSThr']
    #ax.plot(xes,NPixInSThr[fns-1:fns-1+w],label="NinSubThr")

    frseph=math.floor(1+(frseph)*1.1)  #leave 10% empty at top, bott is zero.

    frseph=frseph*1.05 #more space for boolean reports

    for i in range(fns-1,fns+w):
        line=lines.Line2D([i+0.5,i+0.5],   [0.0,frseph], color='white',linewidth=0.25)
        ax.add_line(line)

    #Highlight diff frames contained in events--pink for the first of a new event, white for the rest
    #ax.bar(v1bdata['frame'][fns-1:fns+w],frseph*notnegv(v1bdata['EvtN'][fns-1:fns+w]),alpha=0.25,color='white')
    ax.bar(v1bdata['frame'][fns-1:fns-1+w], frseph*newevtfrm[fns-1:fns-1+w],alpha=0.25,color='pink')
    ax.bar(v1bdata['frame'][fns-1:fns-1+w], frseph*oldevtfrm[fns-1:fns-1+w],alpha=0.25,color='white')

    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],frseph*v1bdata['SignalTruth'][fns-1:fns-1+w],marker='s',s=6,color='white',label='SignalTruth')
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.99)*v1bdata['GoldE'][fns-1:fns-1+w],marker='^',s=6,color='white',label='GoldE')

    # Case 1: probGTlevel &&
    # AbsMaxGTMinThr AbsMaxLTMaxThr NumPixAbvSubThrSumGTMinPix NumPixAbvSubThrSumLTMaxPix
    name='probGTlevel'
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.98)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='yellow',label=name)
    name='AbsMaxGTMinThr'
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.97)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='yellow',label=name)
    name='AbsMaxLTMaxThr' 
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.96)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='yellow',label=name)
    name='NumPixAbvSubThrSumGTMinPix' 
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.95)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='yellow',label=name)
    name='NumPixAbvSubThrSumLTMaxPix'
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.94)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='yellow',label=name )

    # Case 4: (2 and 3 are for obsolete B1,3,4 cameras, ignore)
    # stnGTNumPixAbvThrSumMin && stnLTNumPixAbvThrSumMax
    name='stnGTNumPixAbvThrSumMin'
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.93)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='orange',label=name )
    name='stnLTNumPixAbvThrSumMax'
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.92)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='orange',label=name )

    #Visualize values of SubThr and MainThr
    goodleftplace=(v1bdata['frame'][fns-1])+0.98*w
    goodupperplace=frseph*0.8
    ax.vlines(goodleftplace,goodupperplace-v1bglobaldata['SubThr'],goodupperplace,label='SubThr=height of vertical')
    ax.vlines(goodleftplace+0.01*w,goodupperplace-v1bglobaldata['MainThr'],goodupperplace,label='MainThr=height of vertical',color='magenta')

    ax.legend(title="Pix Color Intensities",bbox_to_anchor=(0.95, 0.83))

    plt.savefig(ResultsDir+movn+str(fns)+"-"+str(fns-1+w)+".tiff")
    if __name__ == "__main__" :
        plt.show(block=False)


# In[ ]:


#for plotting Counts, numbers
PCntNames=['Rn','Gn','Bn','rn','gn','bn'] #n--numbers, ie. counts of pix w/ intens within thresholds of an extreme
PCntColor={ 'Rn' : (1,0,0), 'Gn' : (0,1,0), 'Bn' : (0,0,1),
    'rn' : (0,1,1), 'gn' : (1,0,1), 'bn' : (1,1,0) }
def plotCounts(fns, w, movn):
    """
    fns : starting frame diff num
    w   : number of frame diffs (width)
    movn: movie name to label the plot
    """
    fig, ax = plotsetup(fns, w, movn+RunNumberStr+" Counts of intensity differences under conditions, by frame pair")
    frseph=1  #frame separators (vertical line) heights, max data y-values to be computed, ensure bottom is 0.

    def PIDEstd(PIDE,n):
        return PIDE[:,n].std(ddof=1)

    def PIDEmean(PIDE,n):
        return PIDE[:,n].mean()
    Ms=makeDiffMats(intdata)
    PIDE=Ms['PixIntDiffExtremal']

    for name in PCntNames[0:3]: 
        ax.scatter(intdata['frame'][fns-1:fns-1+w],intdata[name][fns-1:fns-1+w],color=PCntColor[name],alpha=0.6,s=50)
    for name in PCntNames[3:6]:
        ax.scatter(intdata['frame'][fns-1:fns-1+w],intdata[name][fns-1:fns-1+w],color=PCntColor[name],alpha=0.8,s=15)

    xes=np.arange(fns,fns+w)
    """
    Ms=makeDiffMats(intdata)
    PIDE=Ms['PixIntDiffExtremal']
    def funstd(n):
        return PIDEstd(PIDE,n-1)
    def funmean(n):
        return PIDEmean(PIDE,n-1)
    vfunstd=np.vectorize(funstd)
    vfunmean=np.vectorize(funmean) 
    yes=vfunstd(xes)*2
    ax.plot(xes,yes,label='std*2')
    yes=vfunmean(xes)
    ax.plot(xes,yes,label='mean')
    #yes=vSG(vfunstd(xes))*100
    #ax.plot(xes,yes,label='prob*100')
    """
    NPixInSThr=Ms['NPixInSThr']
    frseph=NPixInSThr[fns-1:fns-1+w].max()

    ax.plot(xes,NPixInSThr[fns-1:fns-1+w],label="NinSubThr for Case1cj2")

    yones=np.ones(w)
    yes=yones*v1bglobaldata['MinPix']
    ax.plot(xes,yes,label='MinPix calc for Case1cj2')

    yes=yones*v1bglobaldata['MaxPix']
    ax.plot(xes,yes,label='MaxPix calc for Case1cj2')
    frseph=max(frseph,v1bglobaldata['MaxPix'])

    yes=yones*v1bglobaldata['NumPixAbvThrMin']
    ax.plot(xes,yes,label='NumPixAbvThrMin param for Case2')

    yes=yones*v1bglobaldata['NumPixAbvThrMax']
    ax.plot(xes,yes,label='NumPixAbvThrMax param for Case2')
    frseph=max(frseph,v1bglobaldata['NumPixAbvThrMax'])

    frseph=frseph*1.05 #more space for boolean reports  
    frseph=math.floor(1+(frseph)*1.1) #leave 10% empty at top, ensure bott is 0.
    for i in range(fns-1,fns+w):
        line=lines.Line2D([i+0.5,i+0.5],   [0.0,frseph], color='white',linewidth=0.25)
        ax.add_line(line)

    #Highlight diff frames contained in events--pink for the first of a new event, white for the rest
    #ax.bar(v1bdata['frame'][fns-1:fns+w],frseph*notnegv(v1bdata['EvtN'][fns-1:fns+w]),alpha=0.25,color='white')
    ax.bar(v1bdata['frame'][fns-1:fns-1+w], frseph*newevtfrm[fns-1:fns-1+w],alpha=0.25,color='pink')
    ax.bar(v1bdata['frame'][fns-1:fns-1+w], frseph*oldevtfrm[fns-1:fns-1+w],alpha=0.25,color='white')

    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],frseph*v1bdata['SignalTruth'][fns-1:fns-1+w],marker='s',s=6,color='white',label='SignalTruth')
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.99)*v1bdata['GoldE'][fns-1:fns-1+w],marker='^',s=6,color='white',label='GoldE')

    # Case 1: probGTlevel &&
    # AbsMaxGTMinThr AbsMaxLTMaxThr NumPixAbvSubThrSumGTMinPix NumPixAbvSubThrSumLTMaxPix
    name='probGTlevel'
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.98)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='yellow',label=name)
    name='AbsMaxGTMinThr'
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.97)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='yellow',label=name)
    name='AbsMaxLTMaxThr' 
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.96)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='yellow',label=name)
    name='NumPixAbvSubThrSumGTMinPix' 
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.95)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='yellow',label=name)
    name='NumPixAbvSubThrSumLTMaxPix'
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.94)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='yellow',label=name )

    # Case 4: (2 and 3 are for obsolete B1,3,4 cameras, ignore)
    # stnGTNumPixAbvThrSumMin && stnLTNumPixAbvThrSumMax
    name='stnGTNumPixAbvThrSumMin'
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.93)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='orange',label=name+' for Case2' )
    name='stnLTNumPixAbvThrSumMax'
    ax.scatter(v1bdata['frame'][fns-1:fns-1+w],(frseph*0.92)*v1bdata[name][fns-1:fns-1+w],marker='+',s=6,color='orange',label=name+' for Case2' )

    ax.legend(title='Counts', bbox_to_anchor=(0.95, 0.83))


    plt.savefig(ResultsDir+movn+str(fns)+"-"+str(fns+w)+"counts.tiff")
    if __name__ == "__main__" :
        plt.show(block=True)


# In[ ]:


def whereinbaby(i):
    if v1bdata['EvtN'][i] < 0:
        print(f"Diff frame [{i}] is not in an event.")
        return None
    else:
        return int(outsfn[i])-1

def printEvtNandbabyfn(fns,w=200,limit=200):
    print("[frame shown, Event Number, Frame in output movie]")
    outcount=0
    for i in range(fns,fns+w):
        if isinbaby[i] == 1:
            outcount += 1
            if outcount > limit:
                print("Listing limited by limit=" + str(limit),"Use vs.whereinbaby(frameno).\n")
                return None
            print([i, int(v1bdata['EvtN'][i]),int(outsfn[i])-1])
            #         int so print doesn't print np.int32(..) etc.
    #Newlines to separate this run from any next one.
    print("")


# In[ ]:


def printstuff(fns,w):
    """ Print global numeric data input parameters and calculations """
    print(f"Starting your next visualization: {MovieName} {RunNumberStr} diff frames {fns} to {fns+w}")
    print("Global Data pertaining to the whole movie:")

    printglobals(v1bglobaldata)

    print(\
"""\nUnits--
PID:Pixel Intensity Difference
NPC:Number of Cases of a Pixel state or other Predicate about Pixels
FD:Frame Difference Index for Frame[i+1]-Frame[i]""")

    print("\nHow it is used to select interesting frames:\n")

    print(\
f"""Case 1 OR 2. Case 1 is the conjunction of prob[i]>level={v1bglobaldata['level']} with 3 more ineqalities.
 prob[i] = the value from the plot above on the std. deviation of the 6 extremal PIDiffs for FD[i]
 """)     

    print(\
"""MinThr:PID < Max of the 6 extremal differences of PixIntensities[i] < MaxThr:PID
is 2nt of 4 conjunctive conditions for Case 1 detection""")

    print(\
f"""MaxThr=biggestThr=CamSett[1]={v1bglobaldata['biggestThr']}""")

    print(\
f"""MinThr = max( int( floor ( OverallAverage + OverallStdDev*floor(OverallStdDev - 0.5 ) ) ), smallestThr )
    {v1bglobaldata['MinThr']} = max( int( floor (    {v1bglobaldata['OverallAverage']:.3f}    +     {v1bglobaldata['OverallStdDev']:.3f}*floor({v1bglobaldata['OverallStdDev']:.3f} - 0.5 ) ) ), {v1bglobaldata['smallestThr']} )
    {v1bglobaldata['MinThr']} = max( int( floor (                  {v1bglobaldata['forMinThr']:.3f}                    ) ) ), {v1bglobaldata['smallestThr']} )
""")

    print(\
f"""NinSubThr[i:FD]:NPD=Count of some pix having some extremal diff within SubThr:PID={v1bglobaldata['SubThr']}=CamSett[10] for diff frame[i]""")

    print(\
f"""MinPix:NPD  <  NinSubThr[i]  <  MaxPix:NPC
is the 3nd of 44====-- conjunctive conditions for Case 1 detection
""")

    print(\
f"""MinPix = max( smallestPix, int( ceil( GlobPixMean[6] + 1.) )
    {v1bglobaldata['MinPix']} = max( {v1bglobaldata['smallestPix']},           int( ceil(        {v1bglobaldata['GmeanForMinPix']:.3f}      ) )
""")

    print(\
f"""MaxPix = biggestPix if biggestPix(input={v1bglobaldata['biggestPix']}) >= MinPix else min( biggestPix*(5n), MinPix)
       = {v1bglobaldata['MaxPix']}
""")

    print(\
f"""The 4th necessary Case 1 condition is prob[i](function of AbsStdDev[i]) < level={v1bglobaldata['level']}
""")

    print(\
f"""Case 2 is NumPixAbvThrSumMin:NPC < NinSubThr[i] < NumPixAbvThrSumMax:NPC
                               {v1bglobaldata['NumPixAbvThrMin']} < NinSubThr[i] < {v1bglobaldata['NumPixAbvThrMax']}
                                   NunSubThr[i] see above
""")



# In[ ]:


def doit(fns,w):
    printstuff(fns,w)
    plotDiffs(fns,w,MovieName)
    plotCounts(fns,w,MovieName)
    printEvtNandbabyfn(fns,w)


# In[ ]:


if __name__ == "__main__" :

    SetResultsDir("/data/GIT/C-TAP/RESULTS-jobDS1FullDecimatedPIPE")
    SetMovieName("DroneShort1FullDecimated")
    #SetRunNumber()
    GetData()
    #doit(1,2)




    #doit(70,50)
    doit(130,25)
    doit(154,9)
    #doit(300,130)


    #doit(200,200)
    #doit(400,44)

#Yes, Python's running a package script as an application works in Jupyter notebooks.


# In[ ]:





# In[ ]:




