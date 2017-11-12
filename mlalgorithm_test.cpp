#include <stdio.h>


int algorithm_selection = 0;

char* algorithm_list[] =
{
	"naivebayes",
	"svm",
	NULL
};

void print_algorithm_selection()
{
	char** temp = algorithm_list;
	int index = 0;
	printf("\n");
	for (; temp[index] != NULL; )
	{
		printf("[%d] %s ", index, temp[index]);
		printf("\n");
		index++;
	}

	printf("please select the machine learning algorithm \n");

	while (1)
	{
		int ret = getchar();
		if ((ret >= '0') && (ret <= '9'))
		{
			algorithm_selection *= 10;
			algorithm_selection += ret-'0';
		}
		else if (ret = '\n')
		{
			break;
		}
		else
			printf("wrong input！");
	}
	printf("you have selected [%d] %s \n", algorithm_selection, algorithm_list[algorithm_selection]);
}

int main()
{
	print_algorithm_selection();

}
