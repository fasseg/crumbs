bin_PROGRAMS=crumbs
crumbs_SOURCES=crumbs.c
man1_MANS=doc/crumbs.man
sysconf_DATA=crumbs.conf
AM_CPPFLAGS = -DSYSCONFDIR="\"$(sysconfdir)\""
README: README.md

if OS_LINUX
data_sharedir=$(prefix)/share/crumbs
data_share_DATA=crumbs-completion.bash
endif
