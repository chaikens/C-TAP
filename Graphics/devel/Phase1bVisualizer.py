#!/usr/bin/env python
# coding: utf-8

# In[ ]:


def helpVis():
    print(" Get data and put plots a RESULTS dir: Use SetResultsDir(_), SetMovieName() and SetRunNumber( ).")
    print(" Then, doit(<start frame number>, <number of frames>)" )


# In[ ]:


import numpy as np
import math
import matplotlib.pyplot as plt
import matplotlib.lines as lines


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
               ('SubThr',np.int16),                                                         # 0
               ('RewFram',np.int16),('ForFram',np.int16),('FramBeNew',np.int16),            #1, 2, 3
               ('FracYes',np.single),('smallestThr',np.int16),('biggestThr',np.int16),      #4, 5, 6 
               ('smallestPix',np.int16),('biggestPix',np.int16),                            #7,8
               ('SkewGaussAmpl',np.single),('SkewGaussXi',np.single),                       #9, 10
               ('SkewGaussOmega',np.single),('SkewGaussAlpha',np.single),                   #11, 12
               ('level',np.single),                                                         #13
               ("NumPixAbvThrMin",np.int16),("NumPixAbvThrMax",np.int16),                   #14, 15
               ('OverallAverage',np.single),('OverallStdDev',np.single), #of Extremal PixIntDiff vals. #16, 17
               ('forMinThr',np.single),('MinThr',np.int16),                                 #18, 19
               ('GlobalPixSigma',np.single),                                                #20
               ('GmeanForMinPix',np.single),                                                #21
               ('MinPix',np.int16),('MaxPix',np.int16)                                      #22, 23
]
Ph1bVglobaldt =  np.dtype(Ph1bVglobaldtList)

def printglobals(v1bglobaldata):
    i = 0
    print("Variable = value   ( Key output by Phase1b C++ program, please check consistency. )")
    for e in Ph1bVglobaldtList:
        print(f"{e[0]}  =  {v1bglobaldata[e[0]]:.3f}    ({v1bglobalkeydata[i]} )")
        i = i + 1

#obsolete settings for testing away from C-TAP script RESULTS dirs.
ResultsDir='./'
IntFileName=""
PlotDir='./'
MovieName=""
# In[ ]:


def SetResultsDir(x):
    global ResultsDir
    ResultsDir=x+"/"
def SetMovieName(x):
    global MovieName
    MovieName=x
def SetRunNumber(x):
    global RunNumber
    RunNumber=x

def setfiles(): #called internally by doit(_,_)
    global IntFilePath
    global v1bFilePath
    global v1bkeyFilePath
    global v1bglobalFilePath
    global v1bglobalkeyFilePath
    global intdata

    global v1bdata, v1bdatakey, v1bglobaldata, v1bglobalkeydata
    v1bdata=None        #frame by frame phase 1b used and intermediate data
    v1bdatakey=None     #its keys
    v1bglobaldata=None  #settings and data computed from all frames and then used
    v1bglobalkeydata=None #its keys

    RunNumberStr=""
    if RunNumber != "":
        RunNumberStr+="."+str(RunNumber)
    MovieNamePath=ResultsDir+MovieName
    IntFilePath=ResultsDir+MovieName+".int"+RunNumberStr
    v1bFilePath=MovieNamePath+".v1b"+RunNumberStr
    v1bkeyFilePath=MovieNamePath+".v1b.key"+RunNumberStr
    v1bglobalFilePath=MovieNamePath+".v1b.global"+RunNumberStr
    v1bglobalkeyFilePath=MovieNamePath+".v1b.global.key"+RunNumberStr

    intdata=np.loadtxt(IntFilePath,converters=float,dtype=Ph1adt)
    v1bglobaldata=np.genfromtxt(v1bglobalFilePath,unpack=False,dtype=Ph1bVglobaldt,delimiter=",",missing_values="",filling_values=0.0)
    v1bdata=np.genfromtxt(v1bFilePath,unpack=False,dtype=Ph1bVdt,delimiter=",",missing_values="",filling_values=0.0)
    v1bglobalkeydata=np.loadtxt(v1bglobalkeyFilePath,dtype=str,delimiter=",")


# In[ ]:


#to do:  Get the params from 1b global data!
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

def plotprob(start,finish,level=0):
    fig, ax = plt.subplots()
    xs=np.linspace(start,finish)
    ys=vSG(xs)
    #print(xs)
    #print(ys)
    ax.plot(xs,ys, label='1-(skewGauss density)')
    if level > 0:
        ax.hlines(level,start,finish,label='level',color='red')
    ax.legend()
    #plt.show()
    plt.savefig(ResultsDir+"ProbFunction.jpg")


# In[ ]:


#Used to overlay a transparent white bar for frames in the events output by Phase1b.
def notneg(x):
    if x < 0:
        return 0
    else:
        return 1

