################################################################
# Rebarlinx
# Copyright (C) 2018   Michael W Hoag
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Rebarlinx License, Version 1.0
################################################################

VEE_VER_MAJ      = 4
VEE_VER_MIN      = 2
VEE_VER_PAT      = 0
VEE_VERSION      = $${REE_VER_MAJ}.$${REE_VER_MIN}.$${REE_VER_PAT}

DEFINES += VERSION_MAJOR="$$VEE_VER_MAJ"\
       VERSION_MINOR="$$VEE_VER_MIN"\
       VERSION_PATCH="$$VEE_VER_PAT"\
#        R_DEBUG\     #used as debug macro version


