try: paraview.simple
except: from paraview.simple import *
paraview.simple._DisableFirstRenderCameraReset()

src = GetActiveSource()
src_rep = GetRepresentation(src)
src_rep.Visibility = 0

surface = ExtractSurface()

warp = WarpByVector()
warp_repr = Show()

info = surface.GetDataInformation()
bounds = info.DataInformation.GetBounds()
side = [bounds[1] - bounds[0], bounds[3]-bounds[2], bounds[5]-bounds[4]]
len = max(max(side[0], side[1]), side[2])

scale_factor = len / 20.0

scene = GetAnimationScene()

PI = 3.14159
scene.PlayMode = 'Sequence'
scene.StartTime = -PI
scene.EndTime = PI
scene.NumberOfFrames = 20
scene.Loop = 1
time_cue = scene.Cues[0]
time_cue.Enabled = 0

track = GetAnimationTrack("ScaleFactor")
keyf0 = CompositeKeyFrame()
keyf0.Interpolation='Sinusoid'
keyf0.KeyTime = 0
keyf0.KeyValues = [scale_factor]

keyf1 = CompositeKeyFrame()
keyf1.Interpolation='Sinusoid'
keyf1.KeyTime = 1
keyf1.KeyValues = [scale_factor]

track.KeyFrames = [keyf0, keyf1]

scene.Play()
