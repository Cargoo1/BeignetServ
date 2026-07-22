#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void ft_handler(int sig) {
	(void)sig;
	return ;
}

size_t ft_strlen(char *str) {
	size_t len = 0;

	if(!str)
		return len;
	while (str[len])
		len++;
	return len;
}

void ft_putstr(char *str) {
	write(1, str, ft_strlen(str));
}

int main(void) {
	ft_putstr("Hello wordl!\nI'm going to sleep\n");
	signal(SIGINT, ft_handler);
	sleep(5);
	return (0);
}