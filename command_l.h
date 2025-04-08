/**
 * @file command_l.h
 * @brief Header file for command L functionality to list vaccine batches.
 * 
 * This file contains the declaration of the commandL function which handles the listing
 * of vaccine batches in the system.
 * 
 * Author: Vicente B. Duarte
 */

#ifndef COMMAND_L_H
#define COMMAND_L_H

#include "project.h"

 /**
  * @brief Lists vaccine batches based on the provided criteria.
  * 
  * @param args The command arguments.
  * @param vaccineList The list of vaccine lots.
  * @param nameHashTable The hash table of vaccine names.
  * @param hashSize The size of the hash table.
  * @param portuguese Flag indicating if the output should be in Portuguese.
  */
void commandL(char *args, VaccineLot *vaccineList, VaccineNameIndex **nameHashTable,
              int hashSize, int portuguese);

#endif