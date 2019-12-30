/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef JSE_ENGINE_H
#define JSE_ENGINE_H

/**
 *
 * JSEngine initialization
 *
 */
void jsengine_init(void);

/**
 * Running JS code
 *
 */
void jsengine_start(const char* js);

/**
 * Running JS file
 */
void jsengine_eval_file(const char* pathname);

/**
 * JSEngine exit
 * free memory(including symbol table)
 */
void jsengine_exit(void);

#endif /* JSE_ENGINE_H */