notnegv = np.vectorize(notneg)

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

    fig, ax = plotsetup(fns, w, movn)
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

    ax.hlines(v1bglobaldata['biggestThr'],fns-1,fns+w,label='biggest(Max)Thr',color='green')

    ax.hlines(v1bglobaldata['smallestThr'],fns-1,fns+w,label='smallestThr',color='blue')

    ax.hlines(v1bglobaldata['MinThr'],fns-1,fns+w,label='MinThr',color='red')

    yes=vfunstd(xes)*2
    ax.plot(xes,yes,label='std*2')
    yes=vfunmean(xes)
    ax.plot(xes,yes,label='mean')

    ax.plot(v1bdata['frame'][fns-1:fns+w],v1bdata['AbsMax'][fns-1:fns+w],label='AbsMax')

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

    ax.bar(v1bdata['frame'][fns-1:fns+w],frseph*notnegv(v1bdata['EvtN'][fns-1:fns+w]),alpha=0.25,color='white')
    ax.scatter(v1bdata['frame'][fns-1:fns+w],frseph*v1bdata['SignalTruth'][fns-1:fns+w],marker='s',s=6,color='white',label='SignalTruth')
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.99)*v1bdata['GoldE'][fns-1:fns+w],marker='^',s=6,color='white',label='GoldE')

    # Case 1: probGTlevel &&
    # AbsMaxGTMinThr AbsMaxLTMaxThr NumPixAbvSubThrSumGTMinPix NumPixAbvSubThrSumLTMaxPix
    name='probGTlevel'
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.98)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='yellow',label=name)
    name='AbsMaxGTMinThr'
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.97)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='yellow',label=name)
    name='AbsMaxLTMaxThr' 
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.96)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='yellow',label=name)
    name='NumPixAbvSubThrSumGTMinPix' 
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.95)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='yellow',label=name)
    name='NumPixAbvSubThrSumLTMaxPix'
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.94)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='yellow',label=name )

    # Case 4: (2 and 3 are for obsolete B1,3,4 cameras, ignore)
    # stnGTNumPixAbvThrSumMin && stnLTNumPixAbvThrSumMax
    name='stnGTNumPixAbvThrSumMin'
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.93)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='orange',label=name )
    name='stnLTNumPixAbvThrSumMax'
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.92)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='orange',label=name ) 

    ax.legend(bbox_to_anchor=(0.95, 0.83))

    plt.savefig(ResultsDir+movn+str(fns)+"."+str(fns+w)+".tiff")


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
    fig, ax = plotsetup(fns, w, movn)
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



    ax.plot(xes,NPixInSThr[fns-1:fns-1+w],label="NinSubThr")



    yones=np.ones(w)
    yes=yones*v1bglobaldata['MinPix']
    ax.plot(xes,yes,label='MinPix')

    yes=yones*v1bglobaldata['MaxPix']
    ax.plot(xes,yes,label='MaxPix')
    frseph=max(frseph,v1bglobaldata['MaxPix'])

    yes=yones*v1bglobaldata['NumPixAbvThrMin']
    ax.plot(xes,yes,label='NumPixAbvThrMin')

    yes=yones*v1bglobaldata['NumPixAbvThrMax']
    ax.plot(xes,yes,label='NumPixAbvThrMax')
    frseph=max(frseph,v1bglobaldata['NumPixAbvThrMax'])

    frseph=frseph*1.05 #more space for boolean reports  
    frseph=math.floor(1+(frseph)*1.1) #leave 10% empty at top, ensure bott is 0.
    for i in range(fns-1,fns+w):
        line=lines.Line2D([i+0.5,i+0.5],   [0.0,frseph], color='white',linewidth=0.25)
        ax.add_line(line)

    ax.bar(v1bdata['frame'][fns-1:fns+w],frseph*notnegv(v1bdata['EvtN'][fns-1:fns+w]),alpha=0.25,color='white')
    ax.scatter(v1bdata['frame'][fns-1:fns+w],frseph*v1bdata['SignalTruth'][fns-1:fns+w],marker='s',s=6,color='white',label='SignalTruth')
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.99)*v1bdata['GoldE'][fns-1:fns+w],marker='^',s=6,color='white',label='GoldE')

    # Case 1: probGTlevel &&
    # AbsMaxGTMinThr AbsMaxLTMaxThr NumPixAbvSubThrSumGTMinPix NumPixAbvSubThrSumLTMaxPix
    name='probGTlevel'
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.98)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='yellow',label=name)
    name='AbsMaxGTMinThr'
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.97)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='yellow',label=name)
    name='AbsMaxLTMaxThr' 
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.96)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='yellow',label=name)
    name='NumPixAbvSubThrSumGTMinPix' 
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.95)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='yellow',label=name)
    name='NumPixAbvSubThrSumLTMaxPix'
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.94)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='yellow',label=name )

    # Case 4: (2 and 3 are for obsolete B1,3,4 cameras, ignore)
    # stnGTNumPixAbvThrSumMin && stnLTNumPixAbvThrSumMax
    name='stnGTNumPixAbvThrSumMin'
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.93)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='orange',label=name )
    name='stnLTNumPixAbvThrSumMax'
    ax.scatter(v1bdata['frame'][fns-1:fns+w],(frseph*0.92)*v1bdata[name][fns-1:fns+w],marker='+',s=6,color='orange',label=name )

    ax.legend(bbox_to_anchor=(0.95, 0.83))

    plt.savefig(ResultsDir+movn+str(fns)+"."+str(fns+w)+"counts.tiff")


