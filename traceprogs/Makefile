SRCS = simpleloop.c matmul.c blocked.c is_palindrome.c
PROGS = simpleloop matmul blocked is_palindrome

all : $(PROGS)

$(PROGS) : % : %.c
	gcc -Wall -g -o $@ $<


traces: $(PROGS)
	./runit simpleloop
	./runit matmul 100
	./runit blocked 100 25
	./runit is_palindrome

.PHONY: clean
clean : 
	rm -f simpleloop matmul blocked tr-*.ref *.marker *~
