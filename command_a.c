/**
 * @file command_a.c
 * @brief Implementation of command A functionality for applying vaccine doses
 * to users.
 *
 * This file contains the implementation of the commandA function and its helper
 * functions. It handles parsing the command arguments, checking if the user is
 * already vaccinated, finding the appropriate vaccine lot, and recording the
 * inoculation.
 *
 * Author: Vicente B. Duarte
 */

#include "constants.h"
#include "project.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Extracts the user name from a quoted argument.
 *
 * @param args The command arguments starting with a quote.
 * @param userName Pointer to store the extracted user name.
 * @return const char* Pointer to the rest of the arguments after the quote, or
 * NULL on error.
 */
static const char *extractQuotedName(const char *args, char **userName) {
  const char *endQuote = strchr(args + 1, '"');
  if (endQuote == NULL) {
    return NULL; // Unclosed quotes
  }

  size_t nameLen = endQuote - args - 1;
  *userName = (char *)malloc(nameLen + 1);
  if (*userName == NULL) {
    return NULL; // Memory allocation failed
  }

  memcpy(*userName, args + 1, nameLen);
  (*userName)[nameLen] = '\0';
  return endQuote + 1;
}

/**
 * @brief Extracts the user name from an unquoted argument.
 *
 * @param args The command arguments.
 * @param userName Pointer to store the extracted user name.
 * @return const char* Pointer to the rest of the arguments after the space, or
 * NULL on error.
 */
static const char *extractUnquotedName(const char *args, char **userName) {
  const char *space = strchr(args, ' ');
  if (space == NULL) {
    return NULL; // Missing vaccine name
  }

  size_t nameLen = space - args;
  *userName = (char *)malloc(nameLen + 1);
  if (*userName == NULL) {
    return NULL; // Memory allocation failed
  }

  memcpy(*userName, args, nameLen);
  (*userName)[nameLen] = '\0';
  return space + 1;
}

/**
 * @brief Extracts the vaccine name from the remaining arguments.
 *
 * @param rest The remaining command arguments.
 * @param vaccineName Pointer to store the extracted vaccine name.
 * @return int 1 if successful, 0 otherwise.
 */
static int extractVaccineName(const char *rest, char **vaccineName) {
  while (*rest && isspace(*rest)) {
    rest++;
  }

  if (*rest == '\0') {
    return 0; // Missing vaccine name
  }

  *vaccineName = strdup(rest);
  return (*vaccineName != NULL);
}

/**
 * @brief Extracts the user name and vaccine name from the command arguments.
 *
 * @param args The command arguments.
 * @param userName Pointer to store the extracted user name.
 * @param vaccineName Pointer to store the extracted vaccine name.
 * @return int 1 if extraction was successful, 0 otherwise.
 */
static int extractArguments(const char *args, char **userName,
                            char **vaccineName) {
  const char *rest;
  if (args[0] == '"') {
    rest = extractQuotedName(args, userName);
    if (rest == NULL) {
      return 0;
    }

  } else {
    rest = extractUnquotedName(args, userName);
    if (rest == NULL) {
      return 0;
    }
  }

  if (*userName == NULL) {
    return 0;
  }

  if (!extractVaccineName(rest, vaccineName)) {
    free(*userName);
    return 0;
  }

  return 1;
}

/**
 * @brief Checks if an inoculation matches the criteria.
 *
 * @param inoc The inoculation record.
 * @param currentDate The current date.
 * @param vaccineEntry The vaccine entry.
 * @return int 1 if it matches, 0 otherwise.
 */
static int inoculationMatches(const Inoculation *inoc, Date currentDate,
                              const VaccineNameIndex *vaccineEntry) {
  if (inoc->date.day == currentDate.day &&
      inoc->date.month == currentDate.month &&
      inoc->date.year == currentDate.year) {
    for (int j = 0; j < vaccineEntry->lotCount; j++) {
      if (strcmp(inoc->lot, vaccineEntry->lots[j]->lot) == 0) {
        return 1;
      }
    }
  }
  return 0;
}

