add_executable(Client_cli)
target_sources(Client_cli PRIVATE Client_cli.cpp)

target_link_libraries(Client_cli PRIVATE psi)
target_link_libraries(Client_cli PRIVATE OpenMP::OpenMP_CXX)


add_executable(Server_cli)
target_sources(Server_cli PRIVATE Server_cli.cpp)

target_link_libraries(Server_cli PRIVATE psi)
target_link_libraries(Server_cli PRIVATE OpenMP::OpenMP_CXX)

add_executable(AidServer_cli)
target_sources(AidServer_cli PRIVATE AidServer_cli.cpp)

target_link_libraries(AidServer_cli PRIVATE psi)
target_link_libraries(AidServer_cli PRIVATE OpenMP::OpenMP_CXX)