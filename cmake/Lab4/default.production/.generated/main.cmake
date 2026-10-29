include("${CMAKE_CURRENT_LIST_DIR}/rule.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/file.cmake")

set(Lab4_default_library_list )

# Handle files with suffix s, for group default-XC16
if(Lab4_default_default_XC16_FILE_TYPE_assemble)
add_library(Lab4_default_default_XC16_assemble OBJECT ${Lab4_default_default_XC16_FILE_TYPE_assemble})
    Lab4_default_default_XC16_assemble_rule(Lab4_default_default_XC16_assemble)
    list(APPEND Lab4_default_library_list "$<TARGET_OBJECTS:Lab4_default_default_XC16_assemble>")

endif()

# Handle files with suffix S, for group default-XC16
if(Lab4_default_default_XC16_FILE_TYPE_assemblePreproc)
add_library(Lab4_default_default_XC16_assemblePreproc OBJECT ${Lab4_default_default_XC16_FILE_TYPE_assemblePreproc})
    Lab4_default_default_XC16_assemblePreproc_rule(Lab4_default_default_XC16_assemblePreproc)
    list(APPEND Lab4_default_library_list "$<TARGET_OBJECTS:Lab4_default_default_XC16_assemblePreproc>")

endif()

# Handle files with suffix c, for group default-XC16
if(Lab4_default_default_XC16_FILE_TYPE_compile)
add_library(Lab4_default_default_XC16_compile OBJECT ${Lab4_default_default_XC16_FILE_TYPE_compile})
    Lab4_default_default_XC16_compile_rule(Lab4_default_default_XC16_compile)
    list(APPEND Lab4_default_library_list "$<TARGET_OBJECTS:Lab4_default_default_XC16_compile>")

endif()

# Handle files with suffix s, for group default-XC16
if(Lab4_default_default_XC16_FILE_TYPE_dependentObject)
add_library(Lab4_default_default_XC16_dependentObject OBJECT ${Lab4_default_default_XC16_FILE_TYPE_dependentObject})
    Lab4_default_default_XC16_dependentObject_rule(Lab4_default_default_XC16_dependentObject)
    list(APPEND Lab4_default_library_list "$<TARGET_OBJECTS:Lab4_default_default_XC16_dependentObject>")

endif()

# Handle files with suffix elf, for group default-XC16
if(Lab4_default_default_XC16_FILE_TYPE_bin2hex)
add_library(Lab4_default_default_XC16_bin2hex OBJECT ${Lab4_default_default_XC16_FILE_TYPE_bin2hex})
    Lab4_default_default_XC16_bin2hex_rule(Lab4_default_default_XC16_bin2hex)
    list(APPEND Lab4_default_library_list "$<TARGET_OBJECTS:Lab4_default_default_XC16_bin2hex>")

endif()

# Handle files with suffix elf, for group default-XC16
if(Lab4_default_default_XC16_FILE_TYPE_objcopy_lss)
add_library(Lab4_default_default_XC16_objcopy_lss OBJECT ${Lab4_default_default_XC16_FILE_TYPE_objcopy_lss})
    Lab4_default_default_XC16_objcopy_lss_rule(Lab4_default_default_XC16_objcopy_lss)
    list(APPEND Lab4_default_library_list "$<TARGET_OBJECTS:Lab4_default_default_XC16_objcopy_lss>")

endif()


# Main target for this project
add_executable(Lab4_default_image_zOBM8n8W ${Lab4_default_library_list})

set_target_properties(Lab4_default_image_zOBM8n8W PROPERTIES
    OUTPUT_NAME "default-production"
    SUFFIX ".elf"
    RUNTIME_OUTPUT_DIRECTORY "${Lab4_default_output_dir}")
target_link_libraries(Lab4_default_image_zOBM8n8W PRIVATE ${Lab4_default_default_XC16_FILE_TYPE_link})
# Add the link options from the rule file.
Lab4_default_link_rule( Lab4_default_image_zOBM8n8W)

# Call bin2hex function from the rule file
Lab4_default_bin2hex_rule(Lab4_default_image_zOBM8n8W)

#Add objcopy steps
Lab4_default_objcopy_lss_rule(Lab4_default_image_zOBM8n8W)

