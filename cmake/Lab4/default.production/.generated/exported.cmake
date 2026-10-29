set(DEPENDENT_MP_BIN2HEXLab4_default_zOBM8n8W "c:/Program Files/Microchip/xc16/v2.10/bin/xc16-bin2hex.exe")
set(DEPENDENT_DEPENDENT_TARGET_ELFLab4_default_zOBM8n8W ${CMAKE_CURRENT_LIST_DIR}/../../../../out/Lab4/production/default-production.elf)
set(DEPENDENT_TARGET_DIRLab4_default_zOBM8n8W ${CMAKE_CURRENT_LIST_DIR}/../../../../out/Lab4/production)
set(DEPENDENT_BYPRODUCTSLab4_default_zOBM8n8W ${DEPENDENT_TARGET_DIRLab4_default_zOBM8n8W}/${sourceFileNameLab4_default_zOBM8n8W}.s)
add_custom_command(
    OUTPUT ${DEPENDENT_TARGET_DIRLab4_default_zOBM8n8W}/${sourceFileNameLab4_default_zOBM8n8W}.s
    COMMAND ${DEPENDENT_MP_BIN2HEXLab4_default_zOBM8n8W} ${DEPENDENT_DEPENDENT_TARGET_ELFLab4_default_zOBM8n8W} --image ${sourceFileNameLab4_default_zOBM8n8W} ${addressLab4_default_zOBM8n8W} ${modeLab4_default_zOBM8n8W} -mdfp=C:/Users/diogo/.mchp_packs/Microchip/PIC24F-GA-GB_DFP/1.11.479/xc16 
    WORKING_DIRECTORY ${DEPENDENT_TARGET_DIRLab4_default_zOBM8n8W}
    DEPENDS ${DEPENDENT_DEPENDENT_TARGET_ELFLab4_default_zOBM8n8W})
add_custom_target(
    dependent_produced_source_artifactLab4_default_zOBM8n8W 
    DEPENDS ${DEPENDENT_TARGET_DIRLab4_default_zOBM8n8W}/${sourceFileNameLab4_default_zOBM8n8W}.s
    )
