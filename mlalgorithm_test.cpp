#include <stdio.h>
#include <functional>

void adaboost_test();
void naivebayes_test();
void apriori_test();
void fpgrowth_test();
void knn_test();
void decisiontree_test();
void cart_test();
void svm_test();
void kmeans_test();
void logisticregression_test();

int algorithm_selection = 0;

using namespace std;
typedef std::function<void()> Functor;

typedef struct {
    char* name;
    Functor api;
} AlgorithmAttr;

AlgorithmAttr algorithm_list[] = {
    {"naivebayes",naivebayes_test},
    {"apriori",apriori_test},
    {"fpgrowth",fpgrowth_test},
    {"knn",knn_test},
    {"decisiontree",decisiontree_test},
    {"cart",cart_test},
    {"svm",svm_test},
    {"kmeans",kmeans_test},
    {"logisticregression",logisticregression_test},
    {"adaboost",adaboost_test}
};

void print_algorithm_selection()
{
    AlgorithmAttr* temp = algorithm_list;
    int index = 0;
    printf("\n");
    for (; index < sizeof(algorithm_list)/sizeof(algorithm_list[0]); index++) {
        printf("[%d] %s \n", index, temp[index].name);
    }

    printf("please select the machine learning algorithm \n");

    while (1) {
        int ret = getchar();
        if ((ret >= '0') && (ret <= '9')) {
            algorithm_selection *= 10;
            algorithm_selection += ret-'0';
        } else if (ret = '\n') {
            break;
        } else
            printf("wrong input!\n");
    }
    printf("you have selected [%d] %s \n", algorithm_selection, algorithm_list[algorithm_selection].name);
}

int main()
{
    print_algorithm_selection();
    algorithm_list[algorithm_selection].api();
}
