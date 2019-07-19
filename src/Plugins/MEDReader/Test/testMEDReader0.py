#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2019  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
# Author : Anthony Geay

import os
import sys

from medcoupling import *
from paraview.simple import *
from MEDReaderHelper import WriteInTmpDir,RetriveBaseLine

def GenerateCase():
    fname="testMEDReader0.med"
    #########
    mz=MEDCoupling1SGTUMesh("mesh",NORM_QUAD9)
    mz.setNodalConnectivity(DataArrayInt([0,6,37,12,76,112,113,82,205,12,37,38,13,113,114,115,83,206,13,38,9,1,115,116,79,84,207,6,7,39,37,77,117,118,112,208,37,39,40,38,118,119,120,114,209,38,40,10,9,120,121,80,116,210,7,8,41,39,78,122,123,117,211,39,41,42,40,123,124,125,119,212,40,42,11,10,125,126,81,121,213,1,9,43,17,79,127,128,88,214,17,43,44,18,128,129,130,89,215,18,44,14,2,130,131,85,90,216,9,10,45,43,80,132,133,127,217,43,45,46,44,133,134,135,129,218,44,46,15,14,135,136,86,131,219,10,11,47,45,81,137,138,132,220,45,47,48,46,138,139,140,134,221,46,48,16,15,140,141,87,136,222,1,9,49,22,79,142,143,94,223,22,49,50,23,143,144,145,95,224,23,50,51,24,145,146,147,96,225,24,51,52,25,147,148,149,97,226,25,52,53,26,149,150,151,98,227,26,53,19,3,151,152,91,99,228,9,10,54,49,80,153,154,142,229,49,54,55,50,154,155,156,144,230,50,55,56,51,156,157,158,146,231,51,56,57,52,158,159,160,148,232,52,57,58,53,160,161,162,150,233,53,58,20,19,162,163,92,152,234,10,11,59,54,81,164,165,153,235,54,59,60,55,165,166,167,155,236,55,60,61,56,167,168,169,157,237,56,61,62,57,169,170,171,159,238,57,62,63,58,171,172,173,161,239,58,63,21,20,173,174,93,163,240,4,27,64,30,100,175,176,103,241,30,64,65,31,176,177,178,104,242,31,65,19,3,178,179,91,105,243,27,28,66,64,101,180,181,175,244,64,66,67,65,181,182,183,177,245,65,67,20,19,183,184,92,179,246,28,29,68,66,102,185,186,180,247,66,68,69,67,186,187,188,182,248,67,69,21,20,188,189,93,184,249,3,19,70,35,91,190,191,109,250,35,70,71,36,191,192,193,110,251,36,71,32,5,193,194,106,111,252,19,20,72,70,92,195,196,190,253,70,72,73,71,196,197,198,192,254,71,73,33,32,198,199,107,194,255,20,21,74,72,93,200,201,195,256,72,74,75,73,201,202,203,197,257,73,75,34,33,203,204,108,199,258]))
    coords=DataArrayDouble([-0.04,0.015,0.,0.,0.015,0.,0.04,0.015,0.,0.,0.105,0.,-0.04,0.105,0.,0.04,0.105,0.,-0.04,0.015,0.015,-0.04,0.015,0.03,-0.04,0.015,0.045,0.,0.015,0.015,0.,0.015,0.03,0.,0.015,0.045,-0.026666666666666665,0.015,0.,-0.013333333333333329,0.015,0.,0.04,0.015,0.015,0.04,0.015,0.03,0.04,0.015,0.045,0.013333333333333336,0.015,0.,0.026666666666666672,0.015,0.,0.,0.105,0.015,0.,0.105,0.03,0.,0.105,0.045,0.,0.03,0.,0.,0.045,0.,0.,0.06,0.,0.,0.075,0.,0.,0.09,0.,-0.04,0.105,0.015,-0.04,0.105,0.03,-0.04,0.105,0.045,-0.026666666666666665,0.105,0.,-0.013333333333333329,0.105,0.,0.04,0.105,0.015,0.04,0.105,0.03,0.04,0.105,0.045,0.013333333333333336,0.105,0.,0.026666666666666672,0.105,0.,-0.026666666666666665,0.015,0.015,-0.013333333333333326,0.015,0.015,-0.026666666666666665,0.015,0.03,-0.013333333333333326,0.015,0.03,-0.026666666666666665,0.015,0.045,-0.013333333333333329,0.015,0.045,0.013333333333333338,0.015,0.015,0.026666666666666675,0.015,0.015,0.013333333333333338,0.015,0.03,0.026666666666666675,0.015,0.03,0.013333333333333336,0.015,0.045,0.026666666666666672,0.015,0.045,0.,0.03,0.015,0.,0.045,0.015,0.,0.06,0.015,0.,0.07500000000000001,0.015,0.,0.09,0.015,0.,0.03,0.03,0.,0.045,0.03,0.,0.06,0.03,0.,0.075,0.03,0.,0.09,0.03,0.,0.03,0.045,0.,0.045,0.045,0.,0.06,0.045,0.,0.075,0.045,0.,0.09,0.045,-0.026666666666666665,0.105,0.015,-0.013333333333333326,0.105,0.015,-0.026666666666666665,0.105,0.03,-0.013333333333333326,0.105,0.03,-0.026666666666666665,0.105,0.045,-0.013333333333333329,0.105,0.045,0.013333333333333338,0.105,0.015,0.026666666666666675,0.105,0.015,0.013333333333333338,0.105,0.03,0.026666666666666675,0.105,0.03,0.013333333333333336,0.105,0.045,0.026666666666666672,0.105,0.045,-0.04,0.015,0.0075,-0.04,0.015,0.0225,-0.04,0.015,0.0375,0.,0.015,0.0075,0.,0.015,0.0225,0.,0.015,0.0375,-0.03333333333333333,0.015,0.,-0.02,0.015,0.,-0.0066666666666666645,0.015,0.,0.04,0.015,0.0075,0.04,0.015,0.0225,0.04,0.015,0.0375,0.006666666666666668,0.015,0.,0.02,0.015,0.,0.03333333333333334,0.015,0.,0.,0.105,0.0075,0.,0.105,0.0225,0.,0.105,0.0375,0.,0.0225,0.,0.,0.0375,0.,0.,0.0525,0.,0.,0.0675,0.,0.,0.0825,0.,0.,0.0975,0.,-0.04,0.105,0.0075,-0.04,0.105,0.0225,-0.04,0.105,0.0375,-0.03333333333333333,0.105,0.,-0.02,0.105,0.,-0.0066666666666666645,0.105,0.,0.04,0.105,0.0075,0.04,0.105,0.0225,0.04,0.105,0.0375,0.006666666666666668,0.105,0.,0.02,0.105,0.,0.03333333333333334,0.105,0.,-0.03333333333333333,0.015,0.015,-0.026666666666666665,0.015,0.0075,-0.02,0.015,0.015,-0.013333333333333327,0.015,0.0075,-0.006666666666666663,0.015,0.015,-0.03333333333333333,0.015,0.03,-0.026666666666666665,0.015,0.0225,-0.02,0.015,0.03,-0.013333333333333326,0.015,0.0225,-0.006666666666666663,0.015,0.03,-0.03333333333333333,0.015,0.045,-0.026666666666666665,0.015,0.0375,-0.02,0.015,0.045,-0.013333333333333327,0.015,0.0375,-0.0066666666666666645,0.015,0.045,0.006666666666666669,0.015,0.015,0.013333333333333336,0.015,0.0075,0.020000000000000007,0.015,0.015,0.026666666666666672,0.015,0.0075,0.03333333333333334,0.015,0.015,0.006666666666666669,0.015,0.03,0.013333333333333338,0.015,0.0225,0.02,0.015,0.03,0.026666666666666675,0.015,0.0225,0.03333333333333334,0.015,0.03,0.006666666666666668,0.015,0.045,0.013333333333333336,0.015,0.0375,0.02,0.015,0.045,0.026666666666666672,0.015,0.0375,0.03333333333333334,0.015,0.045,0.,0.0225,0.015,0.,0.03,0.0075,0.,0.0375,0.015,0.,0.045,0.0075,0.,0.0525,0.015,0.,0.06,0.0075,0.,0.0675,0.015,0.,0.07500000000000001,0.0075,0.,0.0825,0.015,0.,0.09,0.0075,0.,0.0975,0.015,0.,0.0225,0.03,0.,0.03,0.0225,0.,0.0375,0.03,0.,0.045,0.0225,0.,0.0525,0.03,0.,0.06,0.0225,0.,0.0675,0.03,0.,0.07500000000000001,0.0225,0.,0.08249999999999999,0.03,0.,0.09,0.0225,0.,0.0975,0.03,0.,0.0225,0.045,0.,0.03,0.0375,0.,0.0375,0.045,0.,0.045,0.0375,0.,0.0525,0.045,0.,0.06,0.0375,0.,0.0675,0.045,0.,0.075,0.0375,0.,0.08249999999999999,0.045,0.,0.09,0.0375,0.,0.0975,0.045,-0.03333333333333333,0.105,0.015,-0.026666666666666665,0.105,0.0075,-0.02,0.105,0.015,-0.013333333333333327,0.105,0.0075,-0.006666666666666663,0.105,0.015,-0.03333333333333333,0.105,0.03,-0.026666666666666665,0.105,0.0225,-0.02,0.105,0.03,-0.013333333333333326,0.105,0.0225,-0.006666666666666663,0.105,0.03,-0.03333333333333333,0.105,0.045,-0.026666666666666665,0.105,0.0375,-0.02,0.105,0.045,-0.013333333333333327,0.105,0.0375,-0.0066666666666666645,0.105,0.045,0.006666666666666669,0.105,0.015,0.013333333333333336,0.105,0.0075,0.020000000000000007,0.105,0.015,0.026666666666666672,0.105,0.0075,0.03333333333333334,0.105,0.015,0.006666666666666669,0.105,0.03,0.013333333333333338,0.105,0.0225,0.020000000000000007,0.105,0.03,0.026666666666666675,0.105,0.0225,0.03333333333333334,0.105,0.03,0.006666666666666668,0.105,0.045,0.013333333333333336,0.105,0.0375,0.02,0.105,0.045,0.026666666666666672,0.105,0.0375,0.03333333333333334,0.105,0.045,-0.03333333333333333,0.015,0.0075,-0.02,0.015,0.0075,-0.006666666666666664,0.015,0.0075,-0.03333333333333333,0.015,0.0225,-0.02,0.015,0.0225,-0.006666666666666663,0.015,0.0225,-0.03333333333333333,0.015,0.0375,-0.02,0.015,0.0375,-0.006666666666666662,0.015,0.0375,0.006666666666666668,0.015,0.0075,0.02,0.015,0.0075,0.03333333333333334,0.015,0.0075,0.006666666666666669,0.015,0.0225,0.020000000000000007,0.015,0.0225,0.03333333333333334,0.015,0.0225,0.006666666666666668,0.015,0.0375,0.02,0.015,0.0375,0.03333333333333334,0.015,0.0375,0.,0.0225,0.0075,0.,0.0375,0.0075,0.,0.0525,0.0075,0.,0.0675,0.0075,0.,0.0825,0.0075,0.,0.0975,0.0075,0.,0.0225,0.0225,0.,0.0375,0.0225,0.,0.0525,0.0225,0.,0.0675,0.0225,0.,0.0825,0.0225,0.,0.0975,0.0225,0.,0.0225,0.0375,0.,0.0375,0.0375,0.,0.0525,0.0375,0.,0.0675,0.0375,0.,0.08249999999999999,0.0375,0.,0.0975,0.0375,-0.03333333333333333,0.105,0.0075,-0.02,0.105,0.0075,-0.006666666666666664,0.105,0.0075,-0.03333333333333333,0.105,0.0225,-0.02,0.105,0.0225,-0.006666666666666663,0.105,0.0225,-0.03333333333333333,0.105,0.0375,-0.02,0.105,0.0375,-0.006666666666666662,0.105,0.0375,0.006666666666666668,0.105,0.0075,0.02,0.105,0.0075,0.03333333333333334,0.105,0.0075,0.006666666666666669,0.105,0.0225,0.020000000000000007,0.105,0.0225,0.03333333333333334,0.105,0.0225,0.006666666666666668,0.105,0.0375,0.02,0.105,0.0375,0.03333333333333334,0.105,0.0375],259,3)
    coords.setInfoOnComponents(['X [INCONNUE]','Y [INCONNUE]','Z [INCONNUE]'])
    mz.setCoords(coords)
    mz=mz.buildUnstructured()
    #
    arr0=DataArrayDouble()
    vals0=9*[-3593.,85220.,-15343.,-438.,11985.,-15343.,2716.,-61248.,-15343.,5488.,101072.,-16250.,549.,13186.,-16250.,-4389.,-74699.,-16250.,9988.,101646.,-17156.,1537.,14386.,-17156.,-6913.,-72874.,-17156.,22178.,120238.,-19521.,2342.,14222.,-19521.,-17493.,-91793.,-19521.,37737.,139840.,-21887.,3147.,14059.,-21887.,-31443.,-111722.,-21887.,34350.,140668.,-18690.,3714.,16214.,-18690.,-26922.,-108238.,-18690.,34563.,153494.,-15493.,4281.,18370.,-15493.,-26000.,-116753.,-15493.,2147.,115356.,-15418.,1921.,15178.,-15418.,1695.,-84999.,-15418.,12408.,118616.,-17470.,2131.,14700.,-17470.,-8144.,-89215.,-17470.,15624.,100983.,-16718.,2286.,15441.,-16718.,-11050.,-70101.,-16718.,19381.,102680.,-17191.,2505.,16813.,-17191.,-14370.,-69054.,-17191.,22911.,103615.,-17665.,2725.,18184.,-17665.,-17461.,-67245.,-17665.,32377.,125140.,-20005.,4011.,17741.,-20005.,-24353.,-89657.,-20005.,43183.,147067.,-22345.,5298.,17297.,-22345.,-32586.,-112472.,-22345.,41570.,144664.,-22105.,4896.,15874.,-22105.,-31778.,-112915.,-22105.,40070.,142637.,-21866.,4493.,14451.,-21866.,-31083.,-113734.,-21866.,25367.,121066.,-19292.,3390.,14946.,-19292.,-18587.,-91173.,-19292.,28901.,123199.,-19648.,3701.,16343.,-19648.,-21499.,-90512.,-19648.,23716.,103481.,-17470.,2234.,18509.,-17470.,-19247.,-66463.,-17470.,22561.,104808.,-17142.,1238.,20732.,-17142.,-20084.,-63343.,-17142.,21499.,106443.,-16813.,242.,22956.,-16813.,-21014.,-60531.,-16813.,32906.,133441.,-20775.,3873.,26633.,-20775.,-25158.,-80173.,-20775.,45801.,160885.,-24737.,7505.,30311.,-24737.,-30791.,-100262.,-24737.,44184.,154705.,-23601.,5922.,24329.,-23601.,-32340.,-106046.,-23601.,42566.,148521.,-22465.,4338.,18347.,-22465.,-33888.,-111825.,-22465.,32545.,125822.,-19968.,3286.,18428.,-19968.,-25972.,-88965.,-19968.,32703.,129556.,-20371.,3580.,22531.,-20371.,-25542.,-84493.,-20371.,-580.,50781.,-14878.,-188.,10419.,-14878.,203.,-29941.,-14878.,974.,53486.,-13087.,-197.,11696.,-13087.,-1370.,-30094.,-13087.,2154.,54941.,-11297.,-206.,12972.,-11297.,-2567.,-28996.,-11297.,4921.,72039.,-13221.,709.,13770.,-13221.,-3503.,-44499.,-13221.,6472.,88772.,-15144.,1624.,14567.,-15144.,-3223.,-59637.,-15144.,4104.,85353.,-15580.,600.,13047.,-15580.,-2903.,-59258.,-15580.,1258.,80341.,-16016.,-423.,11526.,-16016.,-2105.,-57288.,-16016.,-822.,65212.,-15447.,-306.,10973.,-15447.,210.,-43266.,-15447.,2262.,69336.,-14334.,201.,12371.,-14334.,-1859.,-44593.,-14334.,4047.,55415.,-11562.,137.,13264.,-11562.,-3772.,-28887.,-11562.,6455.,56737.,-11164.,80.,14219.,-11164.,-6294.,-28298.,-11164.,8701.,57516.,-10766.,23.,15175.,-10766.,-8653.,-27166.,-10766.,12843.,74210.,-13196.,662.,16118.,-13196.,-11519.,-41972.,-13196.,17837.,91160.,-15627.,1300.,17062.,-15627.,-15237.,-57034.,-15627.,14573.,90279.,-15674.,1166.,15841.,-15674.,-12240.,-58596.,-15674.,11497.,90026.,-15722.,1033.,14619.,-15722.,-9431.,-60786.,-15722.,7014.,72493.,-13642.,585.,13941.,-13642.,-5844.,-44609.,-13642.,9922.,73330.,-13419.,623.,15030.,-13419.,-8675.,-43270.,-13419.,9608.,58062.,-10666.,-408.,15882.,-10666.,-10425.,-26297.,-10666.,10622.,58358.,-9978.,-497.,16431.,-9978.,-11617.,-25496.,-9978.,11722.,58941.,-9290.,-587.,16979.,-9290.,-12896.,-24981.,-9290.,14386.,76411.,-12130.,-417.,19190.,-12130.,-15221.,-38029.,-12130.,18009.,94168.,-14969.,-248.,21401.,-14969.,-18505.,-51365.,-14969.,19049.,93091.,-15325.,999.,19998.,-15325.,-17050.,-53095.,-15325.,20242.,92527.,-15681.,2246.,18594.,-15681.,-15748.,-55338.,-15681.,14435.,75147.,-13174.,919.,17238.,-13174.,-12597.,-40670.,-13174.,14351.,75579.,-12652.,250.,18214.,-12652.,-13849.,-39150.,-12652.0]
    arr0.setValues(vals0,1458,3)
    arr0.setInfoOnComponents(['SIXX','SIYY','SIYZ'])
    f0z=MEDCouplingFieldDouble(ON_GAUSS_PT)
    f0z.setMesh(mz)
    f0z.setArray(arr0)
    f0z.setGaussLocalizationOnType(NORM_QUAD9,[-1,-1,1,-1,1,1,-1,1,0,-1,1,0,0,1,-1,0,0,0],[-0.774597,-0.774597,0.774597,-0.774597,0.774597,0.774597,-0.774597,0.774597,0,-0.774597,0.774597,0,0,0.774597,-0.774597,0,0,0,-0.774597,-0.774597,0.774597,-0.774597,0.774597,0.774597,-0.774597,0.774597,0,-0.774597,0.774597,0,0,0.774597,-0.774597,0,0,0,-0.774597,-0.774597,0.774597,-0.774597,0.774597,0.774597,-0.774597,0.774597,0,-0.774597,0.774597,0,0,0.774597,-0.774597,0,0,0],[0.308642,0.308642,0.308642,0.308642,0.493827,0.493827,0.493827,0.493827,0.790123,0.308642,0.308642,0.308642,0.308642,0.493827,0.493827,0.493827,0.493827,0.790123,0.308642,0.308642,0.308642,0.308642,0.493827,0.493827,0.493827,0.493827,0.790123])
    f0z.setName("SolutionSIEF_ELGA")
    f0z.checkConsistencyLight()
    ff0z=MEDFileField1TS()
    ff0z.setFieldNoProfileSBT(f0z)
    #
    arr1=DataArrayDouble([158663.,171562.,32597.,37341.,118998.,128979.,88846.,94935.,28786.,32250.,67038.,71287.,102742.,108939.,32907.,37716.,0.,0.,138922.,155768.,41391.,47731.,114491.,128395.,94101.,100197.,31445.,35939.,67322.,71913.,94031.,103701.,34833.,39475.,0.,0.,142412.,160086.,42700.,49041.,113826.,128551.,92705.,102386.,34032.,38580.,61698.,69417.,96177.,104283.,38223.,40570.,0.,0.,87935.,89674.,30482.,34419.,66325.,69150.,53964.,55380.,28306.,32157.,37909.,40395.,53662.,55355.,22264.,24611.,0.,0.,94013.,100457.,31768.,36134.,68065.,73681.,53358.,55305.,23341.,26188.,32996.,35203.,52496.,56792.,23849.,26556.,0.,0.,92797.,102692.,33656.,38268.,61244.,68735.,52594.,56740.,24539.,27146.,29157.,33011.,52853.,57683.,25671.,28315.,0.,0.,53581.,55745.,27972.,31820.,37174.,39935.,29236.,31416.,22475.,25354.,20531.,23672.,25557.,26298.,15741.,17492.,0.,0.,53385.,55725.,23224.,25998.,32556.,35093.,25809.,26619.,16518.,18338.,18447.,21219.,23343.,26173.,16948.,19353.,0.,0.,52384.,56725.,24293.,26951.,29491.,33373.,23510.,26382.,17117.,19543.,19649.,22252.,26670.,30680.,19554.,22512.,0.,0.,158781.,178651.,54413.,62227.,103151.,116693.,96177.,104283.,38223.,40570.,56254.,64331.,92705.,102386.,34032.,38580.,0.,0.,142873.,161033.,43285.,49854.,113088.,127440.,94031.,103701.,34833.,39475.,61888.,69552.,94101.,100197.,31445.,35939.,0.,0.,132908.,149284.,42933.,49391.,109280.,123481.,102742.,108939.,32907.,37716.,75662.,80547.,88846.,94935.,28786.,32250.,0.,0.,96505.,105010.,37990.,40710.,56669.,64741.,52853.,57683.,25671.,28315.,26824.,30727.,52594.,56740.,24539.,27146.,0.,0.,92360.,101577.,33582.,37900.,61559.,69237.,52496.,56792.,23849.,26556.,29094.,32746.,53358.,55305.,23341.,26188.,0.,0.,95615.,101326.,31440.,36083.,69356.,73116.,53662.,55355.,22264.,24611.,31957.,34376.,53964.,55380.,28306.,32157.,0.,0.,53029.,58077.,25636.,28363.,27182.,31122.,26670.,30680.,19554.,22512.,20758.,22139.,23510.,26382.,17117.,19543.,0.,0.,52348.,56723.,23922.,26648.,29286.,32993.,23343.,26173.,16948.,19353.,19615.,22178.,25809.,26619.,16518.,18338.,0.,0.,53623.,55229.,22321.,24800.,32700.,34971.,25557.,26298.,15741.,17492.,18986.,21836.,29236.,31416.,22475.,25354.,0.,0.,64897.,69997.,64897.,69997.,64897.,69997.,81086.,90448.,81086.,90448.,81086.,90448.,85289.,98323.,85289.,98323.,0.,0.,71712.,82585.,71712.,82585.,71712.,82585.,83868.,96651.,83868.,96651.,83868.,96651.,86266.,99505.,86266.,99505.,0.,0.,77671.,89574.,77671.,89574.,77671.,89574.,86057.,99277.,86057.,99277.,86057.,99277.,86664.,99373.,86664.,99373.,0.,0.,80105.,91799.,80105.,91799.,80105.,91799.,86589.,99313.,86589.,99313.,86589.,99313.,85997.,97609.,85997.,97609.,0.,0.,79826.,90350.,79826.,90350.,79826.,90350.,86438.,98180.,86438.,98180.,86438.,98180.,85995.,95152.,85995.,95152.,0.,0.,75137.,82756.,75137.,82756.,75137.,82756.,88617.,98610.,88617.,98610.,88617.,98610.,79961.,83623.,79961.,83623.,0.,0.,79241.,88756.,79241.,88756.,79241.,88756.,84552.,97374.,84552.,97374.,84552.,97374.,93024.,107413.,93024.,107413.,0.,0.,85301.,98315.,85301.,98315.,85301.,98315.,92453.,106752.,92453.,106752.,92453.,106752.,93783.,108035.,93783.,108035.,0.,0.,86813.,100152.,86813.,100152.,86813.,100152.,93530.,107737.,93530.,107737.,93530.,107737.,92512.,106099.,92512.,106099.,0.,0.,87195.,100044.,87195.,100044.,87195.,100044.,92527.,106109.,92527.,106109.,92527.,106109.,90156.,102760.,90156.,102760.,0.,0.,87342.,99233.,87342.,99233.,87342.,99233.,90384.,103001.,90384.,103001.,90384.,103001.,85458.,96494.,85458.,96494.,0.,0.,90428.,100728.,90428.,100728.,90428.,100728.,85454.,96396.,85454.,96396.,85454.,96396.,70692.,78323.,70692.,78323.,0.,0.,84617.,97440.,84617.,97440.,84617.,97440.,83973.,96607.,83973.,96607.,83973.,96607.,92886.,106795.,92886.,106795.,0.,0.,92294.,106564.,92294.,106564.,92294.,106564.,92515.,106364.,92515.,106364.,92515.,106364.,94158.,108033.,94158.,108033.,0.,0.,93639.,107861.,93639.,107861.,93639.,107861.,93950.,107796.,93950.,107796.,93950.,107796.,91874.,105168.,91874.,105168.,0.,0.,92701.,106313.,92701.,106313.,92701.,106313.,91869.,105167.,91869.,105167.,91869.,105167.,87044.,99369.,87044.,99369.,0.,0.,90450.,103104.,90450.,103104.,90450.,103104.,87236.,99605.,87236.,99605.,87236.,99605.,78375.,89211.,78375.,89211.,0.,0.,84833.,95787.,84833.,95787.,84833.,95787.,78665.,89521.,78665.,89521.,78665.,89521.,63448.,71985.,63448.,71985.,0.,0.,9194.,10306.,41521.,44441.,79431.,85178.,8946.,9792.,22335.,23886.,51736.,54740.,7726.,8300.,30477.,32533.,0.,0.,25028.,27404.,28235.,31471.,75379.,84132.,7236.,8309.,29539.,31536.,54052.,58395.,22577.,23370.,39152.,41429.,0.,0.,37235.,42691.,35353.,38183.,100490.,111738.,23433.,25843.,36759.,39109.,53411.,59694.,57437.,65025.,61345.,68878.,0.,0.,6377.,6683.,22457.,23448.,49503.,50751.,10380.,10835.,19266.,19610.,28417.,28750.,20732.,22044.,26521.,27276.,0.,0.,10131.,11681.,28482.,29670.,52601.,56753.,20722.,21728.,25680.,25959.,30780.,31657.,32161.,33290.,33096.,34633.,0.,0.,21380.,22651.,37121.,39923.,54598.,60751.,34744.,36379.,35111.,37417.,35939.,39080.,46708.,51426.,44384.,48624.,0.,0.,10952.,11418.,19643.,20167.,28488.,29118.,20182.,20242.,18359.,18431.,16718.,16900.,24621.,25169.,20737.,21234.,0.,0.,20251.,21309.,25725.,26026.,31355.,32256.,24539.,25102.,20991.,21593.,17889.,18639.,31588.,32417.,27170.,29235.,0.,0.,33431.,35174.,34427.,36736.,35916.,38926.,31741.,32451.,27148.,29040.,23993.,27214.,38993.,40689.,36596.,40772.,0.,0.,41907.,48128.,77947.,87844.,152033.,169876.,57437.,65025.,61345.,68878.,74154.,85533.,23433.,25843.,36759.,39109.,0.,0.,33932.,38310.,36200.,40713.,99297.,111743.,22577.,23370.,39152.,41429.,57432.,62292.,7236.,8309.,29539.,31536.,0.,0.,22968.,26297.,27645.,30092.,72463.,80413.,7726.,8300.,30477.,32533.,57469.,61055.,8946.,9792.,22335.,23886.,0.,0.,48710.,53918.,59260.,65402.,77350.,87781.,46708.,51426.,44384.,48624.,50354.,58143.,34744.,36379.,35111.,37417.,0.,0.,20908.,22670.,38133.,39669.,56332.,59453.,32161.,33290.,33096.,34633.,34367.,36793.,20722.,21728.,25680.,25959.,0.,0.,7404.,8010.,30073.,32272.,55210.,58955.,20732.,22044.,26521.,27276.,32434.,32943.,10380.,10835.,19266.,19610.,0.,0.,45425.,49667.,44460.,48383.,51014.,58851.,38993.,40689.,36596.,40772.,38865.,44837.,31741.,32451.,27148.,29040.,0.,0.,31595.,32602.,33322.,34837.,35379.,37698.,31588.,32417.,27170.,29235.,24035.,27311.,24539.,25102.,20991.,21593.,0.,0.,19475.,20577.,25384.,25965.,31447.,32050.,24621.,25169.,20737.,21234.,17759.,18414.,20182.,20242.,18359.,18431.,0.,0.],486,2)
    arr1.setInfoOnComponents(['VMIS','TRESCA'])
    f1z=MEDCouplingFieldDouble(ON_GAUSS_NE) ; f1z.setName("SolutionSIEQ_ELNO")
    f1z.setArray(arr1)
    f1z.setMesh(mz)
    f1z.checkConsistencyLight()
    ff1z=MEDFileField1TS()
    ff1z.setFieldNoProfileSBT(f1z)
    #
    arr2=DataArrayDouble([4.938269266683534e-28,-6.232001151245993e-28,1.0602290566170399e-27,4.029857051193646e-29,-1.1103217240985741e-27,-1.5536615528327828e-27,1.5241233363338648e-28,-3.015674201798923e-30,-1.02980203874559e-30,2.72351596604712e-29,-1.1888246600739933e-29,-2.69937196846485e-29,-4.7437993223547586e-07,1.3496193329105202e-07,-7.295395086913946e-07,7.054959528904149e-08,-8.433644218153947e-07,-1.3140517414252605e-08,-4.6699282590396373e-07,-4.307881601165781e-21,-7.181557182928824e-07,-8.028640214851634e-21,-8.231757491716489e-07,-1.2311604671716488e-20,6.500213859021137e-28,-5.36425415550288e-29,2.082592789783471e-28,1.3252863207712998e-28,-4.743799322354692e-07,-1.3496193329105909e-07,-7.295395086913853e-07,-7.054959528905818e-08,-8.433644218153849e-07,1.3140517414226876e-08,6.058451752097371e-28,5.632466863278024e-28,2.208810534618833e-28,2.2876966251409342e-28,-6.120655339318954e-07,1.623023866978208e-20,-7.136250920460284e-07,4.359903572644498e-20,-6.995066975751433e-07,7.373674095935513e-20,0.0,1.4598897976731362e-43,0.0,5.997557427310217e-43,0.0,1.156071233067974e-43,0.0,6.726232628759122e-44,0.0,1.0089348943138683e-43,-4.76082153061813e-07,1.7635130038063272e-07,-7.252650834474581e-07,1.797684522608815e-07,-7.726301925249592e-07,1.3593814652780474e-07,1.28713440720985e-28,8.66262871593868e-30,-1.7918024758329104e-28,1.9633850833341234e-29,-4.7608215306193535e-07,-1.7635130038060178e-07,-7.252650834476241e-07,-1.797684522607695e-07,-7.726301925251186e-07,-1.3593814652760442e-07,-3.0106055763582786e-29,-3.568658720396775e-29,-2.2739766223848432e-29,-1.6603258665331888e-29,-4.741010514018535e-07,5.448590612379579e-08,-4.701818411901992e-07,1.6663000490694165e-08,-7.453102534678627e-07,2.7357217239441644e-08,-7.279540615613018e-07,3.4371113890668217e-09,-8.579330208754989e-07,-2.6008405171390205e-08,-8.357526525453042e-07,-2.5729239758795502e-08,-4.701818411901952e-07,-1.6663000490702818e-08,-4.741010514018457e-07,-5.448590612380402e-08,-7.279540615612966e-07,-3.4371113890835274e-09,-7.45310253467852e-07,-2.7357217239458564e-08,-8.357526525452993e-07,2.572923975877074e-08,-8.579330208754882e-07,2.6008405171364682e-08,0.0,-3.689838846534034e-21,0.0,-3.4057248282497575e-21,0.0,-2.389474304570763e-21,0.0,1.1849907150798524e-21,0.0,7.728930340233592e-21,0.0,-9.409753747161713e-21,0.0,-9.27355993916331e-21,0.0,-5.9005465375222355e-21,0.0,3.45820724462685e-21,0.0,2.0021147143626658e-20,0.0,-1.6042705688202718e-20,0.0,-1.541762360721681e-20,0.0,-8.827856096357078e-21,0.0,6.3984676397065826e-21,0.0,3.396409774956617e-20,-5.21059344905059e-07,1.0785416831218214e-07,-5.942832234061152e-07,1.1284412735292002e-07,-7.291198556954883e-07,1.5975182005082218e-07,-7.29419534669376e-07,1.4146009988497262e-07,-7.536819640264805e-07,1.388363757438669e-07,-7.212688685979149e-07,1.1197909398607218e-07,-5.94283223406162e-07,-1.1284412735287996e-07,-5.210593449051535e-07,-1.0785416831213956e-07,-7.294195346694393e-07,-1.4146009988486582e-07,-7.291198556956173e-07,-1.5975182005070884e-07,-7.212688685979762e-07,-1.1197909398589052e-07,-7.536819640266044e-07,-1.3883637574367513e-07,-2.8576578578864835e-07,1.301140611795876e-07,-6.228581335977658e-07,1.1144407150470386e-07,-8.014360278229913e-07,2.882237382538957e-08,-2.6588034168431343e-07,-2.345549761149889e-21,-6.148909793824232e-07,-5.981322827533954e-21,-7.85277352863385e-07,-1.0225807644204478e-20,3.1049565229499025e-27,-4.196740015775783e-28,-2.887230913108903e-28,-4.403551231441193e-28,4.812051521848172e-29,3.851635443793833e-28,-2.857657857886445e-07,-1.301140611795897e-07,-6.228581335977572e-07,-1.1144407150471579e-07,-8.01436027822981e-07,-2.8822373825410947e-08,-1.5635223141480454e-27,-9.802651538976083e-29,3.278505922098525e-27,-6.797558254135845e-28,4.1020767071492614e-27,5.932234007262009e-28,-4.410314084022777e-07,7.268022452197178e-21,-6.915240267186082e-07,2.9979998916103056e-20,-7.135613152026231e-07,5.968769864060137e-20,0.0,1.793662034335766e-43,0.0,1.793662034335766e-43,0.0,8.96831017167883e-43,0.0,-5.6519283938616335e-43,0.0,1.0761972206014595e-42,0.0,9.269247757898544e-43,-2.627131752301911e-07,1.3274214899302934e-07,-6.329945799890571e-07,1.9265484727832687e-07,-7.677771910976133e-07,1.6184720841148966e-07,1.5146129380243427e-28,-5.494056542378401e-29,1.0988346067719275e-50,3.891714187734575e-29,2.524354896707238e-28,1.6407579324035087e-28,-2.62713175230267e-07,-1.3274214899303003e-07,-6.329945799892127e-07,-1.9265484727825895e-07,-7.677771910977852e-07,-1.6184720841133102e-07,-1.5146129380243427e-28,6.686637000472881e-29,-1.5146129380243427e-28,5.412840523535707e-29,5.048709949123486e-29,-1.0255191767873153e-29,-4.801584184334589e-07,9.447106112382042e-08,-2.646155245804423e-07,4.165624981437424e-08,-4.7283844479037397e-07,3.112099903692373e-08,-2.656912190102633e-07,1.2633757629192664e-08,-4.6803908104000066e-07,8.091752496288301e-09,-7.484767456103177e-07,4.8063930505815753e-08,-6.342336387597463e-07,4.783223809223803e-08,-7.371756425756432e-07,1.2491484933343017e-08,-6.217198644652372e-07,1.3160740615455145e-08,-7.210619794360716e-07,-6.140998451021678e-11,-8.616398069136549e-07,-2.0279767975595878e-08,-8.174867338143947e-07,1.761743718501197e-09,-8.474965025861739e-07,-2.8360688201075687e-08,-7.969390370704656e-07,-1.0156396041453297e-08,-8.26809188564297e-07,-1.6608029814857525e-08,-4.6803908103999843e-07,-8.091752496296909e-09,-2.6569121901026077e-07,-1.2633757629197396e-08,-4.72838444790368e-07,-3.112099903693232e-08,-2.6461552458043755e-07,-4.165624981437841e-08,-4.801584184334501e-07,-9.447106112382803e-08,-7.210619794360687e-07,6.140998449395655e-11,-6.217198644652322e-07,-1.3160740615467643e-08,-7.37175642575635e-07,-1.2491484933360037e-08,-6.342336387597367e-07,-4.783223809225061e-08,-7.484767456103061e-07,-4.8063930505832595e-08,-8.268091885642944e-07,1.6608029814832955e-08,-7.969390370704601e-07,1.015639604143253e-08,-8.474965025861654e-07,2.836068820105083e-08,-8.174867338143834e-07,-1.7617437185225845e-09,-8.616398069136434e-07,2.02797679755703e-08,0.0,-3.435877556189529e-21,0.0,-1.3776771401529971e-21,0.0,-3.403994495899553e-21,0.0,-1.3185754926235634e-21,0.0,-2.7680254878980847e-21,0.0,-1.1103673324958778e-21,0.0,-9.900107724251277e-22,0.0,4.77602156134974e-22,0.0,3.959146546409973e-21,0.0,3.776411006916986e-21,0.0,1.2124392797414627e-20,0.0,-8.731709783191639e-21,0.0,-6.144329300702704e-21,0.0,-8.89968038381516e-21,0.0,-5.404653063609732e-21,0.0,-7.547524363424233e-21,0.0,-3.3206893791145496e-21,0.0,-1.6246546029348425e-21,0.0,1.9854969588545943e-21,0.0,1.1317236337409485e-20,0.0,1.3983874347413824e-20,0.0,3.098996503833694e-20,0.0,-1.407865292807831e-20,0.0,-1.1626000835692668e-20,0.0,-1.533412146384094e-20,0.0,-1.1018823571603408e-20,0.0,-1.2512374860235445e-20,0.0,-6.514267779823197e-21,0.0,-2.4719015385819347e-21,0.0,4.858859984935085e-21,0.0,1.881348220188166e-20,0.0,2.740874292245713e-20,0.0,5.211376231395407e-20,-4.864225626157067e-07,1.3296728230814142e-07,-3.076586015279749e-07,6.740802636874205e-08,-5.594534933091086e-07,1.1066583698276748e-07,-3.841186138586638e-07,7.559745482599735e-08,-6.098267681100678e-07,9.875238991849593e-08,-7.2970765755297e-07,1.669225383108583e-07,-6.573316628421653e-07,1.465581454371536e-07,-7.316181481946474e-07,1.5542337568784254e-07,-6.912676702186072e-07,1.3829023572925456e-07,-7.20589385417476e-07,9.666451163676767e-08,-7.723884127434406e-07,1.3857930563622999e-07,-7.569169347428396e-07,1.547516335925935e-07,-7.383025429990218e-07,1.3271074092312e-07,-7.350854918831709e-07,1.2994146610617741e-07,-7.0646090437235e-07,6.938656131041383e-08,-6.098267681100908e-07,-9.875238991845998e-08,-3.841186138586908e-07,-7.559745482597872e-08,-5.594534933091803e-07,-1.1066583698272561e-07,-3.0765860152803245e-07,-6.740802636872275e-08,-4.864225626158294e-07,-1.329672823081032e-07,-7.20589385417508e-07,-9.666451163666813e-08,-6.912676702186662e-07,-1.3829023572918171e-07,-7.316181481947439e-07,-1.5542337568773118e-07,-6.573316628422832e-07,-1.4655814543707692e-07,-7.297076575531353e-07,-1.6692253831074535e-07,-7.064609043723808e-07,-6.938656131024667e-08,-7.350854918832338e-07,-1.2994146610603196e-07,-7.383025429991161e-07,-1.3271074092293177e-07,-7.56916934742969e-07,-1.5475163359243975e-07,-7.723884127435972e-07,-1.3857930563603445e-07,-2.6225523866596743e-07,7.368618860781533e-08,-2.6483901297419715e-07,2.3043552983380804e-08,-2.6569368384400127e-07,6.351605004005378e-09,-6.372294401265312e-07,7.723333950374213e-08,-6.279151194946851e-07,2.6473214771574048e-08,-6.169026138715963e-07,5.559932643680484e-09,-8.206671947434617e-07,1.419786412636469e-08,-8.077626999789484e-07,-6.830966026358984e-09,-7.886512424652054e-07,-7.805683188152915e-09,-2.6569368384400005e-07,-6.351605004010108e-09,-2.648390129741935e-07,-2.3043552983385364e-08,-2.622552386659623e-07,-7.36861886078186e-08,-6.169026138715935e-07,-5.559932643692688e-09,-6.279151194946777e-07,-2.647321477158676e-08,-6.372294401265209e-07,-7.723333950375434e-08,-7.886512424652024e-07,7.805683188132495e-09,-8.077626999789395e-07,6.830966026337777e-09,-8.2066719474345e-07,-1.4197864126386033e-08,0.0,-1.528080794868089e-21,0.0,-1.5426894495622164e-21,0.0,-1.1495211961775125e-21,0.0,-4.441807280413718e-22,0.0,1.9784166326767865e-21,0.0,5.420802477152069e-21,0.0,-6.12858914756041e-21,0.0,-6.197208509921192e-21,0.0,-4.895012827081806e-21,0.0,-1.2104071985400557e-21,0.0,7.207223659623184e-21,0.0,2.1139789648778443e-20,0.0,-1.178112680978544e-20,0.0,-1.2361618534325795e-20,0.0,-9.332953007651742e-21,0.0,-2.4549106998298197e-21,0.0,1.5192320364423287e-20,0.0,4.128606459003346e-20,-2.6179012371741095e-07,8.492155525093568e-08,-3.431936650750607e-07,6.526689580555302e-08,-4.206987870183037e-07,7.275786337707596e-08,-6.39565352471141e-07,1.6214246375922057e-07,-6.76924119958337e-07,1.4588367240696607e-07,-6.94035042843604e-07,1.042901908413762e-07,-7.689004953903582e-07,1.574343097081281e-07,-7.476782438785353e-07,1.489103868358007e-07,-7.209785981316606e-07,8.394206942691553e-08,-4.2069878701831687e-07,-7.275786337705938e-08,-3.431936650751034e-07,-6.52668958055331e-08,-2.617901237174839e-07,-8.492155525092188e-08,-6.940350428436335e-07,-1.0429019084130907e-07,-6.769241199584262e-07,-1.458836724068901e-07,-6.39565352471292e-07,-1.6214246375914686e-07,-7.209785981316918e-07,-8.394206942678136e-08,-7.476782438786325e-07,-1.4891038683565012e-07,-7.68900495390525e-07,-1.574343097079712e-07],259,2)
    arr2.setInfoOnComponents(['DRX','DRZ'])
    f2z=MEDCouplingFieldDouble(ON_NODES) ; f2z.setName("SolutionDEPL") ; f2z.setArray(arr2)
    f2z.setMesh(mz)
    f2z.checkConsistencyLight()
    ff2z=MEDFileField1TS()
    ff2z.setFieldNoProfileSBT(f2z)
    #
    mm2=MEDFileUMesh()
    mm2.setMeshAtLevel(0,mz)
    mm2.setFamilyFieldArr(0,DataArrayInt([-1,-1,-1,-1,-1,-1,-1,-1,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-4,-4,-4,-4,-4,-4,-4,-4,-4,-5,-5,-5,-5,-5,-5,-5,-5,-5]))
    mm2.setFamilyFieldArr(1,DataArrayInt([1,2,3,4,5,6,0,0,0,0,0,0,1,1,0,0,0,3,3,0,0,0,7,7,7,7,7,0,0,0,5,5,0,0,0,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,3,3,3,0,0,0,7,7,7,7,7,7,0,0,0,5,5,5,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]))
    for famName,famId in [('FAMILLE_ZERO',0),('ba1_beam',-1),('ba2_beam',-2),('h1',6),('h1_h2_h3',4),('h2',5),('h3',7),('h3_h4_h5',2),('h4',3),('h5',1),('to1_beam',-4),('to2_beam',-5),('web_beam',-3)]:
        mm2.setFamilyId(famName,famId)
        pass
    for grName,famsOnGrp in [('ba1',['ba1_beam']),('ba2',['ba2_beam']),('beam',['ba1_beam','ba2_beam','to1_beam','to2_beam','web_beam']),('h1',['h1','h1_h2_h3']),('h2',['h1_h2_h3','h2']),('h3',['h1_h2_h3','h3','h3_h4_h5']),('h4',['h3_h4_h5','h4']),('h5',['h3_h4_h5','h5']),('to1',['to1_beam']),('to2',['to2_beam']),('web',['web_beam'])]:
        mm2.setFamiliesOnGroup(grName,famsOnGrp)
        pass
    #
    mm2.write(fname,2)
    ff0z.write(fname,0)
    ff1z.write(fname,0)
    ff2z.write(fname,0)
    return fname

