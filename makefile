MAKE=make -C object

.PHONY: make clear clean clearobj cleanobj

make:
	${MAKE} setup
	${MAKE} $@
	${MAKE} $@
	@printf "\nSmall soft compiled succesfully.\n"
	@printf "Binaries are in bin dir\n\n"


clear clean clearobj cleanobj:
	${MAKE} setup
	${MAKE} $@


