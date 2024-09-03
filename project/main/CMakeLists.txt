idf_component_register(SRC_DIRS
                    "."
                    "ui/fonts"
                    "ui/imgs"
                    "ui/imgs/image_setting"
                    "ui/imgs/image_standby"
                    "ui"
                    "ui/layer_manage"
                    INCLUDE_DIRS
                    "."
                    "ui/layer_manage")

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-cast-function-type)
