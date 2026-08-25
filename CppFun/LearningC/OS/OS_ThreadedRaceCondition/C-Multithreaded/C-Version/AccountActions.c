#include "AccountActions.h"

bool outputBalance(BankAccount *account, FILE* outputFile)
{
  fprintf(outputFile, "\nBalance: $%d", account->balance);
  return true;
}

bool initAction(Action* action)
{
  action->iterations = 0;
  action->amount = 0;
  action->actionFunction = NULL;

  return true;
}

// Assumes the outputFP is open and writable!
// Does not close the outputFP!
bool outputAndDoBankingAction(BankAccount *account, Action *action, pthread_mutex_t * mtx, FILE *outputFP)
{
  if (action->actionFunction == NULL)
  {
    // ptr points to NULL
    return false;
  }

  if (action->actionFunction != deposit && action->actionFunction != withdraw)
  {
    // must point to a valid action function
    return false;
  }

/* Assume don't output
  if (outputFP == NULL)
  {
    FILE *errorFP = fopen("errorLog.txt", "w+");
    fprintf(errorFP, "Output File is NULL");
    fflush(errorFP);
    fclose(errorFP);
    exit(EXIT_FAILURE);
  }
*/

  for (uint64_t i = 0; i < action->iterations; i++)
  {
    if ((*action->actionFunction)(account, action->amount, mtx) == false)
    {
      return false;
    }

    if (outputFP != NULL)
    {
      outputBalance(account, outputFP);
    }
  }

  return true;
}
