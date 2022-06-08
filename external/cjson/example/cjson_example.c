/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
 */

#if AOS_COMP_CLI
#include <stdio.h>
#include <cJSON.h>
#include <aos/cli.h>

static const char *json_string =
    "{\n"
    "\"name\": \"Awesome 4K\",\n"
    "\"resolutions\":\n"
    "[\n"
    "{\n"
    "\"width\": 1280,\n"
    "\"height\": 720\n"
    "},\n"
    "{\n"
    "\"width\": 1920,\n"
    "\"height\": 1280\n"
    "},\n"
    "{\n"
    "\"width\": 3840,\n"
    "\"height\": 2160\n"
    "}\n"
    "]\n"
    "}\n";

static void cjson_example(int argc, char **argv)
{
    cJSON *root = NULL;
    const cJSON *resolution = NULL;
    const cJSON *resolutions = NULL;
    const cJSON *name = NULL;

    root = cJSON_Parse(json_string);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL)
            fprintf(stderr, "Error before: %s\n", error_ptr);

        cJSON_Delete(root);
        return;
    }

    name = cJSON_GetObjectItemCaseSensitive(root, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
        printf("Checking monitor \"%s\"\n", name->valuestring);

    resolutions = cJSON_GetObjectItemCaseSensitive(root, "resolutions");
    cJSON_ArrayForEach(resolution, resolutions) {
        cJSON *width = cJSON_GetObjectItemCaseSensitive(resolution, "width");
        cJSON *height = cJSON_GetObjectItemCaseSensitive(resolution, "height");

        if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height)) {
            cJSON_Delete(root);
            return;
        }

        printf("%d x %.2f\n", width->valueint, height->valuedouble);
    }

    cJSON_Delete(root);
}

ALIOS_CLI_CMD_REGISTER(cjson_example, cjson_example, cJSON example)
#endif /* AOS_COMP_CLI */
