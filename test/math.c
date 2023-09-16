#include <stdio.h>

#include <lib/math.h>

int main(int argc, char* argv[]) {
    for (int i = 0; i < 100; i++) {
	printf("%.08f\n", frandrange(-1.1, 1.5));
    }
 //    for (int i = 0; i < 100; i++) {
	// printf("%d\n", randint());
 //    }
}
