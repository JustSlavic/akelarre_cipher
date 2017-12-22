cmake_minimum_required(VERSION 3.7)
project(akelarre)

set(CMAKE_C_STANDARD 11)

set(MAIN main.c)
set(AKE  akelarre.c akelarre.h)
set(KEYGEN keygen.c keygen.h)

add_executable(akelarre ${MAIN} ${AKE} ${KEYGEN})