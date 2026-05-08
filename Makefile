BUILDDIR := ./build

.PHONY: default
default: byauto

.PHONY: help
help:
	@echo "basic targets:"
	@echo "  default                    The default target.  Equivalent to 'byauto'"
	@echo "  byauto                     Compile entire project by CC"
	@echo "  bygcc                      Compile entire project by GCC"
	@echo "  byclang                    Compile entire project by Clang"
	@echo ""
	@echo "advanced targets:"
	@echo "  cmake-prepare-<tool-id>    Prepare CMake build directory for <tool-id> builder"
	@echo "  test-<tool-id>             Run tests built by <tool-id>"
	@echo "  <tool-id>-help             List available target provided by <tool-id>"
	@echo "  <tool-id>-<cmake-target>   Run cmake command with target <cmake-target> made by <tool-id>"
	@echo "  cmake-refresh-<tool-id>    Re-generate CMake build directory for <tool-id>"
	@echo "  build-dir-<tool-id>        Print build directory for <tool-id>"
	@echo ""
	@echo "<tool-id>:"
	@echo "  byauto                     Use system C compiler"
	@echo "  bygcc                      Use GCC"
	@echo "  byclang                    Use Clang"
	@echo ""
	@echo "<cmake-target>:"
	@echo "  This is what can be specified after 'cmake --target'."
	@echo "  Can be listed by 'make <tool-id>-help'."

define MKDIR
	@test -d $1 || mkdir -p $1
endef

define GEN_BUILDMENUS
.PHONY: cmake-prepare-$1 $1 test-$1 $1-% cmake-refresh-$1 build-dir-$1
cmake-prepare-$1:
	$(call MKDIR,$(BUILDDIR)/$1)
	CC=$2 cmake -B $(BUILDDIR)/$1 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
$1: cmake-prepare-$1
	cmake --build $(BUILDDIR)/$1
test-$1: $1
	ctest --verbose --output-on-failure --test-dir $(BUILDDIR)/$1
$1-%: cmake-prepare-$1
	cmake --build $(BUILDDIR)/$1 --target $${@:$1-%=%}
cmake-refresh-$1:
	$(call MKDIR,$(BUILDDIR)/$1)
	CC=$2 cmake -B $(BUILDDIR)/$1 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON --fresh
build-dir-$1:
	@echo $(BUILDDIR)/$1
endef

$(eval $(call GEN_BUILDMENUS,byauto,))
$(eval $(call GEN_BUILDMENUS,bygcc,gcc))
$(eval $(call GEN_BUILDMENUS,byclang,clang))

# .PHONY: clean
# clean:
# 	$(RM) -r $(BUILDDIR)
