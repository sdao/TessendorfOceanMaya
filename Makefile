#-
# ==========================================================================
# Copyright (c) 2011 Autodesk, Inc.
# All rights reserved.
# 
# These coded instructions, statements, and computer programs contain
# unpublished proprietary information written by Autodesk, Inc., and are
# protected by Federal copyright law. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without
# the prior written consent of Autodesk, Inc.
# ==========================================================================
#+

ifndef INCL_BUILDRULES

#
# Include platform specific build settings
#
TOP := ..
include $(TOP)/buildrules


#
# Always build the local plug-in when make is invoked from the
# directory.
#
all : plugins

endif

#
# Variable definitions
#

SRCDIR := $(TOP)/oceanNode
DSTDIR := $(TOP)/oceanNode

oceanNode_SOURCES  := $(TOP)/oceanNode/*.cpp
oceanNode_OBJECTS  := $(TOP)/oceanNode/oceanNode.o
oceanNode_PLUGIN   := $(DSTDIR)/oceanNode.$(EXT)
oceanNode_MAKEFILE := $(DSTDIR)/Makefile

#
# Include the optional per-plugin Makefile.inc
#
#    The file can contain macro definitions such as:
#       {pluginName}_EXTRA_CFLAGS
#       {pluginName}_EXTRA_C++FLAGS
#       {pluginName}_EXTRA_INCLUDES
#       {pluginName}_EXTRA_LIBS
-include $(SRCDIR)/Makefile.inc


#
# Set target specific flags.
#

$(oceanNode_OBJECTS): CFLAGS   := $(CFLAGS)   $(oceanNode_EXTRA_CFLAGS)
$(oceanNode_OBJECTS): C++FLAGS := $(C++FLAGS) $(oceanNode_EXTRA_C++FLAGS)
$(oceanNode_OBJECTS): INCLUDES := $(INCLUDES) $(oceanNode_EXTRA_INCLUDES)

depend_oceanNode:     INCLUDES := $(INCLUDES) $(oceanNode_EXTRA_INCLUDES)

$(oceanNode_PLUGIN):  LFLAGS   := $(LFLAGS) $(oceanNode_EXTRA_LFLAGS) 
$(oceanNode_PLUGIN):  LIBS     := $(LIBS)   -lOpenMaya -lFoundation $(oceanNode_EXTRA_LIBS) 

#
# Rules definitions
#

.PHONY: depend_oceanNode clean_oceanNode Clean_oceanNode


$(oceanNode_PLUGIN): $(oceanNode_OBJECTS) 
	-rm -f $@
	$(LD) -o $@ $(LFLAGS) $^ $(LIBS)

depend_oceanNode :
	makedepend $(INCLUDES) $(MDFLAGS) -f$(DSTDIR)/Makefile $(oceanNode_SOURCES)

clean_oceanNode:
	-rm -f $(oceanNode_OBJECTS)

Clean_oceanNode:
	-rm -f $(oceanNode_MAKEFILE).bak $(oceanNode_OBJECTS) $(oceanNode_PLUGIN)


plugins: $(oceanNode_PLUGIN)
depend:	 depend_oceanNode
clean:	 clean_oceanNode
Clean:	 Clean_oceanNode

