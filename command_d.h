/**
 * @file command_d.h
 * @brief Header file for command D functionality to delete inoculation records.
 *
 * This file contains the declaration of the commandD function which handles the deletion
 * of inoculation records from the system.
 *
 * Author: Vicente B. Duarte
 */

 #ifndef COMMAND_D_H
 #define COMMAND_D_H
 
 #include "project.h"
 
 /**
  * @brief Structure to store the processed arguments for deletion.
  */
 typedef struct {
     char *userName;
     Date *date;
     char *lotId;
 } DeleteArgs;
 
 /**
  * @brief Deletes inoculation records based on the provided criteria.
  *
  * @param args The command arguments.
  * @param inoculationList The list of inoculations.
  * @param userHashTable The hash table of user indices.
  * @param hashTable The hash table of vaccine lots.
  * @param hashSize The size of the hash tables.
  * @param currentDate The current date.
  * @param portuguese Flag indicating if the output should be in Portuguese.
  */
 void commandD(char *args, Inoculation **inoculationList,
               UserIndex **userHashTable, VaccineLot **hashTable,
               int hashSize, Date currentDate, int portuguese);
 
 #endif