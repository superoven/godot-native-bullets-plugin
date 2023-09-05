GIT_REPO := $(HOME)/git/godot/touhou_demake
ADDONS_DIR := addons/native_bullets
NATIVE_BULLETS_REMOTE := $(GIT_REPO)/$(ADDONS_DIR)
EXTENSIONS := gd gdns cfg gdnlib gdns

.PHONY: build
build: build-addons

build-addons:
	$(MAKE) -C $(ADDONS_DIR) build-mac

.PHONY: export
export: build export-gd export-kits
	cp $(ADDONS_DIR)/bin/macos/libbullets.dylib $(NATIVE_BULLETS_REMOTE)/bin/macos

.PHONY: export-gd
export-gd: $(foreach EXT,$(EXTENSIONS),$(wildcard $(ADDONS_DIR)/*.$(EXT)))
	cp $^ $(NATIVE_BULLETS_REMOTE)/

.PHONY: export-kits
export-kits: $(wildcard $(ADDONS_DIR)/kits/*.gdns)
	cp $^ $(NATIVE_BULLETS_REMOTE)/kits/
