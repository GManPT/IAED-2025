/**
 * @file command_d.c
 * @brief Implementation of command D functionality to delete inoculation
 * records.
 *
 * This file contains the implementation of the commandD function and its helper
 * functions. It handles parsing the command arguments, validating the input,
 * and deleting the specified inoculation records from the system.
 *
 * Author: Vicente B. Duarte
 */

#include "command_d.h"
#include "constants.h"
#include "project.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Frees the memory allocated for the arguments.
 *
 * @param args The processed arguments.
 */
static void freeDeleteArgs(DeleteArgs *args);

/**
 * @brief Checks if a date has a valid format.
 *
 * @param date The date to check.
 * @return int 1 if valid, 0 otherwise.
 */
static int isDateFormatValid(Date date) {
  // Checks if the month is within the valid range (1-12)
  if (date.month < 1 || date.month > 12) {
    return 0;
  }

  // Array to store the number of days in each month (0-indexed for convenience)
  int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Adjust the number of days in February for leap years
  if (date.month == 2 &&
      (date.year % 4 == 0 && (date.year % 100 != 0 || date.year % 400 == 0))) {
    daysInMonth[2] = 29;
  }

  // Checks if the day is within the valid range for the given month
  return (date.day >= 1 && date.day <= daysInMonth[date.month]);
}

/**
 * @brief Checks if a date is in the future relative to the current date.
 *
 * @param date The date to check.
 * @param currentDate The current date.
 * @return int 1 if the date is in the future, 0 otherwise.
 */
static int isDateFuture(Date date, Date currentDate) {
  return (date.year > currentDate.year) ||
         (date.year == currentDate.year && date.month > currentDate.month) ||
         (date.year == currentDate.year && date.month == currentDate.month &&
          date.day > currentDate.day);
}

/**
 * @brief Parses a date string in DD-MM-YYYY format and validates it.
 *
 * @param dateStr The date string to parse.
 * @param currentDate The current date for comparison.
 * @param valid Pointer to an integer flag to indicate if parsing and validation
 * were successful.
 * @param portuguese Flag indicating if error messages should be in Portuguese.
 * @return Date* A pointer to the parsed Date structure, or NULL if parsing
 * fails or the date is invalid.
 */
static Date *parseDateString(const char *dateStr, Date currentDate, int *valid,
                             int portuguese) {
  if (!dateStr) {
    return NULL;
  }
  Date *date = (Date *)malloc(sizeof(Date));
  if (!date) {
    return NULL;
  }

  // Attempt to parse the date string using sscanf
  if (sscanf(dateStr, "%d-%d-%d", &date->day, &date->month, &date->year) != 3) {
    free(date);
    *valid = 0;
    if (portuguese)
      printf("data inválida\n");
    else
      printf("invalid date\n");
    return NULL;
  }

  // Validate the parsed date format and ensure it's not in the future
  if (!isDateFormatValid(*date) || isDateFuture(*date, currentDate)) {
    free(date);
    *valid = 0;
    if (portuguese)
      printf("data inválida\n");
    else
      printf("invalid date\n");
    return NULL;
  }
  return date;
}

/**
 * @brief Parses the user name from the command arguments. Handles quoted and
 * unquoted names.
 *
 * @param args A working copy of the command arguments string.
 * @param deleteArgs A pointer to the DeleteArgs structure to store the parsed
 * user name.
 * @return char* A pointer to the rest of the arguments string after the user
 * name, or NULL on error.
 */
static char *parseUserName(char *args, DeleteArgs *deleteArgs) {
  char *ptr = args;
  // Skip leading whitespace
  while (*ptr && isspace(*ptr))
    ptr++;
  char *username_start;
  char *username_end;
  // Handle quoted user name
  if (*ptr == '"') {
    username_start = ptr + 1;
    username_end = strchr(username_start, '"');
    if (!username_end)
      return NULL;          // Unclosed quote
    *username_end = '\0';   // Terminate the user name
    ptr = username_end + 1; // Move past the closing quote
  } else {
    // Handle unquoted user name
    username_start = ptr;
    while (*ptr && !isspace(*ptr))
      ptr++;
    if (*ptr)
      *ptr++ = '\0'; // Terminate the user name
    username_end = ptr;
  }

  // Allocate memory and copy the extracted user name
  deleteArgs->userName = strdup(username_start);
  return ptr;
}

