WARN_FLAGS=-Wall -Wextra -Werror -Wpedantic

run: ping
	./ping

ping:
	gcc -o ping main.c -lSDL $(WARN_FLAGS) -ggdb3 -Os

clean:
	rm -f a.out
	rm -f core
	rm -f ping
