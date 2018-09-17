//
//  module_js_lite.h
//  bone
//
//  Created by howie on 04/12/2017.
//  Copyright © 2017 alibaba. All rights reserved.
//

#ifndef module_js_lite_h
#define module_js_lite_h

void module_js_lite_load(void);

void module_process_load(void);

be_jse_symbol_t *timers_module_handle_cb(be_jse_vm_ctx_t *execInfo, be_jse_symbol_t *var, const char *name);

#endif /* module_js_lite_h */
