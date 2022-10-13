cmake_minimum_required(VERSION 3.16)
project (decryptor)

add_executable(decryptor "2.1.c")

find_package(OpenSSL COMPONENTS Crypto REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
target_link_libraries(decryptor ${OPENSSL_CRYPTO_LIBRARY})