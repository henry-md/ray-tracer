DEPENDENT_DIRS = Image Util Ray
DEPENDENT_MAKEFILES = MakefileImageProcessor MakefileRayTracer MakefileSceneViewer MakefileAnimatedSceneViewer

all:
	for dir in $(DEPENDENT_DIRS); do make -C $$dir; done
	for makefile in $(DEPENDENT_MAKEFILES); do make -f $$makefile; done

debug:
	for dir in $(DEPENDENT_DIRS); do make debug -C $$dir; done
	for makefile in $(DEPENDENT_MAKEFILES); do make -f $$makefile; done

clean:
	for dir in $(DEPENDENT_DIRS); do make clean -C $$dir; done
	for makefile in $(DEPENDENT_MAKEFILES); do make clean -f $$makefile; done
