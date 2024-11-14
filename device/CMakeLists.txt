idf_component_register(SRCS "smab.c" "usergroups.c"
                    PRIV_REQUIRES spiffs nvs_flash esp_wifi esp_http_client
                    INCLUDE_DIRS ".")
spiffs_create_partition_image(storage ../partition FLASH_IN_PROJECT)
