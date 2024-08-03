SRC := "tou_test.c"
BIN := "tou_test.exe"
DOXYBIN := "e:/doxygen/doxygen.exe"

# Build srcs
build:
	gcc {{SRC}} -o {{BIN}} -std=c11 #-Wall

# Run bin
run:
	./{{BIN}}

# Build and run
rebuild: build run

# Run gcc with -E (preprocess only)
preproc:
	gcc {{SRC}} -o {{BIN}} -std=c11 -E

# Regenerate documentation
doxy:
	{{DOXYBIN}}

# Show this list
list:
	just -l

