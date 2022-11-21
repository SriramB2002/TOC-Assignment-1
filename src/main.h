#ifndef MAIN_H
#define MAIN_H

typedef struct node {
    int x,y;
} Node;

typedef struct tuple {
    char* a;
    char* b;
    char* c;
} Tuple;

bool check (Node node);
void traverse (Node node, int i, int path[]);
char* or(int n, char* str1, char* str2);
int find(int n, char* str, Tuple tup[]);
void table_fill(int n, int map0[], int map1[]);
int find_set(int a, int p[]);

#endif