/**
 * @brief Skips leading whitespace in the input string.
 *
 * @param ptr Pointer to the input string.
 * @return char* Pointer to the first non-whitespace character.
 */
static char *skipWhitespace(char *ptr) {
  while (*ptr && isspace(*ptr))
    ptr++;
  return ptr;
}

/**
 * @brief Extracts the date string from the input and validates its length.
 *
 * @param ptr Pointer to the input string.
 * @param date_str Buffer to store the extracted date string.
 * @param valid Pointer to the validity flag.
 * @return char* Pointer to the remaining input after the date string.
 */
static char *extractDateString(char *ptr, char *date_str, int *valid) {
  char *date_start = ptr;
  while (*ptr && !isspace(*ptr))
    ptr++;
  size_t date_len = ptr - date_start;
  if (date_len >= 32) { // Ensure the date string fits in the buffer
    *valid = 0;
    return NULL;
  }
  memcpy(date_str, date_start, date_len);
  date_str[date_len] = '\0';
  return ptr;
}

/**
 * @brief Extracts the lot ID from the input string.
 *
 * @param ptr Pointer to the input string.
 * @param deleteArgs Pointer to the DeleteArgs structure to store the lot ID.
 * @param valid Pointer to the validity flag.
 */
static void extractLotId(char *ptr, DeleteArgs *deleteArgs, int *valid) {
  if (*ptr) {
    deleteArgs->lotId = strdup(ptr);
    if (!deleteArgs->lotId)
      *valid = 0; // Memory allocation failure
  }
}

/**
 * @brief Parses the date and lot ID from the remaining command arguments.
 *
 * @param ptr A pointer to the remaining part of the arguments string after the
 * user name.
 * @param deleteArgs A pointer to the DeleteArgs structure to store the parsed
 * date and lot ID.
 * @param currentDate The current date for date validation.
 * @param valid Pointer to an integer flag to indicate validity.
 * @param portuguese Flag indicating if error messages should be in Portuguese.
 */
static void parseDateAndLotId(char *ptr, DeleteArgs *deleteArgs,
                              Date currentDate, int *valid, int portuguese) {
  // Skip leading whitespace
  ptr = skipWhitespace(ptr);

  // Check if there is a date argument
  if (*ptr) {
    char date_str[32]; // Buffer to hold the date string
    ptr = extractDateString(ptr, date_str, valid);
    if (!*valid || !ptr)
      return;

    // Parse the date string
    deleteArgs->date =
        parseDateString(date_str, currentDate, valid, portuguese);
    if (!*valid || !deleteArgs->date)
      return;

    // Skip whitespace to find the lot ID
    ptr = skipWhitespace(ptr);

    // Extract the lot ID
    extractLotId(ptr, deleteArgs, valid);
  }
}

/**
 * @brief Processes the command arguments to extract user name, date, and lot
 * ID.
 *
 * @param args The command arguments string.
 * @param currentDate The current date for validation.
 * @param valid Pointer to an integer flag to indicate if the arguments are
 * valid.
 * @param portuguese Flag indicating if error messages should be in Portuguese.
 * @return DeleteArgs* A pointer to the structure containing the processed
 * arguments, or NULL if invalid.
 */
