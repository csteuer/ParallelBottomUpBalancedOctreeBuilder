message(STATUS "Configuring for platform Windows/MSVC.")

set(WIN32_COMPILE_DEFS
    WIN32                       # Windows system
    UNICODE                     # Use unicode
    _UNICODE                    # Use unicode
    _SCL_SECURE_NO_WARNINGS     # Calling any one of the potentially unsafe methods in the Standard C++ Library
    _CRT_SECURE_NO_DEPRECATE    # Disable CRT deprecation warnings
)

set(DEFAULT_COMPILE_DEFS_DEBUG
    ${WIN32_COMPILE_DEFS}
    _DEBUG                      # Debug build
)

set(DEFAULT_COMPILE_DEFS_RELEASE
    ${WIN32_COMPILE_DEFS}
    NDEBUG                      # Release build
)


set(WIN32_COMPILE_FLAGS
    /nologo                             # surpess startup banner
    /Zc:wchar_t                         # treat wchar_t as built-in type: yes
    /Zc:forScope                        # for loop variables goes out of scope after for loop ends
    /fp:fast                            # fast floating point arithmetic
    /MP                                 # Multithreaded build
    /wd4251 
    /W4                                 # warning level 4: enable all warnings
    /WX                                 # treat warnings as errors
    /GR                                 # runtime type information (enables dynamic_cast)

    #Release Flags
    $<$<CONFIG:Release>:/GL>            # whole program optimization: enable link-time code generation
    $<$<CONFIG:Release>:/GS->           # buffer security check: no 
    $<$<CONFIG:Release>:/GF>            # enable string pooling
    $<$<CONFIG:Release>:/Ox>            # full optimization
    $<$<CONFIG:Release>:/Ot>            # prefer fast code

    #Debug Flags        
    $<$<CONFIG:Debug>:/Zi>              # debug information format: program database
    $<$<CONFIG:Debug>:/Od>              # no optimization
    
    # Mixed
    /MD$<$<CONFIG:Debug>:d>              # MD[d] -> Use dynamic multithreaded MSVCRT (runtime library) [with debug symbols]
)


set(DEFAULT_COMPILE_FLAGS ${WIN32_COMPILE_FLAGS})


set(WIN32_LINKER_FLAGS
    "/NOLOGO /INCREMENTAL:NO /NXCOMPAT /DYNAMICBASE:NO"
    # NOLOGO                                            -> suppress logo
    # INCREMENTAL:NO                                    -> enable incremental linking: no
    # MANIFEST                                          -> generate manifest: yes
    # MANIFESTUAC:"level='asInvoker' uiAccess='false'"  -> uac execution level: asinvoker, uac bypass ui protection: false
    # NXCOMPAT                                          -> data execution prevention (dep): image is compatible with dep
    # DYNAMICBASE:NO                                    -> randomized base address: disable image randomization
)

set(DEFAULT_LINKER_FLAGS_DEBUG
    "${WIN32_LINKER_FLAGS} /DEBUG"
    # DEBUG        -> create debug info
)

set(DEFAULT_LINKER_FLAGS_RELEASE
    "${WIN32_LINKER_FLAGS} /OPT:REF /LTCG /OPT:ICF /DELAY:UNLOAD"
    # OPT:REF      -> references: eliminate unreferenced data
    # OPT:ICF      -> enable comdat folding: remove redundant comdats
    # LTCG         -> link time code generation: use link time code generation
    # DELAY:UNLOAD -> delay loaded dll: support unload
)

set(DEFAULT_LIBRARIAN_FLAGS_DEBUG # Flags for the visual c++ librarian (kind of a linker for static libraries)
    "/NOLOGO" 
)

set(DEFAULT_LIBRARIAN_FLAGS_RELEASE # Flags for the visual c++ librarian (kind of a linker for static libraries)
    "/NOLOGO /LTCG" 
)


# Add platform specific libraries for linking
set(EXTRA_LIBS "")
