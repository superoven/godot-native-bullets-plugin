.PHONY: build-all
build-all: build-mac build-windows build-linux

.PHONY: build-mac
build-mac:
	scons platform=osx target=release -Q

.PHONY: build-windows
build-windows:
	scons platform=windows target=release -Q

.PHONY: build-linux
build-linux:
	scons platform=x11 target=release -Q
