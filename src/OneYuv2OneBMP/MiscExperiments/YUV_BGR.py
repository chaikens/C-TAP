import numpy as np
from numpy.linalg import inv

Yp, U, V = var("Yp U V")
R, G, B  = var("R  G B")

#From https://learn.microsoft.com/en-us/windows/win32/medfound/about-yuv-video
#new used for HDTV, ITU-R BT.709
Ypfla = 0.2125*R + 0.7154*G + 0.0721*B
print("Ypfla=",Ypfla)
#Old std def TV, TU-R BT.601
YpflaOld = 0.299*R + 0.587*G + 0.114*B
print("YpflaOld=",YpflaOld)

#    Ypfla= 0.0721000000000000*B + 0.715400000000000*G + 0.212500000000000*R
#    YpflaOld= 0.114000000000000*B + 0.587000000000000*G + 0.299000000000000*R

#    Ufla= 0.927900000000000*B - 0.715400000000000*G - 0.212500000000000*R
#    Vfla= -0.0721000000000000*B - 0.715400000000000*G + 0.787500000000000*R

YUVFromBGR = np.array( [
    [ Ypfla.coefficient(B).n(), Ypfla.coefficient(G).n(), Ypfla.coefficient(R).n() ],
    [ Ufla.coefficient(B).n(), Ufla.coefficient(G).n(), Ufla.coefficient(R).n() ],
    [ Vfla.coefficient(B).n(), Vfla.coefficient(G).n(), Vfla.coefficient(R).n() ] ] )
YUVFromBGR

#    array([[ 0.0721,  0.7154,  0.2125],
#           [ 0.9279, -0.7154, -0.2125],
#           [-0.0721, -0.7154,  0.7875]])

BGRFromYUV=inv(YUVFromBGR)
BGRFromYUV


#array([[ 1.00000000e+00,  1.00000000e+00, -2.99122488e-17],
#       [ 1.00000000e+00, -1.00782779e-01, -2.97036623e-01],
#       [ 1.00000000e+00,  0.00000000e+00,  1.00000000e+00]])

