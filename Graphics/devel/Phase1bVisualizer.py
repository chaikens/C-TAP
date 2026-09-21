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


Ph1adt = np.dtype([('frame', np.int32), 
               ('Rd','<i8'),('Rx',np.int16),('Ry',np.int16),
               ('Gd','<i8'),('Gx',np.int16),('Gy',np.int16),
               ('Bd','<i8'),('Bx',np.int16),('By',np.int16),
               ('rd','<i8'),('rx',np.int16),('ry',np.int16),
               ('gd','<i8'),('gx',np.int16),('gy',np.int16),
               ('bd','<i8'),('bx',np.int16),('by',np.int16),
               ('Rn',np.int32),('Gn',np.int32),('Bn',np.int32),
               ('rn',np.int32),('gn',np.int32),('bn',np.int32),
               ('stn',np.int32)]
             )

"""
For Phase1a .int file: from intdata

dtype([('frame', '<i4'), 
('Rd', '<i8'), ('Rx', '<i2'), ('Ry', '<i2'), 
('Gd', '<i8'), ('Gx', '<i2'), ('Gy', '<i2'), 
('Bd', '<i8'), ('Bx', '<i2'), ('By', '<i2'), 

('rd', '<i8'), ('rx', '<i2'), ('ry', '<i2'), 
('gd', '<i8'), ('gx', '<i2'), ('gy', '<i2'), 
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


Ph1bVglobaldt =  np.dtype([
               ('Subthr',np.int16),
               ('RewFram',np.int16),('ForFram',np.int16),('FramBeNew',np.int16),
               ('FracYes',np.single),('smallestThr',np.int16),('biggestThr',np.int16),
               ('smallestPix',np.int16),('biggestPix',np.int16),
               ('SkewGaussAmpl',np.single),('SkewGaussXi',np.single),
               ('SkewGaussOmega',np.single),('SkewGaussAlpha',np.single),
               ('level',np.single),('OverallStdDev',np.single),
               ("NumPixAbvThrMin",np.int16),("NumPixAbvThrMax",np.int16),
               ('forMinThr',np.single),('MinThr',np.int16),
               ('GlobalPixSigma',np.single),
               ('GmeanForMinPix',np.single),
               ('MinPix',np.int16),('MaxPix',np.int16)
])

#obsolete settings for testing away from C-TAP script RESULTS dirs.
"""
ResultsDir='./'
IntFileName=""
PlotDir='./'
MovieName=""
"""
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

#testing
"""
SetResultsDir("/data/GIT/C-TAP/RESULTS-jobDS1HalfDecimatedPIPE")
SetMovieName("DroneShort1HalfDecimated")
SetRunNumber(7)
setfiles()
"""
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

def plotprob(start,finish):
    fig, ax = plt.subplots()
    xs=np.linspace(start,finish)
    ys=vSG(xs)
    #print(xs)
    #print(ys)
    ax.plot(xs,ys)
    #plt.show()
    plt.savefig(ResultsDir+"ProbFunction.jpg")


# In[ ]:


PIDNames =['Rd','Gd','Bd','rd','gd','bd'] #d for diffs, CapRGB for extr. Pos diffs, lcrgb for Neg diffs.
PCntNames=['Rn','Gn','Bn','rn','gn','bn'] #n--numbers, ie. counts of pix w/ intens within thresholds of an extreme
PIDColor={ 'Rd' : (1,0,0), 'Gd' : (0,1,0), 'Bd' : (0,0,1),
    'rd' : (0,1,1), 'gd' : (1,0,1), 'bd' : (1,1,0) }
PCntColor={ 'Rn' : (1,0,0), 'Gn' : (0,1,0), 'Bn' : (0,0,1),
    'rn' : (0,1,1), 'gn' : (1,0,1), 'bn' : (1,1,0) }


# In[ ]:


def plotDiffs( fns, w,movn):

    #print(v1bglobaldata)

    Ms=makeDiffMats(intdata)
    PIDE=Ms['PixIntDiffExtremal']
    def PIDEstd(PIDE,n):
        return PIDE[:,n].std(ddof=1)

    def PIDEmean(PIDE,n):
        return PIDE[:,n].mean()

    fig, ax = plt.subplots(figsize=(14,11))
    ax.set_title(movn)
    ax.set_facecolor('black')

    plt.xticks(np.arange(fns,fns+w,w/20))

    frseph=1 #frame separators (vertical line) heights, max data y-values to be computed, ensure bottom is 0.

    for name in PIDNames[0:3]:  
        ax.scatter(intdata['frame'][fns-1:fns-1+w],intdata[name][fns-1:fns-1+w],color=PIDColor[name],alpha=0.6,s=50)
        frseph=max(frseph,intdata[name][fns-1:fns-1+w].max())
    for name in PIDNames[3:6]:
        fresph=max(frseph,intdata[name][fns-1:fns-1+w].max())
        ax.scatter(intdata['frame'][fns-1:fns-1+w],-intdata[name][fns-1:fns-1+w],color=PIDColor[name],alpha=0.8,s=15)

    Ms=makeDiffMats(intdata)
    PIDE=Ms['PixIntDiffExtremal']
    def funstd(n):
        return PIDEstd(PIDE,n-1)
    def funmean(n):
        return PIDEmean(PIDE,n-1)
    vfunstd=np.vectorize(funstd)
    vfunmean=np.vectorize(funmean) 
    xes=np.arange(fns,fns+w)
    yones=np.ones(w)

    yes=yones*v1bglobaldata['biggestThr']
    ax.plot(xes,yes,label='biggestThr')

    yes=yones*v1bglobaldata['smallestThr']
    ax.plot(xes,yes,label='smallestThr')

    yes=vfunstd(xes)*2
    ax.plot(xes,yes,label='std*2')
    yes=vfunmean(xes)
    ax.plot(xes,yes,label='mean')
    #yes=vSG(vfunstd(xes))*100
    #ax.plot(xes,yes,label='prob*100')
    NPixInSThr=Ms['NPixInSThr']
    ax.plot(xes,NPixInSThr[fns-1:fns-1+w],label="NinSubThr")

    frseph=math.floor(1+(frseph)*1.1)  #leave 10% empty at top, bott is zero.

    for i in range(fns-1,fns+w):
        line=lines.Line2D([i+0.5,i+0.5],   [0.0,frseph], color='white',linewidth=0.25)
        ax.add_line(line)

    ax.legend()

    plt.savefig(ResultsDir+movn+str(fns)+"."+str(fns+w)+".tiff")


# In[ ]:


def plotCounts( fns, w,movn):

    #print(v1bglobaldata)

    def PIDEstd(PIDE,n):
        return PIDE[:,n].std(ddof=1)

    def PIDEmean(PIDE,n):
        return PIDE[:,n].mean()
    Ms=makeDiffMats(intdata)
    PIDE=Ms['PixIntDiffExtremal']

    fig, ax = plt.subplots(figsize=(14,11))
    ax.set_title(movn)
    ax.set_facecolor('black')

    plt.xticks(np.arange(fns,fns+w,w/20))

    frseph=1  #frame separators (vertical line) heights, max data y-values to be computed, ensure bottom is 0.

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

    frseph=math.floor(1+(frseph)*1.1) #leave 10% empty at top, ensure bott is 0.

    yones=np.ones(w)
    yes=yones*v1bglobaldata['MinPix']
    ax.plot(xes,yes,label='MinPix')

    yes=yones*v1bglobaldata['MaxPix']
    ax.plot(xes,yes,label='MaxPix')

    yes=yones*v1bglobaldata['NumPixAbvThrMin']
    ax.plot(xes,yes,label='NumPixAbvThrMin')

    yes=yones*v1bglobaldata['NumPixAbvThrMax']
    ax.plot(xes,yes,label='NumPixAbvThrMax')


    for i in range(fns-1,fns+w):
        line=lines.Line2D([i+0.5,i+0.5],   [0.0,frseph], color='white',linewidth=0.25)
        ax.add_line(line)


    ax.legend()

    plt.savefig(ResultsDir+movn+str(fns)+"."+str(fns+w)+"counts.tiff")


# In[ ]:


def doit(fns,w):
    #global intndarray #so I can look at it when I am developing

    global IntFilePath
    setfiles()
    #intndarray=np.loadtxt(IntFilePath,converters=float,dtype=Ph1adt)
    plotDiffs(fns,w,MovieName)
    plotCounts(fns,w,MovieName)


# import Phase1bVisualizer as vs

# In[ ]:


if __name__ == "__main__" :

    SetResultsDir("/data/GIT/C-TAP/RESULTS-jobDS1HalfDecimatedPIPE")
    SetMovieName("DroneShort1HalfDecimated")
    SetRunNumber(9)

    doit(1,200)
    doit(200,200)
    doit(400,44)

#Yes, Python's running a package script as an application works in Jupyter notebooks.

