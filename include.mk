# if include.local.mk exists, include it first to set various options
# it shuld not be checked in
includeLocal = ${rootDir}/include.local.mk
ifneq ($(wildcard ${includeLocal}),)
   include ${includeLocal}
endif

binDir =${rootDir}/bin
libDir = ${rootDir}/lib
objDir = ${rootDir}/objs

libHal = ${libDir}/libHal.a
libHalStats = ${libDir}/libHalStats.a
libHalBlockViz = ${libDir}/libHalBlockViz.a
libHalMutations = ${libDir}/libHalMutations.a
libHalLiftover = ${libDir}/libHalLiftover.a
libHalLod = ${libDir}/libHalLod.a
libHalMaf = ${libDir}/libHalMaf.a

inclSpec += -I${rootDir}/api/inc -Iimpl -Iinc -I${rootDir}/liftover/inc

#Modify this variable to set the location of sonLib
#(sonlib is used only for cuTest at this potin)
sonLibRootDir ?= ${rootDir}/../sonLib
sonLibDir = ${sonLibRootDir}/lib

# update PYTHONPATH for tests
export PYTHONPATH := $(abspath ${rootDir}/..):${PYTHONPATH}

export PATH := $(abspath ${rootDir}/bin):${PATH}

.SECONDARY: 

include  ${sonLibRootDir}/include.mk

dataSetsDir=/Users/hickey/Documents/Devel/genomes/datasets

#
# The -D_GLIBCXX_USE_CXX11_ABI=1 flag prevents errors with
#   std::__cxx11::basic_string vs std::basic_string
# vs
#   std::basic_string vs std::basic_string
# when linking with HDF5 and modern C++ compilers (well, GCC 7.3)
#
ifeq (${CXX_11_ABI_DEF},)
    CXX_11_ABI_DEF = -D_GLIBCXX_USE_CXX11_ABI=1
endif

cflags += -I${sonLibDir} -fPIC
cppflags += -I${sonLibDir} -fPIC ${CXX_11_ABI_DEF} -std=c++11 -Wno-sign-compare

basicLibs += ${sonLibDir}/sonLib.a ${sonLibDir}/cuTest.a
basicLibsDependencies += ${sonLibDir}/sonLib.a ${sonLibDir}/cuTest.a

# hdf5 compilation is done through its wrappers.
# we can speficy our own (sonlib) compilers with these variables:
cpp = h5c++ ${h5prefix}
cxx = h5cc ${h5prefix}

#
# phyloP support
#
phyloPcppflags = 
phyloPlibs = 

ifdef ENABLE_PHYLOP

ifndef TARGETOS
  TARGETOS := $(shell uname -s)
endif

#  Defaults to local Linux install (phast and clapack sister dirs to hal/)
# (note CLAPACKPATH not needed in Mac)
ifeq (${PHAST},)
    PHAST=../../phast
endif
ifeq (${CLAPACKPATH},)
    CLAPACKPATH=../../clapack
endif

ifeq ($(TARGETOS), Darwin)
    cppflags += -DENABLE_PHYLOP -I${PHAST}/include -I${PHAST}/src/lib/pcre -DVECLIB
    basicLibs += -L${PHAST}/lib -lphast -lc -framework Accelerate
else
    F2CPATH=${CLAPACKPATH}/F2CLIBS
    cppflags += -DENABLE_PHYLOP -I${PHAST}/include -I${PHAST}/src/lib/pcre -I${CLAPACKPATH}/INCLUDE -I${F2CPATH}
    basicLibs += -L${PHAST}/lib -lphast -L${CLAPACKPATH} -L${F2CPATH} -llapack -ltmg -lblaswr -lf2c 
endif

endif

# add compiler flag and kent paths if udc is enabled
# relies on KENTSRC containing path to top level kent/ dir
# and MACHTYPE being specified.
# This MUST follow PHAST defs, as they both have a gff.h
ifdef ENABLE_UDC
ifdef ENABLE_PHYLOP
     $(error can not compile with both ENABLE_UDC and ENABLE_PHYLOP due to link function name conflict)
endif
    #  Find htslib as in kent/src/inc/common.mk:
    MACHTYPE = x86_64
    cppflags += -DENABLE_UDC -I${KENTSRC}/inc -I${KENTSRC}/htslib -pthread
    # FIXME: standarize var names
    cflags += -Wall -Werror -std=c99 -I${KENTSRC}/inc -I${KENTSRC}/htslib -pthread
    basicLibs += ${KENTSRC}/lib/${MACHTYPE}/jkweb.a  ${KENTSRC}/htslib/libhts.a -lcurl -lssl -lcrypto
endif



# test includes and libs uses buy several modules
halApiTestIncl = ${rootDir}/api/tests
halApiTestSupportLibs = ${objDir}/api/tests/halApiTestSupport.o ${objDir}/api/tests/halRandomData.o
