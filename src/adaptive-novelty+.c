#include<stdlib.h>
#include<limits.h>
#include<math.h>
#include<stdio.h>
#define MAX_FLIPS 10000
#define MAX_TRIES 100
#define SUCCESS 1
#define FAILURE 0
#define PHI 0.2

struct clause
{
	int *literals;
	int size;
};
typedef struct clause clause;

int WalkSat(clause **clauses,int no_of_clauses,int *flipNum, int *retryNum,int *literalAssignment,int no_of_literals);
int allClausesSatisfied(clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals);
int numOfClausesSatisfied(clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals);
int flipMaxClauseSatisfyingLiteral(int clause_num,clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals);

int Adaptive_novelty_plus(clause **clauses,int no_of_clauses,int *flipNum, int *retryNum,int *literalAssignment,int no_of_literals)
{
	printf("EXECUTING ADAPTIVE NOVELTY+ ALGORITHM!\n");
    int try,flip,lit_no,lit_number,i;
    int clause_num,lit_num_in_clause;
    int unSatClauseNum;
    double r,k;
    //LITERAL lit_node;


    for ( try = 1 ; try <= MAX_TRIES ; try++) 
    {		
        /* Perform a random assignment of TRUE/FALSE to the literals in the clauses */
        //printf("TRY :%d\n",try);
        k = 0;
    	srand(time(NULL));
    	for( lit_no = 0 ; lit_no < no_of_literals ; lit_no++ ) 
    	{
        	r = fabs( (double)rand() / (double)(RAND_MAX) ); // r in [0,1]
	    	if ( r < 0.5 )
		    	literalAssignment[lit_no] = -(lit_no+1);
	    	else
		    	literalAssignment[lit_no] = (lit_no+1);
    	}
	
        
 	    for ( flip = 1 ; flip <= MAX_FLIPS ; flip++) 
 	    {
	      // HEURISTIC-1    
	      /* If current setting of T/F satisfies given set of clauses,
	      *  unSatClauseNum takes value -1, otherwise it takes the
	      value of an unsatisfied clause number.*/
	      
	      unSatClauseNum = allClausesSatisfied(clauses,no_of_clauses,literalAssignment,no_of_literals);
	      if ( unSatClauseNum == -1 ) 
	      {
	        *flipNum = flip;
		    *retryNum = try;
		    return SUCCESS;
	      }
	    
	      /* Select a clause randomly from clauses that is false */
	      clause_num = unSatClauseNum; 
		  
	      /* With prob prob_rand_walk, flip the value in a randomly
	       * selected literal in clause_num
	       */
          if(k >= 0.75)
          	k = k-(k*PHI)/2;
	      r = fabs( (double)rand() / (double)(RAND_MAX) ); // r in [0,1]
	      if ( r < k ) 
	      {
		    lit_num_in_clause = 1+(int) (((float)clauses[clause_num-1]->size)*rand() /(RAND_MAX+1.0));
		    lit_number = abs(clauses[clause_num-1]->literals[lit_num_in_clause-1]); 


		   // Flipping ...
		    if ( literalAssignment[lit_number-1] > 0  ) // TRUE
			    literalAssignment[lit_number-1] = -lit_number; 
		    else
			    literalAssignment[lit_number-1] = lit_number; 
	      }
          else
          { // else flip the literal satisfying max clauses after flipping
		        if(flipMaxClauseSatisfyingLiteral(clause_num,clauses,no_of_clauses,literalAssignment,no_of_literals) == 0)
		        	k = k+(1-k)*PHI;
	      }

         } // flip ..
    } // try ..

    /* Not satisfied after max_tries also !!! return Failure */
    *flipNum = flip;
    *retryNum = try;
    return FAILURE;
}