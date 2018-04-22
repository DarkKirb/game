set(SrcFiles
    vshader.v.pica
    )

include(Tools)

if(3DS)
    include(Tools3DS)
    foreach(SrcFile ${SrcFiles})
        get_filename_component(SrcFileFull ${SrcFile} ABSOLUTE)
        set(DstFile ${CMAKE_CURRENT_BINARY_DIR}/${SrcFile})
        add_shbin(${DstFile} ${SrcFile})
        makeTargetDirs(${DstFile})
        list(APPEND DstFiles ${DstFile})
    endforeach()
    add_custom_target(Shaders ALL DEPENDS ${DstFiles})
endif()
