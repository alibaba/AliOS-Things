//
//  be_jse_module.h
//  bone
//
//  Created by howie on 28/11/2017.
//  Copyright © 2017 alibaba. All rights reserved.
//

#ifndef be_jse_module_h
#define be_jse_module_h

#include "be_jse.h"
#include "be_jse_api.h"

void be_jse_module_load(const char* moduleName, BE_JSE_FUNCTION_EXECUTE_CB extension);

#endif /* be_jse_module_h */
