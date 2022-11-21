#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// Include any other libraries that you might use. 

// All your imports go here. 
// For example if you have a file: `src/nfa.h`
// import it as: #include "nfa.h"
#include "main.h"

int n;
char intVal[15];
char input[5001];
int strLen;
int* accepted;
int shmid;
FILE* ptr1;
FILE* ptr2;
int table[5000][5000];

bool check(Node node)
{
    return (node.x >= 0 && node.x <= n && node.y >= 0 && node.y <= n);
}

void traverse(Node node, int i, int path[])
{
    if (!check(node)) return;

    shmid = shmget(200000, 8, 0);
    accepted = shmat(shmid, 0, 0);
    if (accepted[0])
    {
        fclose(ptr1);
        return;
    }
    path[i] = node.x + node.y*(n+1);
    fprintf(ptr1, "[%d:%d] ", getppid(), getpid());
    fprintf(ptr1, "Exploring path: ");
    for (int j = 0; j <= i; j++)
    {
        fprintf(ptr1, "%d ", path[j]);
    }
    fprintf(ptr1, "\n");
    fflush(ptr1);

    if (i == strLen)
    {
        if (node.x == n && node.y == n)
        {
            accepted[0] = 1;
            fprintf(ptr1, "Accepted! Followed path: ");
            for (int j = 0; j <= i; j++)
            {
                fprintf(ptr1, "%d ", path[j]);
            }
            fprintf(ptr1, "\n");
            fflush(ptr1);
        }
        else
        {
            fprintf(ptr1, "Failed at path: ");
            for (int j = 0; j <= i; j++)
            {
                fprintf(ptr1, "%d ", path[j]);
            }
            fprintf(ptr1, "\n");
            fflush(ptr1);
        }
        return;
    }

    Node node1,node2;
    node1.x = node.x;
    node2.x = node.x;
    node1.y = node.y;
    node2.y = node.y;
    if (input[i] == '0')
    {
        node1.x++;
        node2.x--;
    }
    else if (input[i] == '1')
    {
        node1.y++;
        node2.y--;
    }

    if (fork() == 0)
    {
        traverse(node1, i + 1, path);
        exit(0);
    }
    if (fork() == 0)
    {
        traverse(node2, i + 1, path);
        exit(0);
    }
    wait(NULL);
    wait(NULL);
}

char* or(int n, char* str1, char* str2)
{
    char* ans = malloc(n);
    for (int i = 0; i < n; i++)
    {
        if (str1[i] == '1' || str2[i] == '1') ans[i] = '1';
        else if (str1[i] == '\0') ans[i] = '\0';
        else ans[i] = '0';
    }
    return ans;
}

int find(int n, char* str, Tuple tup[])
{
    int found = -1;
    for (int i = 0; i < n; i++)
    {
        if (strcmp(tup[i].a, str) == 0)
        {
            found = i;
            break;
        }
    }
    return found;
}

void table_fill(int n, int map0[], int map1[])
{
    for (int i = 0; i < (n+1)*(n+1); i++)
    {
        if (i != (n+1)*(n+1) - 1) table[i][(n+1)*(n+1) - 1] = 1;
    }
    for (int j = 0; j < (n+1)*(n+1); j++)
    {
        if (j != (n+1)*(n+1) - 1) table[(n+1)*(n+1) - 1][j] = 1;
    }
    bool f = false;
    while (!f)
    {
        f = true;
        for (int i = 0; i < (n+1)*(n+1); i++)
        {
            for (int j = 0; j < (n+1)*(n+1); j++)
            {
                if (i != j && ! table[i][j] && (table[map0[i]][map0[j]] || table[map1[i]][map1[j]]))
                {
                    table[i][j] = 1;
                    table[j][i] = 1;
                    f = false;
                }
            }
        }
    }
}

int find_set(int a, int p[])
{
    if (a == p[a]) return a;
    return p[a] = find_set(p[a], p);
}

