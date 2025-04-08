/**
 * @file commands.c
 * @brief Implementation of command handling functionality.
 *
 * This file contains the implementation of the handleCommand function which
 * handles the execution of various commands in the system.
 *
 * Author: Vicente B. Duarte
 */

#include "command_a.h"
#include "command_c.h"
#include "command_d.h"
#include "command_l.h"
#include "command_r.h"
#include "command_t.h"
#include "command_u.h"
#include "constants.h"
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Executes the appropriate command based on the command character.
 *
 * @param cmd The command character.
 * @param args The command arguments.
 * @param hashTable The hash table of vaccine lots.
 * @param nameHashTable The hash table of vaccine names.
 * @param userHashTable The hash table of user indices.
 * @param vaccineList The list of vaccine lots.
 * @param hashSize The size of the hash tables.
 * @param vaccineCount The current count of vaccine lots.
 * @param maxVaccines The maximum number of vaccine lots allowed.
 * @param currentDate The current date.
 * @param inoculationList The list of inoculations.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
void handleCommand(char cmd, char *args, VaccineLot **hashTable,
                   VaccineNameIndex **nameHashTable, UserIndex **userHashTable,
                   VaccineLot **vaccineList, int hashSize, int *vaccineCount,
                   int maxVaccines, Date *currentDate,
                   Inoculation **inoculationList, int portuguese) {
  switch (cmd) {
  case 'c':
    commandC(args, hashTable, nameHashTable, vaccineList, hashSize,
             vaccineCount, maxVaccines, *currentDate, portuguese);
    break;
  case 'l':
    commandL(args, *vaccineList, nameHashTable, hashSize, portuguese);
    break;
  case 't':
    commandT(args, currentDate, portuguese);
    break;
  case 'a':
    commandA(args, nameHashTable, userHashTable, inoculationList, hashSize,
             *currentDate, portuguese);
    break;
  case 'u':
    commandU(args, *inoculationList, userHashTable, hashSize, portuguese);
    break;
  case 'r':
    commandR(args, hashTable, nameHashTable, vaccineList, hashSize, portuguese);
    break;
  case 'd':
    commandD(args, inoculationList, userHashTable, hashTable, hashSize,
             *currentDate, portuguese);
    break;
  default:
    break;
  }
}