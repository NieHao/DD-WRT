set(libs
     polarssl
)

if(USE_PKCS11_HELPER_LIBRARY)
    set(libs ${libs} pkcs11-helper)
endif(USE_PKCS11_HELPER_LIBRARY)

add_executable(cert_app cert_app.c)
target_link_libraries(cert_app ${libs})

add_executable(crl_app crl_app.c)
target_link_libraries(crl_app ${libs})

INSTALL(TARGETS cert_app crl_app
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
