#!/usr/bin/env python
# coding: utf-8

# In[45]:


import numpy as np


# In[46]:


import wand
from wand.image import Image
from wand.drawing import Drawing


# In[47]:


import math


# In[48]:


import matplotlib.pyplot as plt


# In[49]:


dt = np.dtype([('frame', np.int32), 
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


# In[50]:


intdata = np.loadtxt("DroneShort1HalfDecimated.int.1.txt",converters=float,dtype=dt)
nframes=len(intdata)
IPath="/media/seth/CTAP/bitmaps-jobDS1HalfDecimatedFRAME"


# In[51]:


def to6( n ):
    return "{!s:>06}".format(n)
def tothumb(n):
    return IPath+"/thumb"+to6(n)+".bmp"
tothumb(1234)


# In[52]:


width=0
height=0
def setwh(n):
    img=Image(filename=tothumb(1))
    global width
    width=img.width
    global height
    height=img.height
setwh(1)
widthTo1920=math.ceil(float(width)/1920.0)


# Setup for drawing the data line.

# In[53]:


llcapx=int(width/20)
llcapy=int(height-width/20)


# In[54]:


drdbl=Drawing()
drgbl=drdbl.clone()
drdbl.font_size=30*widthTo1920
drdbl.fill_color="WHITE"
drdbl.stroke_color="WHITE"


# Setup for drawing markers on Phase1a selected pixels.

# In[55]:


def M(TH) :
    return( np.array( [ [math.cos(math.pi*TH/180.), math.sin(math.pi*TH/180.)], [-math.sin(math.pi*TH/180.), math.cos(math.pi*TH/180.)] ] ) )

#Geometry of normalized unit arrows to show RGB changes
TH=30.0                  #angle of arrows away from vertical, and hands away from body
lcircr=0.1               #little circle radius
hslen=0.1                #length of each hand of an
# Unit Vectors
g1=np.array([0.0,1.0])   #unit lower case, down, green

#tiny vectors
tc=lcircr*np.array([0.0,1.0]) #radius (down, y dir of tiny circle, and foot of down unit arrow
def T(s) :
    return (tc + g1*s) #tail on tiny circle, head down by unit * s (scale, 0<=s<=1)

#Arrow body is (tc->T(s)*M..
def arrB(s) :
    return np.array([tc, T(s)])

TL=hslen*g1@M(180.0+TH) #coord of left hand rel to head
TR=hslen*g1@M(180.0-TH) #coord of right hand rel to head

#down dir Left arm LA(s) is (T(s)->TL(s))
def arrL(s) :
    return np.array([T(s), T(s)+TL])


#down dir Right arm LA(s) is (T(s)->TL(s))
def arrR(s) :
    return np.array([T(s), T(s)+TR])

def unit_up_arrow(s) :
    return -np.concat([arrB(s),arrL(s),arrR(s)])


# In[56]:


def dispdata(row):
    #print(fn, row['frame'])
    return ( [ row['Rd'],   -30.0, [row['Rx'],row['Ry']], row['Rn' ], "red" ],
             [ row['Gd'],     0.0, [row['Gx'],row['Gy']], row['Gn' ], "green" ],
             [ row['Bd'],    30.0, [row['Bx'],row['By']], row['Bn' ], "blue" ],
             [ -row['rd'], -150.0, [row['rx'],row['ry']], row['rn' ], "red" ],
             [ -row['gd'],  180.0, [row['gx'],row['gy']], row['gn' ], "green" ],
             [ -row['bd'],  150.0, [row['rx'],row['by']], row['bn' ], "blue" ] )             


# In[57]:


colvaldiv = float(128)
arrlen = 100*widthTo1920

def drdata(dwg, row):
    data = dispdata(row)
    for r in data:
        #print(r)
        #print((r[0]/colvaldiv))
        #print( "arrow", arrlen*unit_up_arrow( (r[0]/colvaldiv)) )
        line = np.round( (arrlen*unit_up_arrow(r[0]/colvaldiv))@M(r[1]) + r[2]).astype(int)
        dwg.stroke_width = 2
        dwg.stroke_color = wand.color.Color( r[4] )
        for i in range(0,3):
            dwg.line(line[2*i],line[2*i+1])


# In[58]:


img=0
def vis(fn):
    global img
    img=Image(filename=tothumb(fn))
    row=np.array(intdata[fn-1])
    t=intdata[fn-1].item(0)
    intdatarowstr = str(t[0])
    for z in range(1,24,3):
        intdatarowstr+=" "+str(t[z:z+3])
    intdatarowstr+="  "+str(t[24])
    global drdbl
    draw=drdbl.clone()
    draw.text(llcapx,llcapy,intdatarowstr)
    drdata(draw, row )
    draw(img)
    #draw
    #draw(img)
    return img #so jupyter tries to print the result which makes the picture appear!      


# In[59]:


vis(319)


# In[60]:


def tophonea(n):
    return IPath+"/phonea"+to6(n)+".jpg"
def doAll():
    for i in range(nframes):
        fn=i+1
        img=vis(fn)
        print("visualized frame", fn, end="")
        img.format = 'jpeg'
        img.save(filename=tophonea(fn))
        print(" saved", fn, end="\r")


# In[61]:


doAll()


# In[ ]:




