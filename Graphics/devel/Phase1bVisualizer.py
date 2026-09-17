#!/usr/bin/env python
# coding: utf-8

# In[ ]:


def helpVis():
    print(" Use SetIntFileName(_), and if you don't want to use cwd, SetDataDir(__); SetPlotDir(_); SetMovieName(_).")
    print(" Then, doit(<start frame number>, <number of frames>)" )


# In[ ]:


import numpy as np
import math
import matplotlib.pyplot as plt
import matplotlib.lines as lines


# In[ ]:


DataDir='./'
IntFileName=""
PlotDir='./'
MovieName=""


# In[ ]:


def SetDataDir(x):
    global DataDir
    DataDir=x
def SetIntFileName(x):
    global IntFileName
    IntFileName=x
def SetPlotDir(x):
    global PlotDir
    PlotDir=x
def SetMovieName(x):
    global MovieName
    MovieName=x


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
('bd', '<i8'), ('bx', '<i2'), ('by', '<i2'), 

('Rn', '<i4'), ('Gn', '<i4'), ('Bn', '<i4'), 
('rn', '<i4'), ('gn', '<i4'), ('bn', '<i4'), 

('stn', '<i4')])
"""

Ph1bVdt =  np.dtype([('frame', np.int32), 
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

def makeDiffMats(v):
    PixIntDiffExtremal=np.vstack([v['Rd'],v['Gd'],v['Bd'],-v['rd'],-v['gd'],-v['bd']])
    NsPixInThr=np.vstack([v['Rn'],v['Gn'],v['Bn'],v['rn'],v['gn'],v['bn']])
    NPixInSThr=v['stn']
    return { 'PixIntDiffExtremal':PixIntDiffExtremal, 'NsPixInThr':NsPixInThr, 'NPixInSThr':NPixInSThr}


# In[ ]:


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
    plt.savefig(Plotdir+"ProbFunction.jpg")


# In[ ]:


PIDNames =['Rd','Gd','Bd','rd','gd','bd'] #d for diffs, CapRGB for extr. Pos diffs, lcrgb for Neg diffs.
PCntNames=['Rn','Gn','Bn','rn','gn','bn'] #n--numbers, ie. counts of pix w/ intens within thresholds of an extreme
PIDColor={ 'Rd' : (1,0,0), 'Gd' : (0,1,0), 'Bd' : (0,0,1),
    'rd' : (0,1,1), 'gd' : (1,0,1), 'bd' : (1,1,0) }
PCntColor={ 'Rn' : (1,0,0), 'Gn' : (0,1,0), 'Bn' : (0,0,1),
    'rn' : (0,1,1), 'gn' : (1,0,1), 'bn' : (1,1,0) }


# In[ ]:


def plotDiffs(intdata, fns, w,movn):


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

    plt.savefig(PlotDir+movn+str(fns)+"."+str(fns+w)+".tiff")


# In[ ]:


def plotCounts(intdata, fns, w,movn):

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

    for i in range(fns-1,fns+w):
        line=lines.Line2D([i+0.5,i+0.5],   [0.0,frseph], color='white',linewidth=0.25)
        ax.add_line(line)


    ax.legend()

    plt.savefig(PlotDir+movn+str(fns)+"."+str(fns+w)+"counts.tiff")


# In[ ]:


def doit(fns,w):
    global intndarray #so I can look at it when I am developing
    global MovieName
    global DataDir
    global IntFileName
    if MovieName == "":
        MovieName = IntFileName
    intndarray=np.loadtxt(DataDir+IntFileName,converters=float,dtype=Ph1adt)
    plotDiffs(intndarray,fns,w,MovieName)
    plotCounts(intndarray,fns,w,MovieName)


# import Phase1bVisualizer as vs

# In[ ]:


#vs.#
if __name__ == "__main__" :
    #Hmm... we could query the user for these arguments.
    SetIntFileName("321-18.int")
#vs.#
    SetMovieName("321-18")
#vs.#
    doit(1,200)

#Yes, Python's running a package script as an application works in Jupyter notebooks.


# In[ ]:


intndarray[0]

First 3 lines of an .int file
1          39  1436  165     39  1436  165     39 1436  165      -19   352  559    -19   352  559    -19  352  559    3  3  3    1  1  1   6
2          11   784  591     11   784  591     11  784  591      -17   853   85    -17   853   85    -17  853   85    0  0  0    1  1  1   0
3          29  1076  391     29  1076  391     29 1076  391      -14  1280  458    -14  1280  458    -15 1745  320    4  5  4    0  0  1   10


# In[ ]:


from io import StringIO


# In[ ]:


sorig=StringIO("""1          39  1436  165     39  1436  165     39 1436  165      -19   352  559    -19   352  559    -19  352  559    3  3  3    1  1  1   6 24 34 24
2          11   784  591     11   784  591     11  784  591      -17   853   85    -17   853   85    -17  853   85    0  0  0    1  1  1   0
3          29  1076  391     29  1076  391     29 1076  391      -14  1280  458    -14  1280  458    -15 1745  320    4  5  4    0  0  1   10
""")

s=StringIO("""1,          39,  1436,  165,     39,  1436,  165,     39, 1436,  165,      -19,   352,  559,    -19,   352,  559,    -19,  352,  559,    3,  3,  3,    1,  1,  1,   6, 24, 34, 24
2,          11,   784,  591,     11,   784,  591,     11,  784,  591,      -17,   853,   85,    -17,   853,   85,    -17,  853,   85,    0,  0,  0,    1,  1,  1,   0, , ,
3,         29, 1076,  391,     29,  1076,  391,     29, 1076,  391,      -14,  1280,  458,    -14,  1280,  458,    -15, 1745,  320,    4,  5,  4,   0,  0,  1,   10, , ,
""")

print(s.getvalue())
a=np.genfromtxt(s,unpack=False,dtype=Ph1adt,delimiter=",",missing_values="",filling_values=0.0)
print(a)


# In[ ]:


a


# In[ ]:




