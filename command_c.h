/**
 * @file command_c.h
 * @brief Header file for command C functionality to add new vaccine batches to the system.
 * 
 * This file contains the declaration of the commandC function which handles the addition
 * of new vaccine batches to the system.
 * 
 * Author: Vicente B. Duarte
 */

#ifndef COMMAND_C_H
#define COMMAND_C_H

#include "project.h"

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
void commandC(char *args, VaccineLot **hashTable, VaccineNameIndex **nameHashTable,
              VaccineLot **vaccineList, int hashSize, int *vaccineCount,
              int maxVaccines, Date currentDate, int portuguese);

#endif