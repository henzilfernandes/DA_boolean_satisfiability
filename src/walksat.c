#include<stdlib.h>
#include<limits.h>
#include<math.h>
#include<stdio.h>
#define PROB_RAND_WALK 0.01
#define MAX_FLIPS 10000
#define MAX_TRIES 100
#define SUCCESS 1
#define FAILURE 0

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

int WalkSat(clause **clauses,int no_of_clauses,int *flipNum, int *retryNum,int *literalAssignment,int no_of_literals)
{
    int try,flip,lit_no,lit_number,i;
    int clause_num,lit_num_in_clause;
    int unSatClauseNum;
    double r;
    //LITERAL lit_node;


    for ( try = 1 ; try <= MAX_TRIES ; try++) 
    {		
        /* Perform a random assignment of TRUE/FALSE to the literals in the clauses */
        //printf("TRY :%d\n",try);
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

	      r = fabs( (double)rand() / (double)(RAND_MAX) ); // r in [0,1]
	      if ( r < PROB_RAND_WALK ) 
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
		        i = flipMaxClauseSatisfyingLiteral(clause_num,clauses,no_of_clauses,literalAssignment,no_of_literals);
	      }

         } // flip ..
    } // try ..

    /* Not satisfied after max_tries also !!! return Failure */
    *flipNum = flip;
    *retryNum = try;
    return FAILURE;
}

int allClausesSatisfied(clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals)
{
	int cl,lit,this_clause;
	int litNumber; // stores the current literal 
	int litSign; // is the literal in its current assignment state or flipped sign state ?
	int truthValueOfLiteral;

	// A current assignment of literals exists
    // Check each clause
      
    for ( cl = 0 ; cl < no_of_clauses ; cl++ )
    {
	    this_clause = 0;
	    for ( lit = 0 ; lit < clauses[cl]->size ; lit++ ) 
	    {
		   litNumber = abs(clauses[cl]->literals[lit]);
		   litSign  = (int)(clauses[cl]->literals[lit]/(abs(clauses[cl]->literals[lit])));
		   truthValueOfLiteral = literalAssignment[litNumber-1];  
		   if ( (litSign * truthValueOfLiteral)  > 0 ) 
		   { // TRUE
		        this_clause = 1;
		        break;
		   }
	    } // lit ..
	    
	    if ( this_clause == 0 ) 
	    { // none of the literals were true
		  return (cl+1);
	    }
	}
	return -1;
}



/*  Flips the literal in the specified clause which causes the 
 *  maximum increase in the number of satisfied clauses 
 */
int flipMaxClauseSatisfyingLiteral(int clause_num,clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals)
{
    int clauseArrayIndex = clause_num - 1;
    int litSatisfyingMaxClauses;
    int maxCountSoFar, currCount ;
    int lit_number,lit,old_value_of_literal;
    int satIncreased;

    maxCountSoFar = numOfClausesSatisfied(clauses,no_of_clauses,literalAssignment,no_of_literals); 

    // flip each literal in the specified literal and see what
    // its effect on number of clauses satisfied is

    satIncreased = 0;
    for ( lit = 0 ; lit < clauses[clauseArrayIndex]->size ; lit++) {

	    lit_number = abs(clauses[clauseArrayIndex]->literals[lit]);
	    old_value_of_literal = literalAssignment[lit_number-1]; 

	    // Flip this literal
	    if ( literalAssignment[lit_number-1]  > 0  ) // TRUE
		    literalAssignment[lit_number-1] = -lit_number; 
	    else
		    literalAssignment[lit_number-1] = lit_number; 

	    // Count the number of clauses satisfied by this flipped literal
	    currCount = numOfClausesSatisfied(clauses,no_of_clauses,literalAssignment,no_of_literals);

	    if ( currCount >= maxCountSoFar ) { // Flipping increased number of satisfied clauses
	        satIncreased = 1;
	        maxCountSoFar = currCount;
	        litSatisfyingMaxClauses = lit_number;
	    }
	    // revert to old value
        literalAssignment[lit_number-1] = old_value_of_literal;
    } // lit ..
   
    if ( satIncreased == 1 ) {
     // litSatisfyingMaxClauses is the literal whose flip
     // causes max number of clauses to be satisfied
     // so actually flip it now
     
      if ( literalAssignment[litSatisfyingMaxClauses-1]  > 0  ) // TRUE
		literalAssignment[litSatisfyingMaxClauses-1] = -litSatisfyingMaxClauses; 
      else
		literalAssignment[litSatisfyingMaxClauses-1] = litSatisfyingMaxClauses;
	  return 1;
    }

    return 0;
}

/* Specifies the number of clauses satisfied by current assignment
   of literals 
*/
int numOfClausesSatisfied(clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals)
{
	int cl,lit,this_clause;
	int true_clause_count = 0;
	int litNumber; // stores the current literal 
	int litSign; // is the literal in its current assignment state or flipped sign state ?
	int truthValueOfLiteral;
	// A current assignment of literals exists
    	// Check each clause

	for ( cl = 0 ; cl < no_of_clauses ; cl++ ) {
	    this_clause = 0;
	    for ( lit = 0 ; lit < clauses[cl]->size ; lit++ ) {
		    litNumber = abs(clauses[cl]->literals[lit]);
		    litSign  = (int)(clauses[cl]->literals[lit]/(abs(clauses[cl]->literals[lit])));
		    truthValueOfLiteral = literalAssignment[litNumber-1];  
		    if ( (litSign * truthValueOfLiteral)  > 0 ) { // TRUE
		        this_clause = 1;
		        true_clause_count++;
		        break;
		    }
	    } // lit ..
	} // cl ..

	return true_clause_count;
}

// int IsTrueClause(int cl_num)
// {

// 	int litNumber; // stores the current literal 
// 	int litSign; // is the literal in its current assignment state or flipped sign state ?
// 	int truthValueOfLiteral;
// 	int lit;

// 	for ( lit = 0 ; lit < clauses[cl_num-1].literalCount ; lit++ ) {
// 		litNumber = abs(clauses[cl_num-1].literals[lit]);
// 		litSign  = (int)(clauses[cl_num-1].literals[lit]/(abs(clauses[cl_num-1].literals[lit])));
// 		truthValueOfLiteral = literals[litNumber-1].value;  
// 		if ( (litSign * truthValueOfLiteral)  > 0 ) { // TRUE
// 			return SUCCESS;
// 		}
// 	} // lit ..
// 	return FAILURE;
// }

// void printLiterals(void)
// {
//     int i;
//     for( i = 0 ; i < numberOfLiterals ; i++)
// 	printf("[%d] ",literals[i].value);
//     printf("\n");
// }

/*
LITERAL getLiteralFromClause(int clause_num,int lit_num_in_clause)
{
	int arrayIndex = clause_num - 1;
	int lit_in_clause_index = lit_num_in_clause-1;
	int lit_number, litArrayIndex;

	lit_number = abs(clauses[arrayIndex].literals[lit_in_clause_index]);
	litArrayIndex = lit_number - 1;

	return literals[litArrayIndex];
}
*/

