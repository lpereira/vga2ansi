all:
	gcc -o vga2ansi vga2ansi.c -Wall -Wextra -Werror

clean:
	rm -f vga2ansi
