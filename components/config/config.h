#pragma once
#include <stdio.h>
#include <string.h>
#include "nvs.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif
#define DECLARE_SET_DEFAULT(t) void config_set_default_## t (const char *key, t  value);
#define DECLARE_GET_NUM(t) esp_err_t config_get_## t (const char *key, t *  value);


DECLARE_SET_DEFAULT(uint8_t);
DECLARE_SET_DEFAULT(uint16_t);
DECLARE_SET_DEFAULT(uint32_t);
DECLARE_SET_DEFAULT(int8_t);
DECLARE_SET_DEFAULT(int16_t);
DECLARE_SET_DEFAULT(int32_t);
DECLARE_GET_NUM(uint8_t);
DECLARE_GET_NUM(uint16_t);
DECLARE_GET_NUM(uint32_t);
DECLARE_GET_NUM(int8_t);
DECLARE_GET_NUM(int16_t);
DECLARE_GET_NUM(int32_t);

bool config_has_changes();
void config_commit_to_nvs();
void config_start_timer();
void config_init();
void * config_alloc_get_default(nvs_type_t type, const char *key, void * default_value, size_t blob_size);
void config_delete_key(const char *key);
void config_set_default(nvs_type_t type, const char *key, void * default_value, size_t blob_size);
void * config_alloc_get(nvs_type_t nvs_type, const char *key) ;
void * config_alloc_get_str(const char *key, char *lead, char *fallback);
bool wait_for_commit();
char * config_alloc_get_json(bool bFormatted);
esp_err_t config_set_value(nvs_type_t nvs_type, const char *key, void * value);

