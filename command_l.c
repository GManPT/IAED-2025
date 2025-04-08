/**
 * @file command_l.c
 * @brief Implementation of command L functionality to list vaccine batches.
 *
 * This file contains the implementation of the commandL function and its helper
 * functions. It handles listing all vaccine lots or specific lots based on the
 * provided arguments. The listing can be sorted by validation date and lot ID.
 *
 * Author: Vicente B. Duarte
 */

#include "constants.h"
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Prints the details of a single vaccine lot.
 *
 * @param vaccine The vaccine lot to print.
 */
static void printVaccine(VaccineLot *vaccine) {
  printf("%s %s %02d-%02d-%d %d %d\n", vaccine->name, vaccine->lot,
         vaccine->validation.day, vaccine->validation.month,
         vaccine->validation.year, vaccine->doses - vaccine->dosesUsed,
         vaccine->dosesUsed);
}

/**
 * @brief Compares two vaccines by validation date and then lot ID.
 *
 * @param vaccineA First vaccine lot.
 * @param vaccineB Second vaccine lot.
 * @return int Negative if A < B, zero if A == B, positive if A > B.
 */
int compareVaccines(VaccineLot *vaccineA, VaccineLot *vaccineB) {
  // Compare years of the validation date
  if (vaccineA->validation.year != vaccineB->validation.year)
    return vaccineA->validation.year - vaccineB->validation.year;

  // Compare months of the validation date
  if (vaccineA->validation.month != vaccineB->validation.month)
    return vaccineA->validation.month - vaccineB->validation.month;

  // Compare days of the validation date
  if (vaccineA->validation.day != vaccineB->validation.day)
    return vaccineA->validation.day - vaccineB->validation.day;

  // If validation dates are equal, compare by lot ID alphabetically
  return strcmp(vaccineA->lot, vaccineB->lot);
}

/**
 * @brief Swaps two VaccineLot pointers in an array.
 *
 * @param array The array of VaccineLot pointers.
 * @param a Index of the first element.
 * @param b Index of the second element.
 */
static void swapVaccines(VaccineLot **array, int a, int b) {
  VaccineLot *temp = array[a];
  array[a] = array[b];
  array[b] = temp;
}

/**
 * @brief Partition function for the quicksort algorithm.
 *
 * @param array The array to partition.
 * @param low The lower index of the partition.
 * @param high The higher index of the partition.
 * @return int The index of the pivot element after partitioning.
 */
static int partition(VaccineLot **array, int low, int high) {
  VaccineLot *pivot = array[high]; // Choose the last element as the pivot
  int i = low - 1;                 // Index of smaller element

  for (int j = low; j <= high - 1; j++) {
    // If the current element is smaller than or equal to the pivot
    if (compareVaccines(array[j], pivot) <= 0) {
      i++;                       // Increment index of smaller element
      swapVaccines(array, i, j); // Swap array[i] and array[j]
    }
  }
  swapVaccines(array, i + 1, high); // Swap array[i+1] and array[high] (pivot)
  return (i + 1);
}

/**
 * @brief Insertion sort algorithm for sorting small subarrays.
 *
 * @param array The array to sort.
 * @param low The starting index of the subarray.
 * @param high The ending index of the subarray.
 */
static void insertionSort(VaccineLot **array, int low, int high) {
  for (int i = low + 1; i <= high; i++) {
    VaccineLot *key = array[i];
    int j = i - 1;
    // Move elements of array[0..i-1], that are greater than key,
    // to one position ahead of their current position
    while (j >= low && compareVaccines(array[j], key) > 0) {
      array[j + 1] = array[j];
      j--;
    }
    array[j + 1] = key;
  }
}

/**
 * @brief Implementation of the quicksort algorithm. Uses insertion sort for
 * small subarrays optimization.
 *
 * @param array The array to sort.
 * @param low The lower index of the array.
 * @param high The higher index of the array.
 */
static void quickSort(VaccineLot **array, int low, int high) {
  if (low < high) {
    // For small subarrays, insertion sort is more efficient due to lower
    // overhead
    if (high - low < 10) {
      insertionSort(array, low, high);
    } else {
      // Partition the array
      int pi = partition(array, low, high);
      // Recursively sort the left and right subarrays
      quickSort(array, low, pi - 1);
      quickSort(array, pi + 1, high);
    }
  }
}

/**
 * @brief Fills an array with all vaccine lots from the linked list.
 *
 * @param vaccineList The head of the vaccine lot linked list.
 * @param vaccineArray The array to fill.
 * @return int The total number of vaccines in the linked list.
 */
static int fillVaccineArray(VaccineLot *vaccineList,
                            VaccineLot **vaccineArray) {
  int count = 0;
  VaccineLot *current = vaccineList;
  // Iterate through the linked list and add each vaccine to the array
  while (current != NULL) {
    vaccineArray[count++] = current;
    current = current->next_vaccine;
  }
  return count;
}

/**
 * @brief Prints all vaccines from a given array.
 *
 * @param vaccineArray The array of vaccine lots to print.
 * @param count The number of vaccines in the array.
 */
static void printAllVaccines(VaccineLot **vaccineArray, int count) {
  // Iterate through the array and print each vaccine
  for (int i = 0; i < count; i++) {
    printVaccine(vaccineArray[i]);
  }
}

