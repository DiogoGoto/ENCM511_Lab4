# The following variables contains the files used by the different stages of the build process.
set(Lab4_default_default_XC16_FILE_TYPE_assemble)
set_source_files_properties(${Lab4_default_default_XC16_FILE_TYPE_assemble} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${Lab4_default_default_XC16_FILE_TYPE_assemble})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(Lab4_default_default_XC16_FILE_TYPE_assemblePreproc)
set_source_files_properties(${Lab4_default_default_XC16_FILE_TYPE_assemblePreproc} PROPERTIES LANGUAGE ASM)

# For assembly files, add "." to the include path for each file so that .include with a relative path works
foreach(source_file ${Lab4_default_default_XC16_FILE_TYPE_assemblePreproc})
        set_source_files_properties(${source_file} PROPERTIES INCLUDE_DIRECTORIES "$<PATH:NORMAL_PATH,$<PATH:REMOVE_FILENAME,${source_file}>>")
endforeach()

set(Lab4_default_default_XC16_FILE_TYPE_compile
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../2026_assignment4_main.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../uart.c")
set_source_files_properties(${Lab4_default_default_XC16_FILE_TYPE_compile} PROPERTIES LANGUAGE C)
set(Lab4_default_default_XC16_FILE_TYPE_link)
set(Lab4_default_default_XC16_FILE_TYPE_bin2hex)
set(Lab4_default_default_XC16_FILE_TYPE_objcopy_lss)
set(Lab4_default_image_name "default-production.elf")
set(Lab4_default_image_base_name "default-production")

# The output directory of the final image.
set(Lab4_default_output_dir "${CMAKE_CURRENT_SOURCE_DIR}/../../../out/Lab4/production")

# The full path to the final image.
set(Lab4_default_full_path_to_image ${Lab4_default_output_dir}/${Lab4_default_image_name})
