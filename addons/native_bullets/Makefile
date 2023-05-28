.PHONY: build-all
build-all: build-mac build-windows build-linux

.PHONY: build-mac
build-mac:
	scons platform=osx target=release

.PHONY: build-windows
build-windows:
	scons platform=windows target=release

.PHONY: build-linux
build-linux:
	scons platform=x11 target=release