/**
 * @brief Lists all vaccines in the system, sorted by validation date and lot
 * ID.
 *
 * @param vaccineList The head of the vaccine lot linked list.
 */
static void listAllVaccines(VaccineLot *vaccineList) {
  // Count the total number of vaccines in the linked list
  int count = 0;
  VaccineLot *current = vaccineList;
  while (current != NULL) {
    count++;
    current = current->next_vaccine;
  }
  if (count == 0)
    return; // No vaccines to list

  // Allocate memory for an array to hold all vaccine pointers
  VaccineLot **vaccineArray =
      (VaccineLot **)malloc(count * sizeof(VaccineLot *));
  if (vaccineArray == NULL) {
    printf("No memory\n");
    exit(1);
  }

  // Fill the array with vaccine pointers from the linked list
  fillVaccineArray(vaccineList, vaccineArray);

  // Sort the array using quicksort
  quickSort(vaccineArray, 0, count - 1);

  // Print all the vaccines from the sorted array
  printAllVaccines(vaccineArray, count);

  // Free the allocated memory
  free(vaccineArray);
}

/**
 * @brief Handles the output when a specified vaccine name is not found.
 *
 * @param vaccineName The name of the vaccine that was not found.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
static void handleVaccineNotFound(const char *vaccineName, int portuguese) {
  if (portuguese)
    printf("%s: vacina inexistente\n", vaccineName);
  else
    printf("%s: no such vaccine\n", vaccineName);
}

/**
 * @brief Prints all lots associated with a given vaccine name.
 *
 * @param nameEntry The VaccineNameIndex entry containing the list of lots.
 */
static void printVaccineLots(VaccineNameIndex *nameEntry) {
  // Iterate through the list of lots and print each one
  for (int i = 0; i < nameEntry->lotCount; i++) {
    printVaccine(nameEntry->lots[i]);
  }
}

/**
 * @brief Lists all vaccine lots for a specific vaccine name, sorted.
 *
 * @param nameHashTable The hash table of vaccine names.
 * @param vaccineName The name of the vaccine to list.
 * @param hashSize The size of the hash table.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
static void listVaccinesByName(VaccineNameIndex **nameHashTable,
                               const char *vaccineName, int hashSize,
                               int portuguese) {
  // Find the entry for the given vaccine name in the hash table
  VaccineNameIndex *nameEntry =
      findVaccineByName(nameHashTable, vaccineName, hashSize);

  // If the vaccine name is not found or has no associated lots
  if (nameEntry == NULL || nameEntry->lotCount == 0) {
    handleVaccineNotFound(vaccineName, portuguese);
    return;
  }

  int validCount = nameEntry->lotCount;
  if (validCount == 0) {
    handleVaccineNotFound(vaccineName, portuguese);
    return;
  }

  // Allocate memory to hold the pointers to the vaccine lots
  VaccineLot **validLots =
      (VaccineLot **)malloc(validCount * sizeof(VaccineLot *));
  if (validLots == NULL) {
    printf("No memory\n");
    exit(1);
  }

  // Fill the array with the vaccine lots
  for (int i = 0; i < nameEntry->lotCount; i++) {
    validLots[i] = nameEntry->lots[i];
  }

  // Sort the array of vaccine lots
  quickSort(validLots, 0, validCount - 1);

  // Print all the vaccine lots for the given name
  printVaccineLots(nameEntry);

  // Free the allocated memory
  free(validLots);
}

/**
 * @brief Processes the command arguments to list specific vaccines by name.
 *
 * @param args The command arguments string containing space-separated vaccine
 * names.
 * @param nameHashTable The hash table of vaccine names.
 * @param hashSize The size of the hash table.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
static void processSpecificVaccines(char *args,
                                    VaccineNameIndex **nameHashTable,
                                    int hashSize, int portuguese) {
  char buffer[SIZE_COMMAND];
  char *token =
      strtok(args, " \t"); // Tokenize the input string by space or tab
  while (token != NULL) {
    strncpy(buffer, token, SIZE_COMMAND - 1); // Copy the token to a buffer
    buffer[SIZE_COMMAND - 1] = '\0';          // Ensure null termination
    listVaccinesByName(nameHashTable, buffer, hashSize,
                       portuguese); // List vaccines with the extracted name
    token = strtok(NULL, " \t");    // Get the next token
  }
}

/**
 * @brief Command L: Lists vaccine batches based on the provided arguments.
 *
 * @param args The command arguments. If NULL or empty, lists all vaccines.
 * Otherwise, lists vaccines by name.
 * @param vaccineList The head of the vaccine lot linked list.
 * @param nameHashTable The hash table of vaccine names.
 * @param hashSize The size of the hash table.
 * @param portuguese Flag indicating if the output should be in Portuguese.
 */
void commandL(char *args, VaccineLot *vaccineList,
              VaccineNameIndex **nameHashTable, int hashSize, int portuguese) {
  // If no arguments are provided, list all vaccines
  if (args == NULL || *args == '\0') {
    listAllVaccines(vaccineList);
  } else {
    // Otherwise, process the arguments as specific vaccine names to list
    processSpecificVaccines(args, nameHashTable, hashSize, portuguese);
  }
}