/**
 * @brief Checks if a user has already been vaccinated today with a specific
 * vaccine.
 *
 * @param userHashTable The hash table of user indexes.
 * @param nameHashTable The hash table of vaccine names.
 * @param userName The user's name.
 * @param vaccineName The vaccine name.
 * @param currentDate The current date.
 * @param hashSize The size of the hash tables.
 * @return int 1 if already vaccinated today, 0 otherwise.
 */
static int isAlreadyVaccinated(UserIndex **userHashTable,
                               VaccineNameIndex **nameHashTable,
                               const char *userName, const char *vaccineName,
                               Date currentDate, int hashSize) {
  UserIndex *userEntry = findUserByName(userHashTable, userName, hashSize);
  if (userEntry == NULL)
    return 0;

  VaccineNameIndex *vaccineEntry =
      findVaccineByName(nameHashTable, vaccineName, hashSize);
  if (vaccineEntry == NULL)
    return 0;

  for (int i = 0; i < userEntry->inoculationCount; i++) {
    if (inoculationMatches(userEntry->inoculations[i], currentDate,
                           vaccineEntry)) {
      return 1;
    }
  }
  return 0;
}

/**
 * @brief Checks if a vaccine lot is valid and has available doses.
 *
 * @param lot The vaccine lot.
 * @param currentDate The current date.
 * @return int 1 if valid and available, 0 otherwise.
 */
static int isLotValidAndAvailable(const VaccineLot *lot, Date currentDate) {
  return !lot->isRemoved && lot->doses > lot->dosesUsed &&
         compareDates(lot->validation, currentDate) >= 0;
}

/**
 * @brief Finds the oldest valid lot from a list of lots.
 *
 * @param lots Array of vaccine lots.
 * @param lotCount Number of lots.
 * @param currentDate The current date.
 * @return VaccineLot* The oldest valid lot, or NULL if none found.
 */
static VaccineLot *findOldestValidLotFromList(VaccineLot **lots, int lotCount,
                                              Date currentDate) {
  VaccineLot *oldestValidLot = NULL;
  for (int i = 0; i < lotCount; i++) {
    VaccineLot *lot = lots[i];
    if (isLotValidAndAvailable(lot, currentDate)) {
      if (oldestValidLot == NULL || compareVaccines(lot, oldestValidLot) < 0) {
        oldestValidLot = lot;
      }
    }
  }
  return oldestValidLot;
}

/**
 * @brief Finds the oldest valid lot of a specific vaccine with available doses.
 *
 * @param nameHashTable The hash table of vaccine names.
 * @param vaccineName The vaccine name.
 * @param currentDate The current date.
 * @param hashSize The size of the hash table.
 * @return VaccineLot* The oldest valid lot, or NULL if none found.
 */
static VaccineLot *findOldestValidLot(VaccineNameIndex **nameHashTable,
                                      const char *vaccineName, Date currentDate,
                                      int hashSize) {
  VaccineNameIndex *vaccineEntry =
      findVaccineByName(nameHashTable, vaccineName, hashSize);
  if (vaccineEntry == NULL || vaccineEntry->lotCount == 0) {
    return NULL; // Vaccine not found
  }
  return findOldestValidLotFromList(vaccineEntry->lots, vaccineEntry->lotCount,
                                    currentDate);
}

/**
 * @brief Handles the case when arguments are invalid for command A.
 *
 * @param portuguese Flag indicating if output should be in Portuguese.
 */
static void handleInvalidArguments(int portuguese) {
  if (portuguese)
    printf("argumentos inválidos\n");
  else
    printf("invalid arguments\n");
}

/**
 * @brief Handles the case when the user is already vaccinated today.
 *
 * @param portuguese Flag indicating if output should be in Portuguese.
 */
static void handleAlreadyVaccinated(int portuguese) {
  if (portuguese)
    printf("já vacinado\n");
  else
    printf("already vaccinated\n");
}

/**
 * @brief Handles the case when there is no stock of the requested vaccine.
 *
 * @param portuguese Flag indicating if output should be in Portuguese.
 */
static void handleNoStock(int portuguese) {
  if (portuguese)
    printf("esgotado\n");
  else
    printf("no stock\n");
}

