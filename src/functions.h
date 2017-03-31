char *trim(char *str);
void processPStatement(char *line,int *no_of_variables,int *no_of_clauses);
void addClause(char *line,clause **clauses,int i);
void display(clause **clauses,int no_of_clauses);
