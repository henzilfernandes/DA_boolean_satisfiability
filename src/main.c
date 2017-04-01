#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
struct clause
{
	int *literals;
	int size;
};
typedef struct clause clause;

#include "functions.h"
void performDistribution(clause **clauses,int no_of_clauses,int no_of_variables,int firstmax,int secondmax);

int main(int argc,char **argv)
{
	printf("0");
    int no_of_variables=0,no_of_clauses=0;
    clause **clauses;

	FILE *fp;
	char * line = NULL;
    size_t len = 0;
    size_t read;
    char file_name[13];
    int i = 0,*count,j,firstmax,secondmax;
    
    fp = fopen(argv[1],"r");

	if(fp != NULL)
	{
        strcpy(file_name,argv[1]);
        while ((read = getline(&line, &len, fp)) != -1) 
        {
          line = trim(line);
          if(line[0] == 'p')
          {
            processPStatement(line,&no_of_variables,&no_of_clauses); 
            printf("no_of_variables=%d,no_of_clauses=%d\n",no_of_variables,no_of_clauses); 
            clauses = (clause **)malloc(no_of_clauses*sizeof(clause *));
            count = (int *)malloc((1+no_of_variables)*sizeof(int));
            for(j = 0;j<=no_of_variables;j++)
              count[j] = 0;
          }
          else if(line[0] == '-' || (line[0] >='0' && line[0] <='9'))
          {
            addClause(line,clauses,i,count);
            i++;
            if(i == no_of_clauses)
           	  break;
          }
          else if(line[0] == 'c')
          {
           
          }
          else
          {
          	printf("Error in %s file\n",file_name);
         	exit(1);
          }
       }

       display(clauses,no_of_clauses);
       
       topTwo(count,no_of_variables,&firstmax,&secondmax);
       
       performDistribution(clauses,no_of_clauses,no_of_variables,firstmax,secondmax);
	}
	else
	{
      printf("Unable to read %s",argv[1]);   
	}

	return 1;
}