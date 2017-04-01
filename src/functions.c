#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
struct  clause
{
    int *literals;
    int size;
};
typedef struct clause clause;

#include "functions.h"

char *trim(char *str)
{
    size_t len = 0;
    char *frontp = str;
    char *endp = NULL;

    if( str == NULL ) { return NULL; }
    if( str[0] == '\0' ) { return str; }

    len = strlen(str);
    endp = str + len;

    /* Move the front and back pointers to address the first non-whitespace
     * characters from each end.
     */
    while( isspace((unsigned char) *frontp) ) { ++frontp; }
    if( endp != frontp )
    {
        while( isspace((unsigned char) *(--endp)) && endp != frontp ) {}
    }

    if( str + len - 1 != endp )
            *(endp + 1) = '\0';
    else if( frontp != str &&  endp == frontp )
            *str = '\0';

    /* Shift the string so that it starts at str so that if it's dynamically
     * allocated, we can still free it on the returned pointer.  Note the reuse
     * of endp to mean the front of the string buffer now.
     */
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) { *endp++ = *frontp++; }
            *endp = '\0';
    }


    return str;
}

void processPStatement(char *line,int *no_of_variables,int *no_of_clauses)
{
  char *token;
  int k = 0;
  token = strtok(line," ");
  while(token != NULL)
  {
    k++;
    if(k == 3)
        *no_of_variables = atoi(token);
    if(k == 4)
        *no_of_clauses = atoi(token);
    token = strtok(NULL," ");
  }
}

void addClause(char *line,clause **clauses,int i,int *count)
{
    int len = 0,num;
    int size = strlen(line);
    char *line2 = (char *)malloc((1+strlen(line))*sizeof(char));

    for(len = 0;len < size;len++)
        line2[len] = line[len];
    line2[len] = '\n';

    //strcpy(line2,line);
    len = 0;
    char *token = strtok(line," ");

    while(token != NULL)
    {
        len++;
        token = strtok(NULL," ");
    }

    clauses[i] = (clause *)malloc(sizeof(clause));
    clauses[i]->size = len-1;
    clauses[i]->literals = (int *)malloc((len-1)*sizeof(int));

    token = strtok(line2," ");
    len = 0;
    while(token != NULL)
    {
        num = atoi(token);
        if(num != 0)
        {
            clauses[i]->literals[len] = num; 
            if(num<0)
                num = -1*num;
            count[num]++;
        }
        len++;
        token = strtok(NULL," ");
    }
}

void display(clause **clauses,int no_of_clauses)
{
    int i,j;
    for(i=0;i<no_of_clauses;i++)
    {
        for(j=0;j<clauses[i]->size;j++)
            printf("%d ",clauses[i]->literals[j]);
        printf("\n");
    }
    printf("\n");
}

void topTwo(int *count,int no_of_variables,int *firstmax,int *secondmax)
{
    int i;
    if(count[1]>count[2])
    {
        *firstmax = 1;
        *secondmax = 2;
    }
    else
    {
        *firstmax = 2;
        *secondmax = 1;
    }

    for(i=3;i<=no_of_variables;i++)
    {
        if(count[i]>*firstmax)
        {
            *secondmax = *firstmax;
            *firstmax = i;
        }
        else if(count[i]>*secondmax)
            *secondmax = i;
    }
}