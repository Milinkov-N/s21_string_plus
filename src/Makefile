program: clean s21_string.a main.o
	gcc main.o s21_string.a -lm -o program
	./program

main.o:
	gcc -c main.c

WWW = -std=c11 -Wall -Werror -Wextra
OS := $(shell uname)

ifeq ($(OS),Linux)
	CHFLAGS = -lcheck -lm -lpthread -lrt -lsubunit
else
	CHFLAGS = -lcheck -lm -lpthread
endif

all: clean s21_string.a

s21_string.o: s21_string.c s21_string.h
	gcc $(WWW) -c s21_string.c

s21_sprintf.o: s21_sprintf.c s21_sprintf.h
	gcc $(WWW) -c s21_sprintf.c

s21_sscanf.o: s21_sscanf.c s21_sscanf.h
	gcc $(WWW) -c s21_sscanf.c

s21_string.a: s21_string.o s21_sprintf.o s21_sscanf.o
	ar -crs s21_string.a s21_string.o s21_sprintf.o s21_sscanf.o

unit-test.o:
	gcc $(WWW) -c unit-test.c

test: clean s21_string.a
	gcc $(WWW) unit-test.c s21_string.a $(CHFLAGS) -o unit-test
	./unit-test

test_telega: clean s21_string.a
	gcc $(WWW) test_telega.c s21_string.a $(CHFLAGS) -o unit-test
	./unit-test

update_check:
	wget http://archive.ubuntu.com/ubuntu/pool/universe/c/check/check_0.15.2-2_amd64.deb
	sudo dpkg -i check_0.15.2-2_amd64.deb

gcov_report: s21_string.a
	gcc --coverage -lcheck s21_string.c s21_sprintf.c s21_sscanf.c unit-test.c -o ./gcov
	./gcov
	lcov -t "./gcov" -o report.info --no-external -c -d .
	genhtml -o report report.info
	open ./report/index.html
	rm -rf *.gcno *gcda *.gcov


clean:
	rm -rf *.o *.a unit-test
	rm -rf *.gcno *gcda *.gcov gcov program
	rm -rf report report.info
