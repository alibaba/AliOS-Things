# arg mk_path: path to mk file
include $(mk_path)

ifeq ($(mk_path),)
$(error argument 'mk_path' is not set！)
endif 
	
exist = $(shell if [ -f $(mk_path) ]; then echo "exist"; else echo "notexist"; fi;)
ifeq ($(exist),notexist)
$(error $(mk_path) is not exist)
endif	
	
py_path :=  $(dir $(mk_path) )/ucube.py   

ECHO = echo
QUIET =@
#WRITE_FILE_CREATE =$(ECHO) '$(2)' > $(1);
#WRITE_FILE_APPEND =$(ECHO) '$(2)' >> $(1);
WRITE_FILE_CREATE =$(file >$(1),$(2))
WRITE_FILE_APPEND =$(file >>$(1),$(2))

empty =
tab_space =$(empty)    
colon =:
split_start =Split(''' 
split_end = ''')
component_construct = aos_component('$(NAME)', src)

COMPONENT_DIRECTORIES := . \
						 example   \
						 board	 \
						 kernel	\
						 platform  \
						 utility   \
						 framework \
						 tools	 \
						 test	  \
						 device	\
						 security
						 
#$1 is the component dir path and mk file name
FIND_COMP_DIR =$(wildcard $(foreach dir,  $(COMPONENT_DIRECTORIES), $(dir)/$(subst .,/,$1)))

all:
#NAME := board_b_l433
#$(NAME)_SOURCES += board.c
	$(call WRITE_FILE_CREATE, $(py_path) ,src =$(split_start))
	$(foreach src, $($(NAME)_SOURCES), $(call WRITE_FILE_APPEND, $(py_path),$(tab_space)$(src)) )
	$(call WRITE_FILE_APPEND, $(py_path),$(split_end))
	
	$(call WRITE_FILE_APPEND, $(py_path),component =$(component_construct))
	
#$(NAME)_COMPONENTS += sal
	$(call WRITE_FILE_APPEND, $(py_path),)
ifneq ($($(NAME)_COMPONENTS),)	
	$(call WRITE_FILE_APPEND, $(py_path) ,dependencis =$(split_start))
	$(foreach dep, $($(NAME)_COMPONENTS), $(call WRITE_FILE_APPEND, $(py_path),$(tab_space)$(call FIND_COMP_DIR,$(dep))))
	$(call WRITE_FILE_APPEND, $(py_path),$(split_end))
	$(call WRITE_FILE_APPEND, $(py_path),for i in dependencis$(colon))
	$(call WRITE_FILE_APPEND, $(py_path),$(tab_space)component.add_comp_deps(i))
endif
	
#GLOBAL_INCLUDES += .
	$(call WRITE_FILE_APPEND, $(py_path),)
ifneq ($(GLOBAL_INCLUDES),)	
	$(call WRITE_FILE_APPEND, $(py_path) ,global_includes =$(split_start))
	$(foreach inc, $(GLOBAL_INCLUDES), $(call WRITE_FILE_APPEND, $(py_path),$(tab_space)$(inc)) )
	$(call WRITE_FILE_APPEND, $(py_path),$(split_end))
	$(call WRITE_FILE_APPEND, $(py_path),for i in global_includes$(colon))
	$(call WRITE_FILE_APPEND, $(py_path),$(tab_space)component.add_global_includes(i))
endif
	
#GLOBAL_DEFINES += STDIO_UART=0
	$(call WRITE_FILE_APPEND, $(py_path),)
ifneq ($(GLOBAL_DEFINES),)	
	$(call WRITE_FILE_APPEND, $(py_path) ,global_macros =$(split_start))
	$(foreach macro, $(GLOBAL_DEFINES), $(call WRITE_FILE_APPEND, $(py_path),$(tab_space)$(macro)) )
	$(call WRITE_FILE_APPEND, $(py_path),$(split_end))
	$(call WRITE_FILE_APPEND, $(py_path),for i in global_macros$(colon))
	$(call WRITE_FILE_APPEND, $(py_path),$(tab_space)component.add_global_macros(i))
endif
	
#$(NAME)_INCLUDES
	$(call WRITE_FILE_APPEND, $(py_path),)
ifneq ($($(NAME)_INCLUDES),)
	$(call WRITE_FILE_APPEND, $(py_path) ,includes =$(split_start))
	$(foreach inc, $($(NAME)_INCLUDES), $(call WRITE_FILE_APPEND, $(py_path),$(tab_space)$(inc)) )
	$(call WRITE_FILE_APPEND, $(py_path),$(split_end))
	$(call WRITE_FILE_APPEND, $(py_path),for i in includes$(colon))
	$(call WRITE_FILE_APPEND, $(py_path),$(tab_space)component.add_includes(i))
endif
	
#$(NAME)_DEFINES
	$(call WRITE_FILE_APPEND, $(py_path),)
ifneq ($($(NAME)_DEFINES),)	
	$(call WRITE_FILE_APPEND, $(py_path) ,macros =$(split_start))
	$(foreach macro, $($(NAME)_DEFINES), $(call WRITE_FILE_APPEND, $(py_path),$(tab_space)$(macro)) )
	$(call WRITE_FILE_APPEND, $(py_path),$(split_end))
	$(call WRITE_FILE_APPEND, $(py_path),for i in macros$(colon))
	$(call WRITE_FILE_APPEND, $(py_path),$(tab_space)component.add_macros(i))
endif	
	
#$(NAME)_CFLAGS
	$(call WRITE_FILE_APPEND, $(py_path),)
ifneq ($($(NAME)_CFLAGS),)	
	$(call WRITE_FILE_APPEND, $(py_path) ,cflags =$(split_start))
	$(foreach cflag, $($(NAME)_CFLAGS), $(call WRITE_FILE_APPEND, $(py_path),$(tab_space)$(cflag)) )
	$(call WRITE_FILE_APPEND, $(py_path),$(split_end))
	$(call WRITE_FILE_APPEND, $(py_path),for i in cflags$(colon))
	$(call WRITE_FILE_APPEND, $(py_path),$(tab_space)component.add_cflags(i))
endif	
	
	
	
	
	
	