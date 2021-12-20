OUT = ./hw3
FILE_NAME = hw3
CFLAGS = -Wall -O3
CPPFLAGS = -std=c++17 -Wall -O3
CC = gcc

all: ${FILE_NAME}.*
	@if [ "$(suffix $<)" = ".cpp" ]; then\
		g++ ${CPPFLAGS} $< -o ${OUT};\
	elif [ "$(suffix $<)" = ".c" ]; then\
		gcc ${CFLAGS} $< -o ${OUT};\
	else\
		echo "There is no hw3.cpp and hw3.c file in the src/ folder.";\
    fi

clean:
	rm hw3
