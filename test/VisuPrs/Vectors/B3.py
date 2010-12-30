# This case corresponds to: /visu/Vectors/B3 case

import sys

from paravistest import datadir
from presentations import VectorsOnField, EntityType
import paravis
import pvsimple

my_paravis = paravis.myParavis

#====================Stage1: Importing MED file====================

print "**** Stage1: Importing MED file"

print 'Import "ResOK_0000_236.med"...............',
medFile = datadir + "ResOK_0000_236.med"
my_paravis.ImportFile(medFile)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

cell_entity = EntityType.CELL
node_entity = EntityType.NODE

#====================Stage2: Creation of Vectors====================

print "Creating Vectors..........",
vectors = VectorsOnField(med_reader, node_entity, 'vitesse', 1)

if vectors is None:
    print "FAILED"
else:
    print "OK"

#====================Stage3: Change properties======================
glyph = vectors.Input

print "Default LineWidth : ", vectors.LineWidth
print "Default GlyphType : ", glyph.GlyphType
print "Default subtype   : ", glyph.GlyphType.GlyphType
print "Default GlyphPos  : ", glyph.GlyphType.Center

print "Changing value of line width"
width = -10
vectors.LineWidth = width
print "Negative value -10 : ", vectors.LineWidth
pvsimple.Render()

width = 10
vectors.LineWidth = width
print "Positive value  10 : ", vectors.LineWidth
pvsimple.Render()

width = 0
vectors.LineWidth = width
print "Zero value       0 : ", vectors.LineWidth
pvsimple.Render()

width = 2
vectors.LineWidth = width
print "Positive value   2 : ", vectors.LineWidth
pvsimple.Render()

print "Changing glyph type"
gtype = "Arrow"
glyph.GlyphType = gtype
print "Arrow type     : ", glyph.GlyphType
pvsimple.Render()

gtype = "Cone"
glyph.GlyphType = gtype
glyph.GlyphType.Resolution = 3
glyph.GlyphType.Height = 4
print "Cone type: " + glyph.GlyphType + ", resolution: " + glyph.GlyphType.Resolution
pvsimple.Render()

glyph.GlyphType.Resolution = 6
print "Cone type: " + glyph.GlyphType + ", resolution: " + glyph.GlyphType.Resolution
pvsimple.Render()

gtype = "Line"
glyph.GlyphType = gtype
print "Line Type      : ", glyph.GlyphType
pvsimple.Render()

gtype = "2D Glyph"
glyph.GlyphType = gtype
glyph.GlyphType.GlyphType = "Arrow"
pvsimple.Render()

print "Changing glyph position"
pos = [0.0, 0.0, 0.0]
glyph.GlyphType.Center = pos
print "Center position : ", glyph.GlyphType.Center
pvsimple.Render()

pos = [0.5, 0.0, 0.0]
glyph.GlyphType.Center = pos
print "Tail position   : ", glyph.GlyphType.Center
pvsimple.Render()

pos = [-0.5, 0.0, 0.0]
glyph.GlyphType.Center = pos
print "Head position   : ", glyph.GlyphType.Center
pvsimple.Render()
