add_executable(client client.cpp)
target_include_directories(client PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(client slnp_client)
install(TARGETS client DESTINATION bin)

add_executable(server server.cpp Sample.cpp)
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(server slnp_server)
install(TARGETS server DESTINATION bin)