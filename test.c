#include <ecs.h>
#include <stdio.h>
#include <string.h>

#define COMMA ,

int main()
{
	t_ecs_vec vec;
	ECS_VEC_INIT(vec, char *, 10);
	for (int i = 0; i < 10; i++)
	{
		ECS_VEC_PUSH(vec, &(char*){strdup((char[2]){i + '0', '\0'})});
	}
	if (ECS_VEC_EMPTY(vec))
		printf("\nVector is empty after fill.\n");
	if (ECS_VEC_NOT_EMPTY(vec))
		printf("\nVector is not empty after fill.\n");

	ECS_VEC_REMOVE(vec, 0, { free(*(char **)item); });
	ECS_VEC_REMOVE(vec, 1, { free(*(char **)item); });
	ECS_VEC_ITER(vec, { printf("%s,  ", *(char **)item); });
	
	ECS_VEC_CLEAR_ALL(vec, { free(*(char **)item); });
	if (ECS_VEC_EMPTY(vec))
		printf("\nVector is empty after clear.\n");
	if (ECS_VEC_NOT_EMPTY(vec))
		printf("\nVector is not empty after clear.\n");
	
	ECS_VEC_DESTROY(vec);
	return EXIT_SUCCESS;
}