static DeleteArgs *processDeleteArgs(char *args, Date currentDate, int *valid,
                                     int portuguese) {
  *valid = 1;
  DeleteArgs *deleteArgs = (DeleteArgs *)malloc(sizeof(DeleteArgs));
  if (!deleteArgs)
    return NULL;
  deleteArgs->userName = NULL;
  deleteArgs->date = NULL;
  deleteArgs->lotId = NULL;
  // Create a working copy of the arguments to avoid modifying the original
  char *args_copy = strdup(args);
  if (!args_copy) {
    free(deleteArgs);
    return NULL;
  }
  // Parse the user name
  char *ptr = parseUserName(args_copy, deleteArgs);
  if (!deleteArgs->userName || !ptr) {
    free(args_copy);
    freeDeleteArgs(deleteArgs);
    *valid = 0;
    return NULL;
  }
  // Parse the date and lot ID
  parseDateAndLotId(ptr, deleteArgs, currentDate, valid, portuguese);
  free(args_copy);
  // If any parsing or validation failed, free allocated memory and return NULL
  if (!*valid) {
    freeDeleteArgs(deleteArgs);
    return NULL;
  }
  return deleteArgs;
}

/**
 * @brief Compares two Date structures for equality.
 *
 * @param a The first date.
 * @param b The second date.
 * @return int 1 if the dates are equal, 0 otherwise.
 */
static int datesEqual(Date a, Date b) {
  return (a.day == b.day && a.month == b.month && a.year == b.year);
}

/**
 * @brief Checks if an inoculation record matches the provided deletion
 * criteria.
 *
 * @param inoc A pointer to the Inoculation record.
 * @param args A pointer to the DeleteArgs structure containing the deletion
 * criteria.
 * @return int 1 if the inoculation matches the criteria, 0 otherwise.
 */
static int inoculationMatchesCriteria(const Inoculation *inoc,
                                      const DeleteArgs *args) {
  // Check if the user names match
  if (strcmp(inoc->user, args->userName) != 0)
    return 0;
  // If no date is provided in the arguments, any inoculation for the user
  // matches
  if (!args->date)
    return 1;
  // Check if the dates match
  if (!datesEqual(inoc->date, *(args->date)))
    return 0;
  // If no lot ID is provided, or if it is provided and matches the
  // inoculation's lot ID
  return (!args->lotId || (args->lotId && strcmp(inoc->lot, args->lotId) == 0));
}

/**
 * @brief Removes an inoculation record from the user's index.
 *
 * @param userEntry Pointer to the user's index.
 * @param toRemove Pointer to the inoculation record to remove.
 */
static void removeInoculationFromUser(UserIndex *userEntry,
                                      Inoculation *toRemove) {
  for (int i = 0; i < userEntry->inoculationCount; i++) {
    if (userEntry->inoculations[i] == toRemove) {
      // Replace the removed element with the last element and decrement the
      // count
      userEntry->inoculations[i] =
          userEntry->inoculations[userEntry->inoculationCount - 1];
      userEntry->inoculationCount--;
      break;
    }
  }
}

/**
 * @brief Removes an inoculation record from the global list.
 *
 * @param inoculationList Pointer to the head of the global inoculation list.
 * @param prev Pointer to the previous inoculation in the list.
 * @param curr Pointer to the current inoculation in the list.
 * @return Inoculation* Pointer to the next inoculation in the list.
 */
static Inoculation *
removeInoculationFromGlobalList(Inoculation **inoculationList,
                                Inoculation *prev, Inoculation *curr) {
  if (prev)
    prev->next_global = curr->next_global;
  else
    *inoculationList = curr->next_global;

  Inoculation *next = curr->next_global;
  freeInoculation(curr);
  return next;
}

/**
 * @brief Removes inoculation records that match the deletion criteria.
 *
 * @param inoculationList A pointer to the head of the global inoculation list.
 * @param userHashTable The hash table of user indices.
 * @param args A pointer to the DeleteArgs structure containing the deletion
 * criteria.
 * @param hashSize The size of the hash table.
 * @return int The number of inoculation records that were removed.
 */
