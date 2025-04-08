/**
 * @file commands.h
 * @brief Header file for handling different commands in the system.
 * 
 * This file contains the declaration of the handleCommand function which handles
 * the execution of various commands in the system.
 * 
 * Author: Vicente B. Duarte
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include "project.h"

 /**
  * @brief Handles the execution of different commands.
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
                 VaccineLot **vaccineList, int hashSize,
                 int *vaccineCount, int maxVaccines, Date *currentDate,
                 Inoculation **inoculationList, int portuguese);

#endif