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
//void performDistribution(clause **clauses,int no_of_clauses,int no_of_variables,int firstmax,int secondmax);
int WalkSat(clause **clauses,int no_of_clauses,int *flipNum, int *retryNum,int *literalAssignment,int no_of_literals);
int Adaptive_novelty_plus(clause **clauses,int no_of_clauses,int *flipNum, int *retryNum,int *literalAssignment,int no_of_literals);
int Novelty(clause **clauses,int no_of_clauses,int *flipNum, int *retryNum,int *literalAssignment,int no_of_literals);

int main(int argc,char **argv)
{
    int no_of_variables=0,no_of_clauses=0;
    clause **clauses;

	  FILE *fp;
	  char * line = NULL;
    size_t len = 0;
    size_t read;
    char file_name[13];
    int i = 0,*count,j,firstmax,secondmax,*literalAssignment,flipNum,retryNum,*recv_result,temp1,temp2;
    
    MPI_Status status;
    int my_id, root_process, ierr, num_rows, num_procs,an_id,  sender,send_data_tag=2001,return_data_tag=2002 ;
 
    ierr = MPI_Init(&argc,&argv);

    root_process = 0;
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  if(my_id == root_process)
  {
    printf("Master process\n");
    printf("bsd\n");
    fp = fopen("sample4.cnf","r");
    printf("asd\n");
    if(fp != NULL)
    {
      //strcpy(file_name,argv[1]);
      printf("1\n");
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
       printf("2\n");
      topTwo(count,no_of_variables,&firstmax,&secondmax);
      printf("first:%d,second:%d\n",firstmax,secondmax);
      printf("3\n");
      clause **send_clauses;
      int send_clauses_len,send_literals_len = no_of_variables-2;


      for(an_id = 1; an_id < num_procs; an_id++)
      {
          send_clauses = getClausesToSend(clauses,no_of_clauses,no_of_variables,firstmax,secondmax,an_id,&send_clauses_len);
           
          ierr = MPI_Send( &send_literals_len, 1 ,MPI_INT,an_id, send_data_tag, MPI_COMM_WORLD);
        
          ierr = MPI_Send( &send_clauses_len, 1 ,MPI_INT,an_id, send_data_tag, MPI_COMM_WORLD);

          for(i=0;i<send_clauses_len;i++)
          {
            ierr = MPI_Send( &(send_clauses[i]->size), 1 ,MPI_INT,an_id, send_data_tag, MPI_COMM_WORLD);
            ierr = MPI_Send( &(send_clauses[i]->literals[0]), send_clauses[i]->size ,MPI_INT,an_id, send_data_tag, MPI_COMM_WORLD);
          }

      }
      
      recv_result = (int *)malloc((no_of_variables-2)*sizeof(int));
      j = 0;
      for(an_id = 1; an_id < num_procs ; an_id++)
      {
        ierr = MPI_Recv( &i, 1, MPI_INT, MPI_ANY_SOURCE,return_data_tag, MPI_COMM_WORLD, &status);
        sender = status.MPI_SOURCE;
        if(i == 1)
        {
            ierr = MPI_Recv( &recv_result[0], send_literals_len, MPI_INT, sender,return_data_tag, MPI_COMM_WORLD, &status);
            if(j == 0)
            {
              literalAssignment = (int *)malloc(no_of_variables*sizeof(int));  
              printf("\nSolution sender id : %d\n",sender);
              switch(sender)
              {
                case 1: literalAssignment[firstmax-1] = -1*firstmax;
                        literalAssignment[secondmax-1] = -1*secondmax;
                        break;
                case 2: literalAssignment[firstmax-1] = -1*firstmax;
                        literalAssignment[secondmax-1] = secondmax;
                        break;
                case 3: literalAssignment[firstmax-1] = firstmax;
                        literalAssignment[secondmax-1] = -1*secondmax;
                        break;
                case 4: literalAssignment[firstmax-1] = firstmax;
                        literalAssignment[secondmax-1] = secondmax;
                        break;
              }
              
              if(firstmax > secondmax) { temp1 = firstmax; temp2 = secondmax; }
              else {temp1 = secondmax; temp2 = firstmax; }

              j=0;
              for(i=0;i<send_literals_len;i++)
              {
                  if(j == temp1-1 || j == temp2-1)
                    j++;
                  literalAssignment[j++] = modifiedLValueBackward(recv_result[i],temp1,temp2);
              }
              j = 1;
            }
            
        }   
          
      }

      printf("\n********************SOLUTION*******************\n");
      for(i=0;i<no_of_variables;i++)
      {
        if(i%10 == 0)
          printf("\n");
        if(literalAssignment[i] < 0)
          printf("%d  ",literalAssignment[i]);
        else
          printf(" %d  ",literalAssignment[i]);
      }
      printf("\n***********************************************\n");
      ierr = MPI_Finalize();
      //performDistribution(clauses,no_of_clauses,no_of_variables,firstmax,secondmax);
    }
    else
    {
      printf("Unable to read sample4.cnf\n");   
    }
  }
  else 
  {
    printf("slave process: %d\n",my_id);
    int recv_literals_len,recv_clauses_len;
    clause **recv_clauses;
    ierr = MPI_Recv( &recv_literals_len, 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);
    ierr = MPI_Recv( &recv_clauses_len, 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);
    recv_clauses = (clause **)malloc(recv_clauses_len*sizeof(clause *));
    for(j=0;j<recv_clauses_len;j++)
    {
      recv_clauses[j] = (clause *)malloc(sizeof(clause));
      ierr = MPI_Recv( &(recv_clauses[j]->size), 1, MPI_INT, root_process, send_data_tag, MPI_COMM_WORLD, &status);
      recv_clauses[j]->literals = (int *)malloc((recv_clauses[j]->size)*sizeof(int));
      ierr = MPI_Recv( &(recv_clauses[j]->literals[0]), recv_clauses[j]->size, MPI_INT,root_process, send_data_tag, MPI_COMM_WORLD, &status);
      
    }

    literalAssignment = (int *)malloc(recv_literals_len*sizeof(int));
    
    if(my_id == 2 || my_id == 4)
        j = WalkSat(recv_clauses,recv_clauses_len,&flipNum, &retryNum,literalAssignment,recv_literals_len);
    else if(my_id == 3)
        j = Novelty(recv_clauses,recv_clauses_len,&flipNum, &retryNum,literalAssignment,recv_literals_len);
    else if(my_id == 1)
        j = Adaptive_novelty_plus(recv_clauses,recv_clauses_len,&flipNum, &retryNum,literalAssignment,recv_literals_len);

    ierr = MPI_Send( &j, 1, MPI_INT, root_process, return_data_tag, MPI_COMM_WORLD);
    if(j == 1)
      MPI_Send( &literalAssignment[0],recv_literals_len, MPI_INT, root_process, return_data_tag, MPI_COMM_WORLD);

    ierr = MPI_Finalize();
  }  
    


  //ierr = MPI_Finalize();
	return 1;
}