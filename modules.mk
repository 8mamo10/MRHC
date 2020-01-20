mod_mrhc.la: mod_mrhc.slo
	$(SH_LINK) -rpath $(libexecdir) -module -avoid-version  mod_mrhc.lo
DISTCLEAN_TARGETS = modules.mk
shared =  mod_mrhc.la
