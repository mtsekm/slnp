set(SOURCE_FILES Logger.cpp Types.cpp)
set(HEADER_FILES Logger.h Types.h Macros.h IPCTarget.h Registry.h)

### Server
add_library (slnp_server STATIC Server.cpp IPCTarget.cpp Registry.cpp ${SOURCE_FILES})
target_compile_features(slnp_server PRIVATE cxx_std_11)
target_compile_options(slnp_server PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

### Client
add_library (slnp_client STATIC Client.cpp ${SOURCE_FILES})
target_compile_features(slnp_client PRIVATE cxx_std_11)
target_compile_options(slnp_client PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)

## Install
install(TARGETS slnp_server slnp_client DESTINATION lib)
install(FILES ${HEADER_FILES} Server.h Client.h DESTINATION include/slnp)
