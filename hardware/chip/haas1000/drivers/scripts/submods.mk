cur_makefile := $(lastword $(MAKEFILE_LIST))

$(cur_makefile): ;

get_subdirs = $(foreach m, $(1), $(if $(filter $m/% $m,$(2)),$m,))

get_submodgoals = $(foreach m, \
  $(patsubst $(1)/%,%,$(filter $(1)/%,$(2))), \
  $(addprefix $(1)/,$(firstword $(subst /,$(space),$m))))

get_multi_objs = $(addprefix $(obj)/, $($(subst $(obj)/,,$(1:.o=-objs))) \
  $($(subst $(obj)/,,$(1:.o=-objs))))

