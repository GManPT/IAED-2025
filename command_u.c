/**
 * @file command_u.c
 * @brief Implementation of command U functionality to list inoculations.
 *
 * This file contains the implementation of the commandU function and its helper
 * functions. It handles parsing the command arguments, checking if the user has
 * inoculations, and printing the inoculations in chronological order.
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
 * @brief Prints an inoculation record.
 *
 * @param inoc The inoculation record to print.
 */
static void printInoculation(Inoculation *inoc) {
  printf("%s %s %02d-%02d-%d\n", inoc->user, inoc->lot, inoc->date.day,
         inoc->date.month, inoc->date.year);
}

/**
 * @brief Extracts the user name from a command argument string.
 *
 * @param args The command arguments.
 * @param buffer Buffer to store the extracted name.
 * @param bufSize Size of the buffer.
 * @return int 1 if extraction was successful, 0 otherwise.
 */
static int extractUserName(const char *args, char *buffer, size_t bufSize) {
  if (args == NULL || *args == '\0') {
    return 0; // No user specified
  }

  // Check if the name is enclosed in quotes
  if (args[0] == '"') {
    const char *endQuote = strchr(args + 1, '"');
    if (endQuote == NULL) {
      return 0; // Unclosed quotes
    }

    size_t nameLen = endQuote - args - 1;
    if (nameLen >= bufSize) {
      return 0; // Name too long for buffer
    }

    memcpy(buffer, args + 1, nameLen);
    buffer[nameLen] = '\0';
  } else {
    // Handle names without quotes
    size_t argLen = strlen(args);
    if (argLen >= bufSize) {
      return 0; // Name too long for buffer
    }
    strcpy(buffer, args);
  }

  return 1; // Extraction successful
}

/**
 * @brief Lists all inoculations in chronological order.
 *
 * @param inoculationList The head of the global inoculation list.
 */
static void listAllInoculations(Inoculation *inoculationList) {
  if (inoculationList == NULL) {
    return; // No inoculations to list
  }

  // Count the total number of inoculations
  int count = 0;
  Inoculation *current = inoculationList;
  while (current != NULL) {
    count++;
    current = current->next_global;
  }

  if (count == 0)
    return;

  // Create an array to store inoculation pointers for sorting
  Inoculation **inocArray =
      (Inoculation **)malloc(count * sizeof(Inoculation *));
  if (inocArray == NULL) {
    printf("No memory\n");
    exit(1);
  }

  // Fill the array with inoculations in reverse order to maintain chronological
  // order
  current = inoculationList;
  int index = count - 1; // Start from the end of the array
  while (current != NULL) {
    inocArray[index--] = current; // Fill from back to front
    current = current->next_global;
  }

  // Print inoculations in chronological order (oldest first)
  for (int i = 0; i < count; i++) {
    printInoculation(inocArray[i]);
  }

  free(inocArray);
}

/**
 * @brief Lists all inoculations for a specific user in chronological order.
 *
 * @param userName The name of the user.
 * @param userHashTable The hash table of user indices.
 * @param hashSize The size of the hash table.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
static void listInoculationsByUser(const char *userName,
                                   UserIndex **userHashTable, int hashSize,
                                   int portuguese) {
  // Find the user entry in the hash table
  UserIndex *userEntry = findUserByName(userHashTable, userName, hashSize);

  // If the user does not exist or has no inoculations
  if (userEntry == NULL || userEntry->inoculationCount == 0) {
    if (portuguese) {
      printf("%s: utente inexistente\n", userName);
    } else {
      printf("%s: no such user\n", userName);
    }
    return;
  }

  // Get the count of user's inoculations
  int count = userEntry->inoculationCount;

  // Print inoculations in the order they appear in the user's index
  // (chronological)
  for (int i = 0; i < count; i++) {
    printInoculation(userEntry->inoculations[i]);
  }
}

/**
 * @brief Lists all inoculations or inoculations for a specific user.
 *
 * @param args The command arguments. If empty, lists all inoculations.
 * Otherwise, it should contain the user name (optionally enclosed in quotes).
 * @param inoculationList The head of the global inoculation list.
 * @param userHashTable The hash table of user indices.
 * @param hashSize The size of the hash tables.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
void commandU(char *args, Inoculation *inoculationList,
              UserIndex **userHashTable, int hashSize, int portuguese) {
  char userNameBuffer[SIZE_COMMAND];
  int hasUserName =
      extractUserName(args, userNameBuffer, sizeof(userNameBuffer));

  // If no user name is provided, list all inoculations
  if (!hasUserName) {
    listAllInoculations(inoculationList);
  } else {
    // If a user name is provided, list inoculations for that user
    listInoculationsByUser(userNameBuffer, userHashTable, hashSize, portuguese);
  }
}