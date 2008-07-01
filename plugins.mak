# Makefile - plugins
EFFECTS_DIR=`ls plugins/Effects/* -d`
EFECTOS_MAESTROS_DIR=`ls plugins/MasterEffects/* -d`
PRE_EFFECTS_DIR=`ls plugins/PreEffects/* -d`
POST_EFFECTS_DIR=`ls plugins/PostEffects/* -d`

compile:
	for dir in $(EFFECTS_DIR); \
	do \
	    make -C $$dir; \
	done

	for dir in $(EFECTOS_MAESTROS_DIR); \
	do \
	    make -C $$dir; \
	done

	for dir in $(PRE_EFFECTS_DIR); \
	do \
	    make -C $$dir; \
	done

	for dir in $(POST_EFFECTS_DIR); \
	do \
	    make -C $$dir; \
	done

clean:
	for dir in $(EFFECTS_DIR); \
	do \
	    make -C $$dir clean; \
	done

	for dir in $(EFECTOS_MAESTROS_DIR); \
	do \
	    make -C $$dir clean; \
	done

	for dir in $(PRE_EFFECTS_DIR); \
	do \
	    make -C $$dir clean; \
	done

	for dir in $(POST_EFFECTS_DIR); \
	do \
	    make -C $$dir clean; \
	done
