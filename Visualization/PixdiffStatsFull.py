#!/usr/bin/env python
# coding: utf-8

# In[23]:


import numpy as np
from matplotlib import pyplot as plt


# In[24]:


BMD='/media/seth/CTAP/bmdir/bitmaps-jobFF'
FirstFrameN=1
UNMASKFILE="/data/GIT/C-TAP/RESULTS-jobFF/DS.btmask.1"


# In[25]:


def to6( n ):
    return "{!s:>06}".format(n)
def tothumb(n):
    return BMD+"/thumb"+to6(n)+".bmp"
#def todatasvg(n):
#    return IPath+"/data"+to6(n)+".svg"
#tothumb(1234)
#tothumb(DEVN)


# In[26]:


def getwh(n):
    b1=np.fromfile(tothumb(n),dtype=np.uint8).astype(np.int16)
    return (np.uint32( b1[18]+256*(b1[19]+ 256*(b1[20]+256*b1[21])) ),
            np.uint32( b1[22]+256*(b1[23]+ 256*(b1[24]+256*b1[25])) ))
    #32 bits so multiplication is OK

(width,height)=getwh(FirstFrameN)

#use this for analysis of an entire frame pair
WHOLEB=[0,0,width,height]
print((width,height))


# In[27]:


#Specify a positive (drone containing frame) and control (frame without a drone) frame number.

DEVN=147 #frame pair with a drone KEEP THIS to apply to UNDER FN DEFNS SO experimenting with global results is consistent.
ContrDEVN=80 #no drone

#Specify a rectangle containing the drone and a nearby control rectangle
#[Upper Left X-coord, Upper Left Y-coord, width of rectangle, height]
#Full Resolution
DEVB=[3245,460,80,70] #region in 147-48 with the drone
ContrDEVB=[3000,460,80,70] #nearby region in 147-8 without the drone

#Half Resolution
#DEVB=[1622,230,40,35] #region in 147-48 with the drone
#ContrDEVB=[1500,230,40,35] #nearby region in 147-8 without the drone

WHOLEB=[0,0,width,height] #Bounds for doing stats on the whole image pair


# In[28]:


#This makes an ordinary ndarray that will be used as the mask when we make
# np.ma masked arrays for doing the statistical calculations.

def makemaskarray(f): 
    unmaskbits=np.fromfile(f,dtype=np.uint8) 
        #the bits left-to-right in the bytes of the file and array correspond to pixels we that will not be masked.
    unmaskbytes=np.unpackbits(unmaskbits.data)
        #convert to an array of bytes valued 0 or 1
    unmaskbytesrect=np.reshape(unmaskbytes,(height,width)) #make into rectangle
    unmaskRGBrect0=np.repeat(unmaskbytesrect,3,axis=1)     #triple each 1 or 0 for masking R B B
    unmaskRGBrect1=np.reshape(unmaskRGBrect0,(height,width,3)) 
    #group each threesome for a color the plot software will like
    maskRGBrect=1-unmaskRGBrect1 #convert 1s for using to 0 for numpy's not masking
    return maskRGBrect

maskRGBrect=makemaskarray(UNMASKFILE)
#maskRGBrect


# In[29]:


def setpair(n):
    #global b1,b2,mb1,mb2 #for debugging
    fn1=n
    fn2=n+1
    bmpfile1,bmpfile2=tothumb(fn1),tothumb(fn2)
    b1=np.fromfile(bmpfile1,dtype=np.uint8).astype(np.int16)
    b2=np.fromfile(bmpfile2,dtype=np.uint8).astype(np.int16)
    #Use 16 bits instead of 8 so subtraction is ok.
    b1=np.flip(b1[54:].reshape([height,width,3]),(0,2))
    b2=np.flip(b2[54:].reshape([height,width,3]),(0,2))
    #First, reshape groups into triples the bytes each row.
    #Second, flip (0) flips Microsoft orientation right side up; and (2) flips GBR to RGB
    #shape=b1.shape
    #zeros=np.zeros(shape,like=b1,dtype=np.int16) #for devel experiment
    
    #continue with mask

    mb1=np.ma.masked_array(b1,mask=maskRGBrect,fill_value=128) #Use gentle gray to display masked img.
    mb2=np.ma.masked_array(b2,mask=maskRGBrect,fill_value=128) #Statistical calcs are masked, 
    #so they don't see the fill value 
    #mb1filled=mb1.filled()
    #mb2filled=mb2.filled()
    return (mb1,mb2) 
    #See the masked img by plt.imshow() on each of these

#(mb1,mb2)=setpair(ContrDEVN)


# In[30]:


