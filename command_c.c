/**
 * @file command_c.c
 * @brief Implementation of command C functionality to add new vaccine batches
 * to the system.
 *
 * This file contains the implementation of the commandC function and its helper
 * functions. It handles parsing the command arguments, validating the input,
 * and adding the new vaccine batch to the system.
 *
 * Author: Vicente B. Duarte
 */

#include "constants.h"
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Frees allocated memory for batch and name in case of parsing failure.
 *
 * @param batch Pointer to the batch identifier.
 * @param name Pointer to the vaccine name.
 */
static void freeParseResources(char **batch, char **name) {
  if (*batch) {
    free(*batch);
    *batch = NULL;
  }

  if (*name) {
    free(*name);
    *name = NULL;
  }
}

/**
 * @brief Parses the batch identifier from the arguments.
 *
 * @param token The current token from the arguments.
 * @param batch Pointer to store the batch identifier.
 * @return int 1 if successful, 0 otherwise.
 */
static int parseBatch(char *token, char **batch) {
  *batch = strdup(token);
  return *batch != NULL;
}

/**
 * @brief Parses the validation date from the arguments.
 *
 * @param token The current token from the arguments.
 * @param validation Pointer to store the validation date.
 * @return int 1 if successful, 0 otherwise.
 */
static int parseValidationDate(char *token, Date *validation) {
  return sscanf(token, "%d-%d-%d", &validation->day, &validation->month,
                &validation->year) == 3;
}

/**
 * @brief Parses the arguments for the command efficiently using a single scan.
 *
 * @param args The command arguments.
 * @param batch Pointer to store the batch identifier.
 * @param validation Pointer to store the validation date.
 * @param doses Pointer to store the number of doses.
 * @param name Pointer to store the vaccine name.
 * @return int 1 if the arguments were successfully parsed, 0 otherwise.
 */
static int parseArgumentsC(char *args, char **batch, Date *validation,
                           int *doses, char **name) {
  char *token;
  char *rest = args;

  // Parse batch identifier
  token = strtok_r(rest, " ", &rest);
  if (!token || !parseBatch(token, batch))
    return 0;

  // Parse validation date
  token = strtok_r(NULL, " ", &rest);
  if (!token || !parseValidationDate(token, validation)) {
    freeParseResources(batch, name);
    return 0;
  }

  // Parse doses
  token = strtok_r(NULL, " ", &rest);
  if (!token) {
    freeParseResources(batch, name);
    return 0;
  }
  *doses = atoi(token);

  // Parse vaccine name
  if (!rest || *rest == '\0') {
    freeParseResources(batch, name);
    return 0;
  }
  *name = strdup(rest);
  if (!*name) {
    freeParseResources(batch, name);
    return 0;
  }

  return 1;
}

/**
 * @brief Validates the parsed arguments for adding a new vaccine batch.
 *
 * @param batch The batch identifier.
 * @param validation The validation date.
 * @param doses The number of doses.
 * @param name The vaccine name.
 * @param currentDate The current date.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 * @return int 1 if the arguments are valid, 0 otherwise.
 */
static int validateNewVaccine(char *batch, Date validation, int doses,
                              char *name, Date currentDate, int portuguese) {
  if (!isValidBatch(batch)) {
    printf("%s\n", portuguese ? "lote inválido" : "invalid batch");
    return 0;
  }

  if (!isValidName(name)) {
    printf("%s\n", portuguese ? "nome inválido" : "invalid name");
    return 0;
  }

  if (!isValidDate(validation, currentDate)) {
    printf("%s\n", portuguese ? "data inválida" : "invalid date");
    return 0;
  }

  if (doses <= 0) {
    printf("%s\n", portuguese ? "quantidade inválida" : "invalid quantity");
    return 0;
  }

  return 1;
}

/**
 * @brief Adds the new vaccine batch to the system data structures.
 *
 * @param batch The batch identifier.
 * @param name The vaccine name.
 * @param validation The validation date.
 * @param doses The number of doses.
 * @param hashTable The hash table of vaccine lots.
 * @param nameHashTable The hash table of vaccine names.
 * @param vaccineList The list of vaccine lots.
 * @param hashSize The size of the hash table.
 * @param vaccineCount The current count of vaccine lots.
 * @param maxVaccines The maximum number of vaccine lots allowed.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
static void addNewVaccineToSystem(char *batch, char *name, Date validation,
                                  int doses, VaccineLot **hashTable,
                                  VaccineNameIndex **nameHashTable,
                                  VaccineLot **vaccineList, int hashSize,
                                  int *vaccineCount, int maxVaccines,
                                  int portuguese) {
  if (*vaccineCount >= maxVaccines) {
    printf("%s\n", portuguese ? "demasiadas vacinas" : "too many vaccines");
    return;
  }
  if (findVaccineByBatch(hashTable, batch, hashSize) != NULL) {
    printf("%s\n",
           portuguese ? "número de lote duplicado" : "duplicate batch number");
    return;
  }

  VaccineLot *newLot = createVaccineLot(batch, name, validation, doses);
  if (!newLot) {
    printf("%s\n", portuguese ? "sem memória" : "No memory");
    return;
  }

  addVaccineLotToHash(hashTable, newLot, hashSize);
  addVaccineLotToNameIndex(nameHashTable, newLot, hashSize);

  newLot->next_vaccine = *vaccineList;
  *vaccineList = newLot;

  (*vaccineCount)++;
  printf("%s\n", batch);
}

/**
 * @brief Adds a new vaccine batch to the system.
 *
 * @param args The command arguments.
 * @param hashTable The hash table of vaccine lots.
 * @param nameHashTable The hash table of vaccine names.
 * @param vaccineList The list of vaccine lots.
 * @param hashSize The size of the hash table.
 * @param vaccineCount The current count of vaccine lots.
 * @param maxVaccines The maximum number of vaccine lots allowed.
 * @param currentDate The current date.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
void commandC(char *args, VaccineLot **hashTable,
              VaccineNameIndex **nameHashTable, VaccineLot **vaccineList,
              int hashSize, int *vaccineCount, int maxVaccines,
              Date currentDate, int portuguese) {
  char *batch = NULL;
  Date validation;
  int doses = 0;
  char *name = NULL;

  if (!parseArgumentsC(args, &batch, &validation, &doses, &name)) {
    printf("%s\n", portuguese ? "sem memória" : "No memory");
    return;
  }

  if (!validateNewVaccine(batch, validation, doses, name, currentDate,
                          portuguese)) {
    free(batch);
    free(name);
    return;
  }

  addNewVaccineToSystem(batch, name, validation, doses, hashTable,
                        nameHashTable, vaccineList, hashSize, vaccineCount,
                        maxVaccines, portuguese);

  free(batch);
  free(name);
}