/**
 * @file project.c
 * @brief Main file for the project.
 *
 * This file contains the main function which initializes the system, processes
 * commands, and manages the overall flow of the program.
 *
 * Author: Vicente B. Duarte
 */

#include "project.h"
#include "commands.h"
#include "constants.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initializes the data structures used in the program.
 *
 * @param hashTable Pointer to the vaccine hash table.
 * @param nameHashTable Pointer to the vaccine name hash table.
 * @param userHashTable Pointer to the user hash table.
 * @param vaccineList Pointer to the global vaccine list.
 * @param inoculationList Pointer to the global inoculation list.
 * @param vaccineCount Pointer to the vaccine counter.
 */
void initializeDataStructures(VaccineLot ***hashTable,
                              VaccineNameIndex ***nameHashTable,
                              UserIndex ***userHashTable,
                              VaccineLot **vaccineList,
                              Inoculation **inoculationList,
                              int *vaccineCount) {
  *hashTable = initializeVaccineHashTable(HASH_SIZE);
  *nameHashTable = initializeVaccineNameHashTable(HASH_SIZE);
  *userHashTable = initializeUserHashTable(HASH_SIZE);
  *vaccineList = NULL;
  *inoculationList = NULL;
  *vaccineCount = 0;
}

/**
 * @brief Processes user commands from standard input.
 *
 * @param command Buffer to store the command.
 * @param hashTable Vaccine hash table.
 * @param nameHashTable Vaccine name hash table.
 * @param userHashTable User hash table.
 * @param vaccineList Pointer to the global vaccine list.
 * @param vaccineCount Pointer to the vaccine counter.
 * @param inoculationList Pointer to the global inoculation list.
 * @param currentDate Pointer to the current date.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
void processCommands(char *command, VaccineLot **hashTable,
                     VaccineNameIndex **nameHashTable,
                     UserIndex **userHashTable, VaccineLot **vaccineList,
                     int *vaccineCount, Inoculation **inoculationList,
                     Date *currentDate, int portuguese) {
  while (fgets(command, SIZE_COMMAND, stdin)) {
    command[strcspn(command, "\n")] = 0; // Remove the newline character

    if (strlen(command) == 0) // Skip empty commands
      continue;

    char cmd = command[0];
    if (cmd == 'q') // Exit if the command is 'q'
      break;

    char *args = command + 1;
    while (*args != '\0' && isspace(*args)) // Skip whitespace
      args++;

    handleCommand(cmd, args, hashTable, nameHashTable, userHashTable,
                  vaccineList, HASH_SIZE, vaccineCount, MAX_VACCINES,
                  currentDate, inoculationList, portuguese);
  }
}

/**
 * @brief Frees all allocated resources before exiting the program.
 *
 * @param hashTable Vaccine hash table.
 * @param nameHashTable Vaccine name hash table.
 * @param userHashTable User hash table.
 * @param inoculationList Global inoculation list.
 * @param command Command buffer.
 */
void freeResources(VaccineLot **hashTable, VaccineNameIndex **nameHashTable,
                   UserIndex **userHashTable, Inoculation *inoculationList,
                   char *command) {
  free(command);
  freeVaccineHashTable(hashTable, HASH_SIZE);
  freeVaccineNameHashTable(nameHashTable, HASH_SIZE);
  freeUserHashTable(userHashTable, HASH_SIZE);
  freeInoculationList(inoculationList);
}

/**
 * @brief Main function of the program.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return int Exit status.
 */
int main(int argc, char *argv[]) {
  // Check if the program should use Portuguese
  int portuguese = (argc > 1 && strcmp(argv[1], "pt") == 0) ? 1 : 0;

  // Initialize current date
  Date currentDate = {1, 1, 2025}; // Initial date: 01-01-2025

  // Declare data structures
  VaccineLot **hashTable;
  VaccineNameIndex **nameHashTable;
  UserIndex **userHashTable;
  VaccineLot *vaccineList;
  Inoculation *inoculationList;
  int vaccineCount;

  // Initialize data structures
  initializeDataStructures(&hashTable, &nameHashTable, &userHashTable,
                           &vaccineList, &inoculationList, &vaccineCount);

  // Allocate memory for command
  char *command = (char *)malloc(SIZE_COMMAND);
  if (command == NULL) {
    printf(portuguese ? "sem memória\n" : "No memory\n");
    return 1;
  }

  // Process user commands
  processCommands(command, hashTable, nameHashTable, userHashTable,
                  &vaccineList, &vaccineCount, &inoculationList, &currentDate,
                  portuguese);

  // Free resources and exit
  freeResources(hashTable, nameHashTable, userHashTable, inoculationList,
                command);

  return 0;
}