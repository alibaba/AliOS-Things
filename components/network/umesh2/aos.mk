NAME := umesh2

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION := 1.0.0
$(NAME)_SUMMARY := mesh2 implement.

$(NAME)_COMPONENTS-$(UMESH_WITH_DEFAULT_CORE) += umesh2_core
$(NAME)_COMPONENTS-$(UMESH_WITH_LOCAL_COMM) += local_comm