/**
 * @brief Handles memory allocation failure during command A.
 *
 * @param portuguese Flag indicating if output should be in Portuguese.
 */
static void handleMemoryError(int portuguese) {
  if (portuguese)
    printf("sem memória\n");
  else
    printf("No memory\n");
}

/**
 * @brief Applies the vaccine and updates the data structures.
 *
 * @param userName The name of the user.
 * @param lot The vaccine lot to use.
 * @param currentDate The current date.
 * @param userHashTable The hash table of user indices.
 * @param inoculationList Pointer to the list of inoculations.
 * @param portuguese Flag indicating if output should be in Portuguese.
 */
static void applyVaccine(const char *userName, VaccineLot *lot,
                         Date currentDate, UserIndex **userHashTable,
                         Inoculation **inoculationList, int portuguese) {
  Inoculation *newInoc = createInoculation(userName, lot->lot, currentDate);
  if (newInoc == NULL) {
    handleMemoryError(portuguese);
    return;
  }
  addInoculationToUserIndex(userHashTable, newInoc,
                            HASH_SIZE); // HASH_SIZE is a global constant
  newInoc->next_global = *inoculationList;
  *inoculationList = newInoc;
  lot->dosesUsed++;
  printf("%s\n", lot->lot);
}

/**
 * @brief Frees the memory allocated for userName and vaccineName.
 *
 * @param userName Pointer to the user name.
 * @param vaccineName Pointer to the vaccine name.
 */
static void freeCommandAResources(char *userName, char *vaccineName) {
  if (userName != NULL)
    free(userName);
  if (vaccineName != NULL)
    free(vaccineName);
}

/**
 * @brief Handles the vaccine application process.
 *
 * @param userName The name of the user.
 * @param vaccineName The name of the vaccine.
 * @param nameHashTable The hash table of vaccine names.
 * @param userHashTable The hash table of user indices.
 * @param inoculationList Pointer to the list of inoculations.
 * @param hashSize The size of the hash tables.
 * @param currentDate The current date.
 * @param portuguese Flag indicating if output should be in Portuguese.
 */
static void processVaccineApplication(char *userName, char *vaccineName,
                                      VaccineNameIndex **nameHashTable,
                                      UserIndex **userHashTable,
                                      Inoculation **inoculationList,
                                      int hashSize, Date currentDate,
                                      int portuguese) {
  if (isAlreadyVaccinated(userHashTable, nameHashTable, userName, vaccineName,
                          currentDate, hashSize)) {
    handleAlreadyVaccinated(portuguese);
    freeCommandAResources(userName, vaccineName);
    return;
  }

  VaccineLot *lot =
      findOldestValidLot(nameHashTable, vaccineName, currentDate, hashSize);

  if (lot == NULL) {
    handleNoStock(portuguese);
    freeCommandAResources(userName, vaccineName);
    return;
  }

  applyVaccine(userName, lot, currentDate, userHashTable, inoculationList,
               portuguese);
  freeCommandAResources(userName, vaccineName);
}

/**
 * @brief Command A: Applies a vaccine dose to a user.
 *
 * @param args The command arguments.
 * @param nameHashTable The hash table of vaccine names.
 * @param userHashTable The hash table of user indices.
 * @param inoculationList Pointer to the list of inoculations.
 * @param hashSize The size of the hash tables.
 * @param currentDate The current date.
 * @param portuguese Flag indicating if output should be in Portuguese.
 */
void commandA(char *args, VaccineNameIndex **nameHashTable,
              UserIndex **userHashTable, Inoculation **inoculationList,
              int hashSize, Date currentDate, int portuguese) {
  char *userName = NULL;
  char *vaccineName = NULL;

  // Extract user name and vaccine name from the arguments
  if (!extractArguments(args, &userName, &vaccineName)) {
    handleInvalidArguments(portuguese);
    return;
  }

  // Process the vaccine application
  processVaccineApplication(userName, vaccineName, nameHashTable, userHashTable,
                            inoculationList, hashSize, currentDate, portuguese);
}