
list_projects:
	$(Q)for i in $(wildcard projects/*) ; do \
		if [ -f "$$i/mk/project.mk" ]; then \
			echo $$i; \
			if [ -d "$$i/examples" ]; then \
				echo $$i/examples/*; \
			fi \
		fi \
	done

setup:
	$(Q)if [ -z "$(p)" ]; then \
		rm -f build/project_cfg.mk; \
		echo "Please input p args like \"make setup p=base_proj_freertos\""; \
		$(MAKE) list_projects; \
	else \
		if [ -f "projects/$(p)/mk/project.mk" ]; then \
			if [ -z "$(sp)" ]; then \
				rm -f build/project_cfg.mk; \
				echo build project $(p); \
				echo PROJECT := $(p) >> build/project_cfg.mk; \
				echo PROJ_DIR := projects/$(p) >> build/project_cfg.mk; \
				echo sinclude $(TOPDIR)/projects/$(p)/mk/project.mk >> build/project_cfg.mk; \
				echo export PROJ_DIR PROJECT >> build/project_cfg.mk; \
			else \
				if [ -f "projects/$(p)/examples/$(sp)/mk/project.mk" ]; then \
					rm -f build/project_cfg.mk; \
					echo build subproject $(sp); \
					echo PROJECT := $(sp) >> build/project_cfg.mk; \
					echo PROJ_DIR := projects/$(p)/examples/$(sp) >> build/project_cfg.mk; \
					echo sinclude $(TOPDIR)/projects/$(p)/examples/$(sp)/mk/project.mk >> build/project_cfg.mk; \
					echo INC += -Iprojects/$(p)/src/inc >> build/project_cfg.mk; \
					echo export PROJ_DIR PROJECT >> build/project_cfg.mk; \
				else \
					echo "Please setup a exist subproject, if you don't sure, please use \"make list_projects\""; \
					$(MAKE) list_projects; \
				fi \
			fi \
		else \
			echo "Please setup a exist project, if you don't sure, please use \"make list_projects\""; \
			$(MAKE) list_projects; \
		fi \
	fi
	$(Q)if [ -n "$(version)" ]; then\
		echo LIB_VERSION := $(version) >> build/project_cfg.mk; \
	fi
	$(Q)if [ -n "$(customer)" ]; then\
		echo CUSTOMER := $(customer) >> build/project_cfg.mk; \
	fi
