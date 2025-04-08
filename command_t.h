/**
 * @file command_t.h
 * @brief Header file for command T functionality to manage the simulation date.
 * 
 * This file contains the declaration of the commandT function which handles the
 * management of the simulation date in the system.
 * 
 * Author: Vicente B. Duarte
 */

#ifndef COMMAND_T_H
#define COMMAND_T_H

#include "project.h"

/**
 * @brief Advances the simulation time or prints the current date.
 * 
 * @param args The command arguments.
 * @param currentDate The current date.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
void commandT(char *args, Date *currentDate, int portuguese);

#endif