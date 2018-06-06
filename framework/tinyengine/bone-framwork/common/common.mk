.PHONY: $(dirs) clean  

#SHELL=/bin/bash

exclude_dirs:= inc

dirs=$(shell find . -maxdepth 1 -type d)   
dirs:=$(basename $(patsubst ./%,%,$(dirs)))  
dirs:=$(filter-out $(exclude_dirs),$(dirs))  


all:
	echo "compile $(dirs)..."
	$(foreach N,$(dirs),make -C $(N) || exit "$$?";)  

install:
	$(foreach N,$(dirs),make -C $(N) install;) 

clean:  
	$(foreach N,$(dirs),make -C $(N) clean;)  
