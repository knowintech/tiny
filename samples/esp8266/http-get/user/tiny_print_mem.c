/**
 * Copyright (C) 2013-2015
 *
 * @author jxfengzi@gmail.com
 * @date   2013-11-19
 *
 * @file   tiny_print_mem.c
 *
 * @remark
 *      set tabstop=4
 *      set shiftwidth=4
 *      set expandtab
 */

#include <tiny_print_mem.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

void tiny_print_mem(const char *tag, const char *function)
{
    printf("[%s/%s] stack = %d, free heap size: %d\n", tag, function, uxTaskGetStackHighWaterMark(NULL), system_get_free_heap_size());
}