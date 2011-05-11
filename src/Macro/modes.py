try: paraview.simple
except: from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()

source = GetActiveSource()
representation = GetDisplayProperties(source)
representation.Visibility = 0

if source.SMProxy.GetVTKClassName() == 'vtkMedReader' :
  source.AnimationMode = 'Modes'

ExtractSurface1 = ExtractSurface()
ScaleVector1 = ScaleVector()
WarpByVector1 = WarpByVector()

# first, find the first point-centered vector array.
ExtractSurface1.UpdatePipeline()
info = ExtractSurface1.GetDataInformation()
pinfo = info.DataInformation.GetPointDataInformation()
arrayinfo = None
vectorname = None
rootname = None
for arrayid in range(0, pinfo.GetNumberOfArrays()) :
  arrayinfo = pinfo.GetArrayInformation(arrayid)
  if arrayinfo.GetNumberOfComponents() == 3 : 
    vectorname = arrayinfo.GetName()
    rootpos = vectorname.rfind("[")
    if rootpos == -1 :     
      rootname = vectorname
    else :
      rootname = vectorname[0:rootpos-1]
    ScaleVector1.VectorField = ['POINTS', vectorname]
    WarpByVector1.Vectors = ['POINTS', rootname]
    break

ScaleVector1.ScaleFactor = 0
ScaleVector1.UpdatePipeline()
bounds = info.DataInformation.GetBounds()
side = [bounds[1] - bounds[0], bounds[3] - bounds[2], bounds[5] - bounds[4]]
length = side[0]
if side[1] > length : length = side[1] 
if side[2] > length : length = side[2] 

scale = length / 20
if vectorname != None :
  arrayrange = arrayinfo.GetComponentRange(-1)
  if arrayrange[1] > 0 :
    scale = scale / arrayrange[1]

WarpByVector1.ScaleFactor = scale

AnimationScene1 = GetAnimationScene()

TimeAnimationCue1 = GetTimeTrack()
TimeAnimationCue1.Enabled = 0

KeyFrameAnimationCue1 = GetAnimationTrack( ScaleVector1.GetProperty('ScaleFactor'))
KeyFrame0 = CompositeKeyFrame( KeyValues=[1.0], Interpolation='Sinusoid' )
KeyFrame1 = CompositeKeyFrame( KeyTime=1.000000001, KeyValues=[1.0] )

KeyFrameAnimationCue1.KeyFrames = [ KeyFrame0, KeyFrame1 ]

AnimationScene1.Cues.append(KeyFrameAnimationCue1)
AnimationScene1.Loop = 1
AnimationScene1.PlayMode = 'Sequence'
AnimationScene1.NumberOfFrames = 21

WarpByVectorDataRepresentation = Show(WarpByVector1)
if rootname != None :
  pvLookupTable = GetLookupTableForArray( rootname, 3, VectorMode='Magnitude' )
  WarpByVectorDataRepresentation.ColorArrayName = rootname
  WarpByVectorDataRepresentation.LookupTable = pvLookupTable

AnimationScene1.Play()