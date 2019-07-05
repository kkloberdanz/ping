WARN_FLAGS=-Wall -Wextra -Werror -Wpedantic

run: ping
	./ping

ping:
	gcc -o ping main.c -lSDL $(WARN_FLAGS) -ggdb3 -Og

clean:
	rm -f a.out
	rm -f core
