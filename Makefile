rootDir = .
include include.mk

modules = api stats randgen validate mutations fasta alignmentDepth liftover lod maf blockViz extract analysis phyloP modify assemblyHub synteny


.PHONY: all libs %.libs progs %.progs clean %.clean doxy %.doxy

all : libs progs

libs: ${modules:%=%.libs}
%.libs:
	cd $* && ${MAKE} libs

progs: ${modules:%=%.progs}
%.progs: libs
	cd $* && ${MAKE} progs

clean: ${modules:%=%.clean}
	rm -rf lib bin objs
	rm -f *.pyc */*.pyc */*/*.pyc
	rm -rf __pycache__ */__pycache__ */*/__pycache__

%.clean:
	cd $* && ${MAKE} clean

test: ${modules:%=%.test}

%.test: all
	cd $* && ${MAKE} test


doxy : ${modules:%=doxy.%}

doxy.%:
	cd api && ${MAKE} doxy

etags:
	etags $$(find . -name '*.h' -o -name '*.cpp')
