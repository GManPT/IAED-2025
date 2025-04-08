/**
 * @file command_a.h
 * @brief Header file for command A functionality for applying vaccine doses to users.
 *
 * This file contains the declaration of the `commandA` function, which handles the
 * application of vaccine doses to users, updating the system's data structures accordingly.
 *
 * Author: Vicente B. Duarte
 */

 #ifndef COMMAND_A_H
 #define COMMAND_A_H
 
 #include "project.h"
 
 /**
  * @brief Applies a vaccine dose to a user.
  * 
  * This function processes the application of a vaccine dose to a user, ensuring
  * that all necessary validations are performed and the system's data structures
  * are updated correctly.
  * 
  * @param args The command arguments.
  * @param nameHashTable The hash table of vaccine names.
  * @param userHashTable The hash table of user indices.
  * @param inoculationList Pointer to the list of inoculations.
  * @param hashSize The size of the hash tables.
  * @param currentDate The current date.
  * @param portuguese Flag indicating if output should be in Portuguese.
  */
 void commandA(char* args, VaccineNameIndex** nameHashTable, 
               UserIndex** userHashTable, Inoculation** inoculationList, 
               int hashSize, Date currentDate, int portuguese);
 
 #endif