MAKE=make -C obj

.PHONY: make clear clean clearobj cleanobj

make clear clean cealrobj cleanobj:
	${MAKE} setup
	${MAKE} $@
        
