/**
 * @file command_r.h
 * @brief Header file for command R functionality to remove vaccine lot availability.
 * 
 * This file contains the declaration of the commandR function which handles the removal
 * of vaccine lot availability in the system.
 * 
 * Author: Vicente B. Duarte
 */

#ifndef COMMAND_R_H
#define COMMAND_R_H

#include "project.h"

/**
 * @brief Removes the availability of a vaccine lot.
 * 
 * @param args The command arguments.
 * @param hashTable The hash table of vaccine lots.
 * @param nameHashTable The hash table of vaccine names.
 * @param vaccineList The list of vaccine lots.
 * @param hashSize The size of the hash table.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
void commandR(char *args, VaccineLot **hashTable, VaccineNameIndex **nameHashTable,
              VaccineLot **vaccineList, int hashSize, int portuguese);

#endif