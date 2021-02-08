NAME := jsoncpp

$(NAME)_MBINS_TYPE := app
$(NAME)_VERSION := 1.0.2
$(NAME)_SUMMARY := JsonCpp is a C++ library that allows manipulating JSON values under MIT license.

GLOBAL_INCLUDES += include
$(NAME)_INCLUDES += src/lib_json/

$(NAME)_SOURCES := src/lib_json/json_reader.cpp \
					src/lib_json/json_value.cpp \
					src/lib_json/json_writer.cpp \
					src/lib_json/json_valueiterator.inl
