/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef UCLI_H
#define UCLI_H

int cli_getchar(char *ch);

int cli_init(void);

int cli_deinit(void);

void cli_loop(void);

#endif /* UCLI_H */
