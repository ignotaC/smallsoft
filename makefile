MAKE=make -C object

.PHONY: make clear clean clearobj cleanobj

make clear clean clearobj cleanobj:
	${MAKE} setup
	${MAKE} $@
        