static int removeMatchingInoculations(Inoculation **inoculationList,
                                      UserIndex **userHashTable,
                                      DeleteArgs *args, int hashSize) {
  int count = 0;
  Inoculation *curr = *inoculationList;
  Inoculation *prev = NULL;

  // Find the user entry in the hash table
  UserIndex *userEntry =
      findUserByName(userHashTable, args->userName, hashSize);
  if (!userEntry)
    return 0; // User not found (shouldn't happen at this point)

  // Iterate through the global inoculation list
  while (curr) {
    if (inoculationMatchesCriteria(curr, args)) {
      count++;
      removeInoculationFromUser(userEntry, curr);
      curr = removeInoculationFromGlobalList(inoculationList, prev, curr);
    } else {
      prev = curr;
      curr = curr->next_global;
    }
  }

  return count;
}

/**
 * @brief Frees the memory allocated for the DeleteArgs structure.
 *
 * @param args A pointer to the DeleteArgs structure.
 */
static void freeDeleteArgs(DeleteArgs *args) {
  if (args) {
    free(args->userName);
    free(args->date);
    free(args->lotId);
    free(args);
  }
}

/**
 * @brief Validates the user existence and inoculation count.
 *
 * @param userEntry Pointer to the user's index.
 * @param deleteArgs Pointer to the DeleteArgs structure.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 * @return int 1 if valid, 0 otherwise.
 */
static int validateUser(UserIndex *userEntry, DeleteArgs *deleteArgs,
                        int portuguese) {
  if (!userEntry || userEntry->inoculationCount == 0) {
    if (portuguese)
      printf("%s: utente inexistente\n", deleteArgs->userName);
    else
      printf("%s: no such user\n", deleteArgs->userName);
    return 0;
  }
  return 1;
}

/**
 * @brief Validates the vaccine lot existence.
 *
 * @param deleteArgs Pointer to the DeleteArgs structure.
 * @param hashTable The hash table of vaccine lots.
 * @param hashSize The size of the hash table.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 * @return int 1 if valid, 0 otherwise.
 */
static int validateLot(DeleteArgs *deleteArgs, VaccineLot **hashTable,
                       int hashSize, int portuguese) {
  if (deleteArgs->lotId != NULL &&
      !findVaccineByBatch(hashTable, deleteArgs->lotId, hashSize)) {
    if (portuguese)
      printf("%s: lote inexistente\n", deleteArgs->lotId);
    else
      printf("%s: no such batch\n", deleteArgs->lotId);
    return 0;
  }
  return 1;
}

/**
 * @brief Command D: Deletes inoculation records based on the provided
 * arguments.
 *
 * @param args The command arguments string.
 * @param inoculationList A pointer to the head of the global inoculation list.
 * @param userHashTable The hash table of user indices.
 * @param hashTable The hash table of vaccine lots.
 * @param hashSize The size of the hash tables.
 * @param currentDate The current date.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
void commandD(char *args, Inoculation **inoculationList,
              UserIndex **userHashTable, VaccineLot **hashTable, int hashSize,
              Date currentDate, int portuguese) {
  int valid = 1;

  // Process the command arguments
  DeleteArgs *deleteArgs =
      processDeleteArgs(args, currentDate, &valid, portuguese);
  if (!valid || !deleteArgs)
    return;

  // Validate user existence
  UserIndex *userEntry =
      findUserByName(userHashTable, deleteArgs->userName, hashSize);
  if (!validateUser(userEntry, deleteArgs, portuguese)) {
    freeDeleteArgs(deleteArgs);
    return;
  }

  // Validate vaccine lot existence
  if (!validateLot(deleteArgs, hashTable, hashSize, portuguese)) {
    freeDeleteArgs(deleteArgs);
    return;
  }

  // Remove the inoculations that match the criteria
  int removed = removeMatchingInoculations(inoculationList, userHashTable,
                                           deleteArgs, hashSize);

  // Print the number of removed records
  printf("%d\n", removed);

  // Free the memory allocated for the arguments
  freeDeleteArgs(deleteArgs);
}