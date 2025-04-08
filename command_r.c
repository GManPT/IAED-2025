/**
 * @file command_r.c
 * @brief Implementation of command R functionality to remove vaccine lot
 * availability.
 *
 * This file contains the implementation of the commandR function and its helper
 * functions. It handles finding the specified vaccine lot, checking its usage,
 * and either removing it or updating its availability.
 *
 * Author: Vicente B. Duarte
 */

#include "constants.h"
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Removes a vaccine lot from the linked list.
 *
 * @param vaccineList The list of vaccine lots.
 * @param batch The batch identifier.
 */
void removeVaccineFromList(VaccineLot **vaccineList, const char *batch) {
  if (*vaccineList == NULL)
    return;

  // If the batch to remove is the first in the list
  if (strcmp((*vaccineList)->lot, batch) == 0) {
    *vaccineList = (*vaccineList)->next_vaccine;
    // Memory is not freed here as the lot might still be in the hash table
    return;
  }

  // Search for the batch in the rest of the list
  VaccineLot *current = *vaccineList;
  while (current->next_vaccine != NULL) {
    if (strcmp(current->next_vaccine->lot, batch) == 0) {
      VaccineLot *temp = current->next_vaccine;
      current->next_vaccine = temp->next_vaccine;
      // Memory is not freed here
      return;
    }
    current = current->next_vaccine;
  }
}

/**
 * @brief Removes a vaccine lot from the hash table.
 *
 * @param hashTable The hash table of vaccine lots.
 * @param batch The batch identifier.
 * @param size The size of the hash table.
 */
void removeVaccineFromHash(VaccineLot **hashTable, const char *batch,
                           int size) {
  unsigned int index = hashBatch(batch, size);

  // If the list at this index is empty
  if (hashTable[index] == NULL)
    return;

  // If the batch to remove is the first in the list at this index
  if (strcmp(hashTable[index]->lot, batch) == 0) {
    VaccineLot *temp = hashTable[index];
    hashTable[index] = hashTable[index]->next_hash;
    freeVaccineLot(temp);
    return;
  }

  // Search for the batch in the rest of the list at this index
  VaccineLot *current = hashTable[index];
  while (current->next_hash != NULL) {
    if (strcmp(current->next_hash->lot, batch) == 0) {
      VaccineLot *temp = current->next_hash;
      current->next_hash = temp->next_hash;
      freeVaccineLot(temp);
      return;
    }
    current = current->next_hash;
  }
}

/**
 * @brief Removes a vaccine lot from the name index.
 *
 * @param nameHashTable The hash table of vaccine names.
 * @param batch The batch identifier.
 * @param vaccineName The name of the vaccine.
 * @param hashSize The size of the hash table.
 */
static void removeVaccineFromNameIndex(VaccineNameIndex **nameHashTable,
                                       const char *batch,
                                       const char *vaccineName, int hashSize) {
  VaccineNameIndex *nameEntry =
      findVaccineByName(nameHashTable, vaccineName, hashSize);
  if (nameEntry != NULL) {
    for (int i = 0; i < nameEntry->lotCount; i++) {
      if (strcmp(nameEntry->lots[i]->lot, batch) == 0) {
        // Replace this slot with the last lot in the array and decrease the
        // count
        nameEntry->lots[i] = nameEntry->lots[nameEntry->lotCount - 1];
        nameEntry->lotCount--;
        break;
      }
    }
  }
}

/**
 * @brief Handles the case where the vaccine lot has no doses used, performing
 * full removal.
 *
 * @param batch The batch identifier.
 * @param hashTable The hash table of vaccine lots.
 * @param nameHashTable The hash table of vaccine names.
 * @param vaccineList The list of vaccine lots.
 * @param hashSize The size of the hash table.
 */
static void handleUnusedVaccineLot(const char *batch, VaccineLot **hashTable,
                                   VaccineNameIndex **nameHashTable,
                                   VaccineLot **vaccineList, int hashSize) {
  VaccineLot *lot = findVaccineByBatch(hashTable, batch, hashSize);
  if (lot != NULL) {
    removeVaccineFromNameIndex(nameHashTable, batch, lot->name, hashSize);
    removeVaccineFromList(vaccineList, batch);
    removeVaccineFromHash(hashTable, batch, hashSize);
  }
}

/**
 * @brief Handles the case where the vaccine lot has doses used, marking it as
 * removed.
 *
 * @param lot The vaccine lot to mark as removed.
 */
static void handleUsedVaccineLot(VaccineLot *lot) {
  lot->isRemoved = 1;
  lot->doses = lot->dosesUsed; // Ensure no more doses can be used
}

/**
 * @brief Command R: Removes the availability of a vaccine lot.
 *
 * @param args The command arguments (the batch identifier).
 * @param hashTable The hash table of vaccine lots.
 * @param nameHashTable The hash table of vaccine names.
 * @param vaccineList The list of vaccine lots.
 * @param hashSize The size of the hash table.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
void commandR(char *args, VaccineLot **hashTable,
              VaccineNameIndex **nameHashTable, VaccineLot **vaccineList,
              int hashSize, int portuguese) {
  // Check if a batch argument is provided
  if (args == NULL || *args == '\0') {
    if (portuguese)
      printf("lote em falta\n");
    else
      printf("missing batch\n");
    return;
  }

  // Find the vaccine lot by its batch identifier
  VaccineLot *lot = findVaccineByBatch(hashTable, args, hashSize);

  // Check if the vaccine lot exists
  if (lot == NULL) {
    if (portuguese)
      printf("%s: lote inexistente\n", args);
    else
      printf("%s: no such batch\n", args);
    return;
  }

  // Print the number of doses already used for this lot
  printf("%d\n", lot->dosesUsed);

  // If no doses have been used, remove the lot completely from all data
  // structures
  if (lot->dosesUsed == 0) {
    handleUnusedVaccineLot(args, hashTable, nameHashTable, vaccineList,
                           hashSize);
  } else {
    // If doses have been used, mark the lot as removed and ensure no more doses
    // are available
    handleUsedVaccineLot(lot);
  }
}