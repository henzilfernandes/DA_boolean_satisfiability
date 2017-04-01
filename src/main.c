#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
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
    int no_of_variables=0,no_of_clauses=0;
    clause **clauses;

	  FILE *fp;
	  char * line = NULL;
    size_t len = 0;
    size_t read;
    char file_name[13];
    int i = 0,*count,j,firstmax,secondmax;
    
    MPI_Status status;
    int my_id, root_process, ierr, num_rows, num_procs,an_id, num_rows_to_receive, avg_rows_per_process, sender, num_rows_received, start_row, end_row, num_rows_to_send;
 
    ierr = MPI_Init(&argc,&argv);

    root_process = 0;
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  if(my_id == root_process)
  {
    printf("Master process\n");
    fp = fopen("sample.cnf","r");

    if(fp != NULL)
    {
      //strcpy(file_name,argv[1]);
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

      //display(clauses,no_of_clauses);
       
      topTwo(count,no_of_variables,&firstmax,&secondmax);
      printf("first:%d,second:%d\n",firstmax,secondmax);

      clause **send_clauses;
      int send_clauses_len,send_literals_len = no_of_variables-2;
      for(an_id = 1; an_id < num_procs; an_id++)
      {
          send_clauses = getClausesToSend(clauses,no_of_clauses,no_of_variables,firstmax,secondmax,an_id,&send_clauses_len);
          printf("send_len:%d,i=%d\n",send_clauses_len,an_id);

          ierr = MPI_Send( &send_literals_len, 1 ,MPI_INT,an_id, send_data_tag, MPI_COMM_WORLD);
      }

      ierr = MPI_Finalize();
      //performDistribution(clauses,no_of_clauses,no_of_variables,firstmax,secondmax);
  }
  else
  {
      printf("Unable to read %s",argv[1]);   
  }
  }
  else 
  {
    printf("slave process: %d\n",my_id);
    ierr = MPI_Finalize();
  }  
    


  //ierr = MPI_Finalize();
	return 1;
}