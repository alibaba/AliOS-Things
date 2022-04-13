cur_makefile := $(lastword $(MAKEFILE_LIST))

$(cur_makefile): ;

ifeq ($(WIN_PLAT),y)
SUBMODGOALS := $(sort $(foreach m, $(MAKECMDGOALS), \
  $(if $(filter-out ./,$(wildcard $(dir $(subst \,/,$m)) $(subst \,/,$m))),$m,)))
else
  SUBMODGOALS := $(sort $(foreach m, $(MAKECMDGOALS), \
  $(if $(filter-out ./,$(wildcard $(dir $m) $m)),$m,)))
endif

ifneq ($(SUBMODGOALS),)
MAKECMDGOALS := $(filter-out $(SUBMODGOALS),$(MAKECMDGOALS))
ifeq ($(WIN_PLAT),y)
SUBMODGOALS := $(subst \,/,$(SUBMODGOALS))
endif
SUBMODS := $(patsubst $(CURDIR)/%,%,$(SUBMODGOALS))
SUBMODS := $(patsubst %/,%,$(SUBMODS))
# Filter out subdirectories if their parent directories already in SUBMODS
SUBMODS := $(filter-out $(addsuffix /%,$(SUBMODS)),$(SUBMODS))
export SUBMODS
endif

