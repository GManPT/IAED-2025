/**
 * @file command_u.h
 * @brief Header file for command U functionality to list inoculations.
 * 
 * This file contains the declaration of the commandU function which handles the listing
 * of inoculations in the system.
 * 
 * Author: Vicente B. Duarte
 */

#ifndef COMMAND_U_H
#define COMMAND_U_H

#include "project.h"

/**
 * @brief Lists all inoculations or inoculations for a specific user.
 * 
 * @param args The command arguments.
 * @param inoculationList The list of inoculations.
 * @param userHashTable The hash table of user indices.
 * @param hashSize The size of the hash tables.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
void commandU(char* args, Inoculation* inoculationList, UserIndex** userHashTable,
              int hashSize, int portuguese);

#endif