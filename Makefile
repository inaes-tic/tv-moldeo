# Makefile - moldeo

.PHONY: all
all: \
	moldeo

.PHONY: lib
lib:
	$(MAKE) -f libmoldeo.mak

.PHONY: plugins
plugins:
	$(MAKE) -f plugins.mak

.PHONY: main
main:
	$(MAKE) -f moldeo.mak

.PHONY: moldeo
moldeo:
	$(MAKE) -f libmoldeo.mak
	$(MAKE) -f moldeo.mak
	$(MAKE) -f plugins.mak

.PHONY: clean
clean:
	$(MAKE) -f libmoldeo.mak clean
	$(MAKE) -f moldeo.mak clean
	$(MAKE) -f plugins.mak clean

.PHONY: install
install:
	$(MAKE) -f libmoldeo.mak install

.PHONY: uninstall
uninstall:
	$(MAKE) -f libmoldeo.mak uninstall


.PHONY: depends
depends:
	$(MAKE) -f moldeo.mak depends

