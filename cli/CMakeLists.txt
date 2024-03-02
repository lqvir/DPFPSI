add_executable(Client_cli)
target_sources(Client_cli PRIVATE Client_cli.cpp)

target_link_libraries(Client_cli PRIVATE psi)
target_link_libraries(Client_cli PRIVATE OpenMP::OpenMP_CXX)