# In[ ]:


def printstuff():
    """ Print global numeric data input parameters and calculations """
    print("Global Data pertaining to the whole movie:")

    printglobals(v1bglobaldata)

    print(\
"""\nUnits--
PID:Pixel Intensity Difference
NPC:Number of Cases of a Pixel state or other Predicate about Pixels
FD:Frame Difference Index for Frame[i+1]-Frame[i]""")

    print("\nHow it is used to select interesting frames:\n")

    print(\
"""MinThr:PID < Max of the 6 extremal differences of PixIntensities[i] < MaxThr:PID
is one of 3 conjunctive conditions for Case 1 detection""")

    print(\
f"""MaxThr=biggestThr=CamSett[1]={v1bglobaldata['biggestThr']}""")

    print(\
f"""MinThr = max( int( floor ( OverallAverage + OverallStdDev*floor(OverallStdDev - 0.5 ) ) ), smallestThr )
    {v1bglobaldata['MinThr']} = max( int( floor ( {v1bglobaldata['OverallAverage']:.3f} + {v1bglobaldata['OverallStdDev']:.3f}*floor({v1bglobaldata['OverallStdDev']:.3f} - 0.5 ) ) ), {v1bglobaldata['smallestThr']:.3f} )
    {v1bglobaldata['MinThr']} = max( int( floor (                  {v1bglobaldata['forMinThr']:.3f}              ), {v1bglobaldata['smallestThr']} )
""")

    print(\
"NinSubThr=Count of some pix having some extremal diff within Subthr[for each frame diff i]")

    print(\
f"""MinPix:NPD < NinSubThr[i:FD]:NPD(Num pix-col-intens-within SubThr:PID={v1bglobaldata['SubThr']} (CamSett[10]) of maximal extremal diff < MaxPix:NPC
is the 2nd of 3 conjunctive conditions for Case 1 detection
""")

    print(\
f"""MinPix = max( smallestPix, int( ceil( GlobPixMean[6] + 1.) )
    {v1bglobaldata['MinPix']} = max( {v1bglobaldata['smallestPix']}, int( ceil(      {v1bglobaldata['GmeanForMinPix']:.3f}        ) )
""")

    print(\
f"""MaxPix = biggestPix if biggestPix(input={v1bglobaldata['biggestPix']}) >= MinPix else min( biggestPix*(5n), MinPix)
       = {v1bglobaldata['MaxPix']}
""")

    print(\
f"""The 3rd necessary Case 1 condition is prob[i](function of AbsStdDev[i]) < level={v1bglobaldata['level']}
""")

    print(\
f"""Case 2 is NumPixAbvThrSumMin:NPC < NinSubThr[i] < NumPixAbvThrSumMax:NPC
           {v1bglobaldata['NumPixAbvThrMin']} < Count of some pix having some extremal diff within Subthr < {v1bglobaldata['NumPixAbvThrMax']}
""")



# In[ ]:


def doit(fns,w):
    #global intndarray #so I can look at it when I am developing

    global IntFilePath
    setfiles()

    #put our SkewGauss params into our skewgauss function, and plot it
    global SkewGauss
    SkewGauss=np.array([v1bglobaldata['SkewGaussAmpl'],
                        v1bglobaldata['SkewGaussXi'],
                        v1bglobaldata['SkewGaussOmega'], 
                        v1bglobaldata['SkewGaussAlpha']])
    plotprob(0,10,v1bglobaldata['level'])

    printstuff()

    plotDiffs(fns,w,MovieName)
    plotCounts(fns,w,MovieName)


# import Phase1bVisualizer as vs

# In[ ]:


if __name__ == "__main__" :

    SetResultsDir("/data/GIT/C-TAP/RESULTS-jobDS1FullDecimatedPIPE")
    SetMovieName("DroneShort1FullDecimated")
    SetRunNumber(16)
    setfiles()



    doit(70,100)


    #doit(200,200)
    #doit(400,44)

#Yes, Python's running a package script as an application works in Jupyter notebooks.

