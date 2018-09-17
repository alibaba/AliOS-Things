SRC = ../src

SOURCE_FILES = $(SRC)/spiffs_cache.c \
	$(SRC)/spiffs_check.c \
	$(SRC)/spiffs_gc.c \
	$(SRC)/spiffs_hydrogen.c \
	$(SRC)/spiffs_nucleus.c \
	python_ops.c

INCLUDES = -I . \
	-I $(SRC)/ \
	-I $(SRC)/default

spiffs_.so: $(SOURCE_FILES)
	$(CC) -g3 -fPIC -shared $(INCLUDES) -o $@ $(SOURCE_FILES) -lm

clean:
	rm -rf spiffs_.so *~ *.pyc
