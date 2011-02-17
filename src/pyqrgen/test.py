#!/usr/bin/env python

import math
import cairo

import pyqrgen

WIDTH, HEIGHT = 200, 200

surface = cairo.ImageSurface(cairo.FORMAT_ARGB32, WIDTH, HEIGHT)

ctx = cairo.Context(surface)

ctx.scale(WIDTH, HEIGHT)

ctx.rectangle(0, 0, 1, 1)

ctx.set_source_rgb(0.7, 0.5, 0)

ctx.fill()

pyqrgen.generate(ctx)

surface.write_to_png("example.png")
