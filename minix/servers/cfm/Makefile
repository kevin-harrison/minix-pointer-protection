# Makefile for the input server

PROG=	cfm
SRCS=	main.c store.c

DPADD+=	${LIBSYS}
LDADD+=	-lsys

CPPFLAGS.store.c+= -Dregcomp=_regcomp -Dregfree=_regfree

.include <minix.service.mk>

