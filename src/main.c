#include <stdio.h>
#include <string.h>
#include "BPlusTree.h"

int main() {
    Tree *tree = createTree();
    long instruction_num;
    char instruction[10];
    long a, b;
    KV buffer[2001];
    long result_count;
    scanf("%ld", &instruction_num);
    for (long i = 0; i < instruction_num; i++) {
        scanf("%s", instruction);
        if (strcmp(instruction, "insert") == 0) {
            scanf("%ld %ld", &a, &b);
            if (insertToTree(tree, a, b) == -1) {
                printf("insert %ld %ld: -1\n", a, b);
            } else {
                printf("insert %ld %ld: 0\n", a, b);
            }
        } else if (strcmp(instruction, "delete") == 0) {
            scanf("%ld", &a);
            if (delFromTree(tree, a) == -1) {
                printf("delete %ld: -1\n", a);
            } else {
                printf("delete %ld: 0\n", a);
            }
        } else if (strcmp(instruction, "height") == 0) {
            printf("height: %ld\n", treeHeight(tree));
        } else if (strcmp(instruction, "get") == 0) {
            scanf("%ld", &a);
            printf("get %ld: %ld\n", a, equalSearch(tree, a));
        } else if (strcmp(instruction, "list") == 0) {
            scanf("%ld %ld", &a, &b);
            result_count = rangeSearch(tree, a, b, buffer, 2000);
            printf("list %ld %ld:", a, b);
            for (long j = 0; j < result_count; j++) {
                printf(" (%ld,%ld)", buffer[j].key, buffer[j].value);
            }
            printf("\n");
        } else {
            printf("unknown instruction: %s\n", instruction);
        }
    }
    return 0;
}
