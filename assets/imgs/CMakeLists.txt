set(SrcFiles
    launcher_icon.png
    launcher_image.png)

include(Tools)
if(WII)
    include(ToolsWii)
endif()

foreach(SrcFile ${SrcFiles}) 
    get_filename_component(SrcFileFull ${SrcFile} ABSOLUTE)
    set(DstFile ${CMAKE_CURRENT_BINARY_DIR}/${SrcFile})
    if(WII)
        set(DstFile ${DstFile}.tpl)
        add_custom_command(
            OUTPUT ${DstFile}
            COMMAND ${GXTEXCONV} -i ${SrcFileFull} -o ${DstFile}
            DEPENDS ${SrcFile}
            )
    elseif(NOT PNGCRUSH)
        add_custom_command(
            OUTPUT ${DstFile}
            COMMAND cp ${SrcFileFull} ${DstFile}
            DEPENDS ${SrcFile}
            )
    else()
        add_custom_command(
            OUTPUT ${DstFile}
            COMMAND ${PNGCRUSH} ${SrcFileFull} ${DstFile}
            DEPENDS ${SrcFile}
            )
    endif()
    makeTargetDirs(${DstFile})
    list(APPEND DstFiles ${DstFile})
endforeach()

add_custom_target(Images ALL DEPENDS ${DstFiles})
