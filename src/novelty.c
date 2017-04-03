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
int flipMaxClauseLiteral(int clause_num,clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals,int *lastFlipVar);

int Novelty(clause **clauses,int no_of_clauses,int *flipNum, int *retryNum,int *literalAssignment,int no_of_literals)
{
    printf("EXECUTING NOVELTY ALGORITHM!\n");
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
        // HEURISTIC-1    
        /* If current setting of T/F satisfies given set of clauses,
        *  unSatClauseNum takes value -1, otherwise it takes the
        value of an unsatisfied clause number.*/
        
        unSatClauseNum = allClausesUnSatisfied(clauses,no_of_clauses,literalAssignment,no_of_literals,unSatClauses);
        if ( unSatClauseNum == -1 ) 
        {
          *flipNum = flip;
        *retryNum = try;
        return SUCCESS;
        }
      
        /* Select a clause randomly from clauses that is false */
        //clause_num = unSatClauseNum; 
      
        /* With prob prob_rand_walk, flip the value in a randomly
         * selected literal in clause_num
         */

        //r = fabs( (double)rand() / (double)(RAND_MAX) ); // r in [0,1]
       //  if ( r < PROB_RAND_WALK ) 
       //  {
       //  lit_num_in_clause = 1+(int) (((float)clauses[clause_num-1]->size)*rand() /(RAND_MAX+1.0));
       //  lit_number = abs(clauses[clause_num-1]->literals[lit_num_in_clause-1]); 


       // // Flipping ...
       //  if ( literalAssignment[lit_number-1] > 0  ) // TRUE
       //    literalAssignment[lit_number-1] = -lit_number; 
       //  else
       //    literalAssignment[lit_number-1] = lit_number; 
       //  }
          //else
          //{ // else flip the literal satisfying max clauses after flipping
        for(i = 0;i<unSatClauseNum;i++)
        {
            if(flipMaxClauseLiteral(unSatClauses[i],clauses,no_of_clauses,literalAssignment,no_of_literals,&lastFlipVar))
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

int allClausesUnSatisfied(clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals,int *unSatClauses)
{
  int cl,lit,this_clause,count= 0;
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
      //return (cl+1);
        unSatClauses[count] = cl+1;
        count++;
      }
  }
  if(count == 0)
      return -1;
  return count;
}



/*  Flips the literal in the specified clause which causes the 
 *  maximum increase in the number of satisfied clauses 
 */
int flipMaxClauseLiteral(int clause_num,clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals,int *lastFlipVar)
{
    int clauseArrayIndex = clause_num - 1;
    int litSatisfyingMaxClauses=-1,litSatisfyingSecondMaxClauses;
    int maxCountSoFar, currCount;
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
          if(litSatisfyingMaxClauses == -1)
            litSatisfyingSecondMaxClauses = -1;
          else
            litSatisfyingSecondMaxClauses = litSatisfyingMaxClauses;
          litSatisfyingMaxClauses = lit_number;
      }
      // revert to old value
        literalAssignment[lit_number-1] = old_value_of_literal;
    } // lit ..
   
    if ( satIncreased == 1 ) {
     // litSatisfyingMaxClauses is the literal whose flip
     // causes max number of clauses to be satisfied
     // so actually flip it now
      if(litSatisfyingMaxClauses != *lastFlipVar)
      {
        if ( literalAssignment[litSatisfyingMaxClauses-1]  > 0  ) // TRUE
            literalAssignment[litSatisfyingMaxClauses-1] = -litSatisfyingMaxClauses; 
        else
            literalAssignment[litSatisfyingMaxClauses-1] = litSatisfyingMaxClauses;
        *lastFlipVar = litSatisfyingMaxClauses;
        return 1;
      }
      else if(litSatisfyingSecondMaxClauses != -1)
      {
        if ( literalAssignment[litSatisfyingSecondMaxClauses-1]  > 0  ) // TRUE
            literalAssignment[litSatisfyingSecondMaxClauses-1] = -litSatisfyingSecondMaxClauses; 
        else
            literalAssignment[litSatisfyingSecondMaxClauses-1] = litSatisfyingSecondMaxClauses;
        *lastFlipVar = litSatisfyingSecondMaxClauses;
        return 1;
      }
      return 0;
      
    }
    return 0;
}

/* Specifies the number of clauses satisfied by current assignment
   of literals 
*/
// int numOfClausesSatisfied(clause **clauses,int no_of_clauses,int *literalAssignment,int no_of_literals)
// {
//   int cl,lit,this_clause;
//   int true_clause_count = 0;
//   int litNumber; // stores the current literal 
//   int litSign; // is the literal in its current assignment state or flipped sign state ?
//   int truthValueOfLiteral;
//   // A current assignment of literals exists
//       // Check each clause

//   for ( cl = 0 ; cl < no_of_clauses ; cl++ ) {
//       this_clause = 0;
//       for ( lit = 0 ; lit < clauses[cl]->size ; lit++ ) {
//         litNumber = abs(clauses[cl]->literals[lit]);
//         litSign  = (int)(clauses[cl]->literals[lit]/(abs(clauses[cl]->literals[lit])));
//         truthValueOfLiteral = literalAssignment[litNumber-1];  
//         if ( (litSign * truthValueOfLiteral)  > 0 ) { // TRUE
//             this_clause = 1;
//             true_clause_count++;
//             break;
//         }
//       } // lit ..
//   } // cl ..

//   return true_clause_count;
// }