def hhh(pairn,range):
    #plt.imshow(mb1)
#shows the original image
    mb1,mb2=setpair(pairn)
    plt.subplot(221)
    plt.imshow( np.clip(mb1-mb2, a_min=0,a_max=255)[range[1]:range[1]+range[3],range[0]:range[0]+range[2]] )
    plt.subplot(222)
    plt.imshow( np.clip(mb2-mb1, a_min=0,a_max=255)[range[1]:range[1]+range[3],range[0]:range[0]+range[2]] )
    plt.subplot(223)
    plt.imshow(mb1[range[1]:range[1]+range[3],range[0]:range[0]+range[2]])
    plt.subplot(224)
    plt.imshow(mb2[range[1]:range[1]+range[3],range[0]:range[0]+range[2]].filled())
#plt.show()
#shows the image with masked area in grey
hhh(DEVN,DEVB)


# In[31]:


hhh(DEVN,WHOLEB)


# In[32]:


#
# Histogram making, using np.histogram()
#
#It took a bit of doing to hack out how to deal with np.histogram() operates on the underlying, unmasked version
#of a np.ma  This includes converting bool to int (bool is a Python subclass of int) in order to complement the bool arran
def matofa(mar):
    #print("matofa:mar")
    #print(type(mar), mar, mar.mask)
    marf=mar.flatten()
    #print("matofa:marf")
    #print(type(marf),marf, marf.mask)
    if marf.mask.any() == False:
        return marf.data
    mcomp=(1-marf.mask.astype(bool)).astype(bool)
    #print("matofa:mcomp")
    #print(mcomp)
    return marf.compress(mcomp).data
def histofma(mar,bins=0):
    fa=matofa(mar)
    if bins == 0:
        bins = mar.size-1
    return np.histogram(fa,bins=bins)
    
def nsrs(f):
    if f <= 0:
        return f
    else:
        return max(0,np.log(f))
        
def dostats(pdiff,ndiff):
    ca=['red','green','blue']
    diffs=[]
    print("The bar lengths are logarithmic!\n")       
    for foo in [["PosDiff", pdiff]              ,["NegDiff", ndiff]]:
        for i in range(0,3):
            print(foo[0], "color=", ca[i])
            #print(foo[1], foo[1].mask )
            diffs.append((foo[1])[:,:,i:i+1].flatten())
            #print(diffs[i], diffs[i].mask)
            maxdiff=(foo[1])[:,:,i:i+1].flatten().max()
            freq, bins = histofma(diffs[i],bins=maxdiff)
            for b, f in zip(bins[:], freq):
                print(round(b, 1), ' '.join(np.repeat('*', nsrs(f))), 
                     f, "/", len(diffs[i]), "=", f"{f/len(diffs[i]):.3g}" ) # f) 
            print("mean=",(foo[1])[:,:,i:i+1].flatten().mean())
            print("std=",(foo[1])[:,:,i:i+1].flatten().std())
            print("max=",(foo[1])[:,:,i:i+1].flatten().max())
            print()


def st(d1,d2,IX,IY,NX,NY):
    global sd1, sd2,pdiff,ndiff
    sd1=d1[IY:IY+NY,IX:IX+NX] #section off the subimage someone wants
    sd2=d2[IY:IY+NY,IX:IX+NX]
    pdiff=np.clip(sd2-sd1,a_min=0,a_max=255) #positive differences.
    ndiff=np.clip(sd1-sd2,a_min=0,a_max=255) #absolute value of neg differences.
    dostats(pdiff,ndiff)
    #(plt.imshow(mb1s),plt.imshow(mb2s))

def seestat(np):
    global mb1, mb2
    mb1,mb2=setpair(np)
    #st(mb1,mb2,*WHOLEB)
    print("XXXXXXXXXXXXXXX  CONTROL XXXXXXXXXXXXXXXXXXXXXXX")
    st(mb1,mb2,*ContrDEVB)
    print("XXXXXXXXXXXXXXX  POSITIVE XXXXXXXXXXXXXXXXXXXXXXX")
    st(mb1,mb2,*DEVB)

def seestatWHOLE(np):
    global mb1, mb2
    mb1,mb2=setpair(np)
    print("XXXXXXXXXXXXXXX  WHOLE IMAGE ", np, "    XXXXXXXXXXXXXXXXXXXXXXX")
    st(mb1,mb2,*WHOLEB)


# In[33]:


seestat(DEVN)


# In[34]:


seestat(ContrDEVN)


# In[35]:


seestatWHOLE(DEVN) 


# In[36]:


seestatWHOLE(ContrDEVN)

