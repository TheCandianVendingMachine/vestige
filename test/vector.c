#include <lib/vector.h>

typedef struct Reg {
	int a;
	int b;
} Reg;

int main(int argc, char* argv[]) {
	Vector v = VECTOR(int);
	VECTOR_PUSH(int, &v, 2);
	VECTOR_PUSH(int, &v, 1);
	VECTOR_PUSH(int, &v, 23);

	for (int i = 0; i < v.length; i++)
		printf("Item %d: %d\n", i, _VECTOR_GET(int, &v, i));

	Vector q = VECTOR(Reg);
	Reg r1 = {.a=1,.b=2};
	Reg r2 = {.a=3,.b=5};
	VECTOR_PUSH(Reg, &q, r1);
	VECTOR_PUSH(Reg, &q, r2);

	Reg t1 = _VECTOR_GET(Reg, &v, 0);
	printf("Item %d: (%d, %d)\n", 0, t1.a, t1.b);

	Reg t2 = {0};
	VECTOR_GET(Reg, &v, 1, &t2);
	printf("Item %d: (%d, %d)\n", 1, t2.a, t2.b);

	return 0;
}
