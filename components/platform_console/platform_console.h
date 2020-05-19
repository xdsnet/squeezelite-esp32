/* Console example — declarations of command registration functions.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "cJSON.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef cJSON * parm_values_fn_t(void);
esp_err_t cmd_to_json(const esp_console_cmd_t *cmd);
esp_err_t cmd_to_json_with_cb(const esp_console_cmd_t *cmd, parm_values_fn_t parm_values_fn);
int arg_parse_msg(int argc, char **argv, struct arg_hdr ** args);
cJSON * get_cmd_list();
#ifdef __cplusplus
}
#endif