int main(int argc, char *argv[])
{
    // Read all input from stdin and write all output to stdout.

    // Your code goes here...

    // Reading input
    FILE* ptr = fopen("input.txt", "r");
    ptr1 = fopen("2020A7PS0002H_t1.txt", "w+");
    fscanf(ptr, "%s", intVal);
    n = atoi(intVal);
    fscanf(ptr, "%s", input);
    fclose(ptr);
    strLen = strlen(input);

    // Task 1
    Node node;
    node.x = 0;
    node.y = 0;
    shmid = shmget(200000, 8, 0666 | IPC_CREAT);
    accepted = shmat(shmid, 0, 0);
    int path[strLen + 1];
    for (int i = 0; i < strLen + 1; i++)
    {
        path[i] = -1;
    }
    path[0] = 0;
    if (fork() == 0) traverse(node, 0, path);
    else
    {
        while (wait(NULL) > 0);
        shmctl(shmid, IPC_RMID, NULL);
        fclose(ptr1);

        // Task 2
        ptr2 = fopen("2020A7PS0002H_t2.txt", "w+");

        // Creating the NFA
        Tuple nfa[(n+1)*(n+1)];
        for (int i = 0; i < (n+1)*(n+1); i++)
        {
            nfa[i].a = "";
            nfa[i].b = "";
            nfa[i].c = "";
        }
        for (int j = 0; j <= n; j++)
        {
            for (int i = 0; i <= n; i++)
            {
                char* temp0 = malloc((n+1)*(n+1) + 1);
                char* temp1 = malloc((n+1)*(n+1) + 1);
                char* temp2 = malloc((n+1)*(n+1) + 1);
                for (int k = 0; k < (n+1)*(n+1); k++)
                {
                    temp0[k] = '0';
                    temp1[k] = '0';
                    temp2[k] = '0';
                }
                temp0[(n+1)*(n+1)] = '\0';
                temp1[(n+1)*(n+1)] = '\0';
                temp2[(n+1)*(n+1)] = '\0';
                temp0[i + (n+1)*j] = '1';
                if (i > 0) temp1[i-1 + (n+1)*j] = '1';
                if (i < n) temp1[i+1 + (n+1)*j] = '1';
                if (j > 0) temp2[i + (n+1)*(j-1)] = '1';
                if (j < n) temp2[i + (n+1)*(j+1)] = '1';
                nfa[i + (n+1)*j].a = temp0;
                nfa[i + (n+1)*j].b = temp1;
                nfa[i + (n+1)*j].c = temp2;
            }
        }

        // Converting NFA to DFA
        Tuple dfa[(n+1)*(n+1)];
        for (int i = 0; i < (n+1)*(n+1); i++)
        {
            dfa[i].a = malloc((n+1)*(n+1) + 1);
            dfa[i].b = malloc((n+1)*(n+1) + 1);
            dfa[i].c = malloc((n+1)*(n+1) + 1);
            dfa[i].a[(n+1)*(n+1)] = '\0';
            dfa[i].a[(n+1)*(n+1)] = '\0';
            dfa[i].a[(n+1)*(n+1)] = '\0';
        }
        
        char* start_string = malloc((n+1)*(n+1) + 1);
        for (int i = 0; i < (n+1)*(n+1); i++)
        {
            start_string[i] = '0';
        }
        start_string[(n+1)*(n+1)] = '\0';
        start_string[0] = '1';
        dfa[0].a = start_string;
        int next_free_index = 1;
        for (int i = 0; i < (n+1)*(n+1); i++)
        {
            char* temp0 = malloc((n+1)*(n+1) + 1);
            char* temp1 = malloc((n+1)*(n+1) + 1);

            for (int j = 0; j < (n+1)*(n+1); j++)
            {
                temp0[j] = '0';
                temp1[j] = '0';
            }
            temp0[(n+1)*(n+1)] = '\0';
            temp1[(n+1)*(n+1)] = '\0';
            for (int j = 0; j < (n+1)*(n+1); j++)
            {
                if (start_string[j] == '1')
                {
                    temp0 = or((n+1)*(n+1), temp0, nfa[j].b);
                    temp1 = or((n+1)*(n+1), temp1, nfa[j].c);
                }
            }
            dfa[i].b = temp0;
            dfa[i].c = temp1;

            if (find((n+1)*(n+1), temp0, dfa) == -1)
            {
                dfa[next_free_index].a = temp0;
                next_free_index++;
            }

            if (find((n+1)*(n+1), temp1, dfa) == -1)
            {
                dfa[next_free_index].a = temp1;
                next_free_index++;
            }
            if (i + 1 < (n+1)*(n+1)) start_string = dfa[i + 1].a;
        }

        int map0[(n+1)*(n+1)], map1[(n+1)*(n+1)];
        for (int i = 0; i < (n+1)*(n+1); i++)
        {
            char* x = dfa[i].b;
            char* y = dfa[i].c;
            map0[i] = find((n+1)*(n+1), x, dfa);
            map1[i] = find((n+1)*(n+1), y, dfa);
        }

        // Checking if DFA is minimized using table-filling algorithm
        table_fill(n, map0, map1);
        Tuple min_dfa[(n+1)*(n+1)];
        int idx = 0;
        int state_count = 0;
        for (int i = 0; i < (n+1)*(n+1); i++)
        {
            bool f = true;
            for (int j = 0; j < i; j++)
            {
                if (!table[i][j])
                {
                    f = false;
                    break;
                }
            }
            if (f)
            {
                min_dfa[idx].a = dfa[i].a;
                min_dfa[idx].b = dfa[i].b;
                min_dfa[idx].c = dfa[i].c;
                idx++;
                state_count++;
            }
        }

        for (int i = 0; i < (n+1)*(n+1); i++)
        {
            char* x = min_dfa[i].b;
            char* y = min_dfa[i].c;
            map0[i] = find((n+1)*(n+1), x, min_dfa);
            map1[i] = find((n+1)*(n+1), y, min_dfa);
        }

        // Printing output for final DFA
        int k = state_count;
        int final_dfa[2*k+1][k];
        for (int i = 0; i < 2*k+1; i++)
        {
            for (int j = 0; j < k; j++)
            {
                final_dfa[i][j] = 0;
            }
        }
        final_dfa[0][k-1]=1;
        for (int i = 1; i <= k; i++)
        {
            final_dfa[i][map0[i-1]]=1;
        }
        for (int i = k+1; i < 2*k+1; i++)
        {
            final_dfa[i][map1[i-k-1]]=1;
        }
        for (int i = 0; i < 2*k+1; i++)
        {
            for (int j = 0; j < k; j++)
            {
                fprintf(ptr2, "%d ", final_dfa[i][j]);
            }
            fprintf(ptr2, "\n");
            fflush(ptr2);
        }
        fclose(ptr2);
    }
    return 0;
}