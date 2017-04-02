char *trim(char *str);
void processPStatement(char *line,int *no_of_variables,int *no_of_clauses);
void addClause(char *line,clause **clauses,int i,int *count);
void display(clause **clauses,int no_of_clauses);
void topTwo(int *count,int no_of_variables,int *firstmax,int *secondmax);
clause **getClausesToSend(clause **clauses,int no_of_clauses,int no_of_literals,int firstmax,int secondmax,int i,int *len);
int modifiedLValueBackward(int,int,int);
