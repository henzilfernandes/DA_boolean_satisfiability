#include<stdlib.h>
#include<limits.h>
#include<math.h>
#include<stdio.h>
#define PROB_RAND_WALK 0.5
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

int Novelty(clause **clauses,int no_of_clauses,int *flipNum, int *retryNum,int *literalAssignment,int no_of_literals);
int allClausesUnSatisfied(clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals,int *unSatClauses);
int numOfClausesSatisfied(clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals);
int r_flipMaxClauseLiteral(int clause_num,clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals,int *lastFlipVar);

int R_novelty(clause **clauses,int no_of_clauses,int *flipNum, int *retryNum,int *literalAssignment,int no_of_literals)
{
    
    printf("EXECUTING R-NOVELTY ALGORITHM!\n");

    int try,flip,lit_no,lit_number,i;
    int clause_num,lit_num_in_clause,lastFlipVar;
    int *unSatClauses = (int *)malloc(no_of_clauses*sizeof(int));
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
  
      lastFlipVar = -1;
      for ( flip = 1 ; flip <= MAX_FLIPS ; flip++) 
      {
        
        unSatClauseNum = allClausesUnSatisfied(clauses,no_of_clauses,literalAssignment,no_of_literals,unSatClauses);
        if ( unSatClauseNum == -1 ) 
        {
          *flipNum = flip;
        *retryNum = try;
        return SUCCESS;
        }
         
        //printf("unSatClauseNum = %d\n",unSatClauseNum);
        // for(i = 0;i<unSatClauseNum;i++)
        // {
        //   printf("%d ",unSatClauses[i]);
        // }
        //printf("\n");
        for(i = 0;i<unSatClauseNum;i++)
        {
            if(r_flipMaxClauseLiteral(unSatClauses[i],clauses,no_of_clauses,literalAssignment,no_of_literals,&lastFlipVar))
              break;
        }  
        
        //}

        } // flip ..
    } // try ..

    /* Not satisfied after max_tries also !!! return Failure */
    *flipNum = flip;
    *retryNum = try;
    return FAILURE;
}





/*  Flips the literal in the specified clause which causes the 
 *  maximum increase in the number of satisfied clauses 
 */
int r_flipMaxClauseLiteral(int clause_num,clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals,int *lastFlipVar)
{
    int clauseArrayIndex = clause_num - 1;
    int litSatisfyingMaxClauses=-1,litSatisfyingSecondMaxClauses;
    int maxCountSoFar, currCount,secondMaxCount;
    int lit_number,lit,old_value_of_literal;
    int satIncreased,n;
    double r;

    maxCountSoFar = numOfClausesSatisfied(clauses,no_of_clauses,literalAssignment,no_of_literals); 
    secondMaxCount = maxCountSoFar;
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
          
          if(litSatisfyingMaxClauses == -1)
            litSatisfyingSecondMaxClauses = -1;
          else
          {
            litSatisfyingSecondMaxClauses = litSatisfyingMaxClauses;
            secondMaxCount = maxCountSoFar;
          }
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
      n = maxCountSoFar-secondMaxCount;
      if(litSatisfyingMaxClauses != *lastFlipVar)
      {
        if ( literalAssignment[litSatisfyingMaxClauses-1]  > 0  ) // TRUE
            literalAssignment[litSatisfyingMaxClauses-1] = -litSatisfyingMaxClauses; 
        else
            literalAssignment[litSatisfyingMaxClauses-1] = litSatisfyingMaxClauses;
        *lastFlipVar = litSatisfyingMaxClauses;
        return 1;
      }
      else if(litSatisfyingSecondMaxClauses != -1 && (n > 0))
      {
         
        srand(time(NULL));
        r = fabs( (double)rand() / (double)(RAND_MAX) );
        if(r < 0.5 && n > 1)
        {
          r=1;
        }
        else if(r < 0.5 && n == 1)
        {
          if(fabs( (double)rand() / (double)(RAND_MAX) ) >= 2*r )
            r=2;
          else 
            r=1;
        }
        else if(r>=0.5 && n == 1)
          r=2;
        else if(r>=0.5 && n >1)
        {
          if(fabs( (double)rand() / (double)(RAND_MAX)) >= 2*(r-0.5))
            r = 2;
          else 
            r=1;
        }
        
        if(r == 1)
        {
          if ( literalAssignment[litSatisfyingMaxClauses-1]  > 0  ) // TRUE
            literalAssignment[litSatisfyingMaxClauses-1] = -litSatisfyingMaxClauses; 
          else
            literalAssignment[litSatisfyingMaxClauses-1] = litSatisfyingMaxClauses;
          *lastFlipVar = litSatisfyingMaxClauses;
        }
        else
        {
          if ( literalAssignment[litSatisfyingSecondMaxClauses-1]  > 0  ) // TRUE
            literalAssignment[litSatisfyingSecondMaxClauses-1] = -litSatisfyingSecondMaxClauses; 
          else
            literalAssignment[litSatisfyingSecondMaxClauses-1] = litSatisfyingSecondMaxClauses;
          *lastFlipVar = litSatisfyingSecondMaxClauses;
        }
        
        return 1;
      }
      return 0;
      
    }
    return 0;
}