#include <stdio.h>
#include <mpi.h>

#define send_data_tag 2001
#define return_data_tag 2002

struct clause
{
	int *literals;
	int size;
};
typedef struct clause clause;

void performDistribution(clause **clauses,int no_of_clauses,int no_of_variables,int firstmax,int secondmax);

void performDistribution(clause **clauses,int no_of_clauses,int no_of_variables,int firstmax,int secondmax)
{
	MPI_Status status;
	int my_id, root_process, ierr, i, num_rows, num_procs,
         an_id, num_rows_to_receive, avg_rows_per_process, 
         sender, num_rows_received, start_row, end_row, num_rows_to_send;

    
    //ierr = MPI_Init();
}