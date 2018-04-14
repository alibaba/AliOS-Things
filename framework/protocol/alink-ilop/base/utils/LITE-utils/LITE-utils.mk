#NAME := LITE-utils

LITE_SOURCE_DIR = base/utils/LITE-utils/src

$(NAME)_SOURCES  += $(LITE_SOURCE_DIR)/json_parser.c
$(NAME)_SOURCES  += $(LITE_SOURCE_DIR)/json_token.c
$(NAME)_SOURCES  += $(LITE_SOURCE_DIR)/mem_stats.c
$(NAME)_SOURCES  += $(LITE_SOURCE_DIR)/string_utils.c


GLOBAL_INCLUDES    += \
                     $(LITE_SOURCE_DIR)/
                   
                    