@WriteInTmpDir
def test0(baseline_file):
    fname = GenerateCase()
    ################### MED write is done -> Go to MEDReader

    myMedReader=MEDReader(FileName=fname)
    keys=myMedReader.GetProperty("FieldsTreeInfo")[::2]
    # list all the names of arrays that can be seen (including their spatial discretization)
    arr_name_with_dis=[elt.split("/")[-1] for elt in keys]
    # list all the names of arrays (Equal to those in the MED File)
    arr_name=[elt.split(myMedReader.GetProperty("Separator").GetData())[0] for elt in arr_name_with_dis]
    myMedReader.AllArrays=keys

    if '-D' not in sys.argv:
        RenderView1 = GetRenderView()
        ELNOfieldToSurface1=ELNOfieldToSurface(Input=myMedReader)
        ExtractGroup1=ExtractGroup(Input=ELNOfieldToSurface1)
        #ExtractGroup1.UpdatePipelineInformation()
        ExtractGroup1.AllGroups=['GRP_ba2','GRP_to1','GRP_web']
        assert(isinstance(ExtractGroup1.GetProperty("MeshName")[0],str))
        assert(ExtractGroup1.GetProperty("MeshName")[0]=="mesh")
        #
        DataRepresentation3 = Show()
        DataRepresentation3.ScaleFactor = 0.008999999705702066
        DataRepresentation3.EdgeColor = [0.0, 0.0, 0.5000076295109483]
        DataRepresentation3.SelectionPointFieldDataArrayName = 'SolutionSIEF_ELNO'
        a2_SolutionSIEQ_ELNO_PiecewiseFunction = CreatePiecewiseFunction(Points=[0.0, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0])
        #VectorMode='Magnitude' or VectorMode='Component'
        a2_SolutionSIEQ_ELNO_PVLookupTable = GetLookupTableForArray("SolutionSIEQ_ELNO",2,RGBPoints=[0.0, 0.23, 0.299, 0.754, 239013.7773476667, 0.706, 0.016, 0.15], VectorMode='Component', VectorComponent=1, NanColor=[0.25, 0.0, 0.0], ColorSpace='Diverging', ScalarRangeInitialized=1.0, AllowDuplicateScalars=1 )
        DataRepresentation3.ScalarOpacityFunction = a2_SolutionSIEQ_ELNO_PiecewiseFunction
        DataRepresentation3.ColorArrayName = 'SolutionSIEQ_ELNO'
        DataRepresentation3.LookupTable = a2_SolutionSIEQ_ELNO_PVLookupTable
        DataRepresentation3.Visibility = 1
        #
        ELGAfieldToPointGaussian1=ELGAfieldToPointGaussian(Input=ELNOfieldToSurface1)
        ELGAfieldToPointGaussian1.SelectSourceArray=['CELLS','ELGA@0']
        DataRepresentation4 = Show()
        DataRepresentation4.ScaleFactor = 0.008999999705702066
        DataRepresentation4.EdgeColor = [0.0, 0.0, 0.5000076295109483]
        DataRepresentation4.SelectionPointFieldDataArrayName = 'SolutionSIEF_ELGA'
        DataRepresentation4.ColorArrayName = 'SolutionSIEF_ELGA'
        ########
        RenderView1.CameraViewUp = [-0.19545466285945437, 0.837274140321886, -0.5106559396646081]
        RenderView1.CameraPosition = [0.11797550069274401, 0.20119836056342144, 0.20885419432082736]
        RenderView1.CameraFocalPoint = [1.0170565790969026e-18, 0.0599999981932342, 0.022500000894069675]
        RenderView1.ViewSize =[300,300]
        Render()

        # compare with baseline image
        import vtk.test.Testing
        from vtk.util.misc import vtkGetTempDir
        vtk.test.Testing.VTK_TEMP_DIR = vtk.util.misc.vtkGetTempDir()
        vtk.test.Testing.compareImage(GetActiveView().GetRenderWindow(), baseline_file,
                                                                    threshold=1)
        vtk.test.Testing.interact()

if __name__ == "__main__":
    outImgName="testMEDReader0.png"
    baseline_file = RetriveBaseLine(outImgName)
    test0(baseline_file)
    pass
