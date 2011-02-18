#!/usr/bin/env python

import cairo

import pyqrgen

SIZE = 200

surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, SIZE, SIZE)

ctx = cairo.Context(surface)

#ctx.scale(WIDTH, HEIGHT)

ctx.rectangle(0, 0, SIZE, SIZE)

ctx.set_source_rgb(1, 1, 1)

ctx.fill()

pyqrgen.generate("green pride", 0, 4, 2, ctx, SIZE)


surface.write_to_png("example.png")
