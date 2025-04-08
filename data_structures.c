/**
 * @file data_structures.c
 * @brief Implementation of data structures and related functions.
 *
 * This file contains the implementation of functions for managing vaccine lots
 * and inoculations, including initialization, creation, addition, and memory
 * management.
 *
 * Author: Vicente B. Duarte
 */

#include "constants.h"
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Initializes the hash table for vaccine batches.
 *
 * @param size The size of the hash table.
 * @return VaccineLot** The initialized hash table.
 */
VaccineLot **initializeVaccineHashTable(int size) {
  VaccineLot **hashTable = (VaccineLot **)malloc(size * sizeof(VaccineLot *));
  if (hashTable == NULL) {
    printf("No memory\n");
    exit(1);
  }

  for (int i = 0; i < size; i++) {
    hashTable[i] = NULL;
  }
  return hashTable;
}

/**
 * @brief Initializes the hash table for vaccine names.
 *
 * @param size The size of the hash table.
 * @return VaccineNameIndex** The initialized hash table.
 */
VaccineNameIndex **initializeVaccineNameHashTable(int size) {
  VaccineNameIndex **nameHashTable =
      (VaccineNameIndex **)malloc(size * sizeof(VaccineNameIndex *));
  if (nameHashTable == NULL) {
    printf("No memory\n");
    exit(1);
  }

  for (int i = 0; i < size; i++) {
    nameHashTable[i] = NULL;
  }
  return nameHashTable;
}

/**
 * @brief Initializes the hash table for user indices.
 *
 * @param size The size of the hash table.
 * @return UserIndex** The initialized hash table.
 */
UserIndex **initializeUserHashTable(int size) {
  UserIndex **userHashTable = (UserIndex **)malloc(size * sizeof(UserIndex *));
  if (userHashTable == NULL) {
    printf("No memory\n");
    exit(1);
  }

  for (int i = 0; i < size; i++) {
    userHashTable[i] = NULL;
  }
  return userHashTable;
}

/**
 * @brief Checks if a batch already exists in the hash table.
 *
 * @param hashTable The hash table of vaccine lots.
 * @param batch The batch identifier.
 * @param size The size of the hash table.
 * @return VaccineLot* The found vaccine lot, or NULL if not found.
 */
VaccineLot *findVaccineByBatch(VaccineLot **hashTable, const char *batch,
                               int size) {
  unsigned int index = hashString(batch, size);
  VaccineLot *current = hashTable[index];

  while (current != NULL) {
    if (strcmp(current->lot, batch) == 0) {
      return current;
    }
    current = current->next_hash;
  }
  return NULL;
}

/**
 * @brief Finds a vaccine by name in the hash table.
 *
 * @param nameHashTable The hash table of vaccine names.
 * @param name The vaccine name to find.
 * @param size The size of the hash table.
 * @return VaccineNameIndex* The found vaccine name entry, or NULL if not found.
 */
VaccineNameIndex *findVaccineByName(VaccineNameIndex **nameHashTable,
                                    const char *name, int size) {
  unsigned int index = hashString(name, size);
  VaccineNameIndex *current = nameHashTable[index];

  while (current != NULL) {
    if (strcmp(current->name, name) == 0) {
      return current;
    }
    current = current->next_hash;
  }
  return NULL;
}

/**
 * @brief Finds a user by name in the hash table.
 *
 * @param userHashTable The hash table of user indices.
 * @param user The user name to find.
 * @param size The size of the hash table.
 * @return UserIndex* The found user index, or NULL if not found.
 */
UserIndex *findUserByName(UserIndex **userHashTable, const char *user,
                          int size) {
  unsigned int index = hashString(user, size);
  UserIndex *current = userHashTable[index];

  while (current != NULL) {
    if (strcmp(current->userName, user) == 0) {
      return current;
    }
    current = current->next_hash;
  }
  return NULL;
}

/**
 * @brief Helper function to allocate memory for a vaccine lot and its strings.
 *
 * @param batch The batch identifier.
 * @param name The vaccine name.
 * @return char* Pointer to the allocated memory block.
 */
static char *allocateVaccineLotMemory(size_t batch_len, size_t name_len) {
  char *mem_block = (char *)malloc(sizeof(VaccineLot) + batch_len + name_len);
  if (mem_block == NULL) {
    printf("No memory\n");
    exit(1);
  }
  return mem_block;
}

/**
 * @brief Helper function to initialize non-string fields of a VaccineLot.
 *
 * @param lot The vaccine lot to initialize.
 * @param validation The validation date.
 * @param doses The number of doses.
 */
static void initializeVaccineLotFields(VaccineLot *lot, Date validation,
                                       int doses) {
  lot->validation = validation;
  lot->doses = doses;
  lot->dosesUsed = 0;
  lot->isRemoved = 0;
  lot->next_hash = NULL;
  lot->next_vaccine = NULL;
}

/**
 * @brief Creates a new vaccine batch.
 *
 * @param batch The batch identifier.
 * @param name The vaccine name.
 * @param validation The validation date.
 * @param doses The number of doses.
 * @return VaccineLot* The created vaccine lot.
 */
VaccineLot *createVaccineLot(const char *batch, const char *name,
                             Date validation, int doses) {
  size_t batch_len = strlen(batch) + 1;
  size_t name_len = strlen(name) + 1;

  char *mem_block = allocateVaccineLotMemory(batch_len, name_len);
  VaccineLot *newLot = (VaccineLot *)mem_block;
  char *strings_area = mem_block + sizeof(VaccineLot);

  newLot->lot = strings_area;
  newLot->name = strings_area + batch_len;

  memcpy(newLot->lot, batch, batch_len);
  memcpy(newLot->name, name, name_len);

  initializeVaccineLotFields(newLot, validation, doses);

  return newLot;
}

/**
 * @brief Helper function to allocate memory for an inoculation and its strings.
 *
 * @param user The user name.
 * @param lot The batch identifier.
 * @return char* Pointer to the allocated memory block.
 */
static char *allocateInoculationMemory(size_t user_len, size_t lot_len) {
  char *mem_block = (char *)malloc(sizeof(Inoculation) + user_len + lot_len);
  if (mem_block == NULL) {
    printf("No memory\n");
    exit(1);
  }
  return mem_block;
}

/**
 * @brief Creates a new inoculation record.
 *
 * @param user The user name.
 * @param lot The batch identifier.
 * @param date The inoculation date.
 * @return Inoculation* The created inoculation.
 */
Inoculation *createInoculation(const char *user, const char *lot, Date date) {
  size_t user_len = strlen(user) + 1;
  size_t lot_len = strlen(lot) + 1;

  char *mem_block = allocateInoculationMemory(user_len, lot_len);
  Inoculation *newInoc = (Inoculation *)mem_block;
  char *strings_area = mem_block + sizeof(Inoculation);

  newInoc->user = strings_area;
  newInoc->lot = strings_area + user_len;

  memcpy(newInoc->user, user, user_len);
  memcpy(newInoc->lot, lot, lot_len);

  newInoc->date = date;
  newInoc->next_global = NULL;

  return newInoc;
}

/**
 * @brief Adds a batch to the hash table.
 *
 * @param hashTable The hash table of vaccine lots.
 * @param lot The vaccine lot to add.
 * @param size The size of the hash table.
 */
void addVaccineLotToHash(VaccineLot **hashTable, VaccineLot *lot, int size) {
  unsigned int index = hashString(lot->lot, size);
  lot->next_hash = hashTable[index];
  hashTable[index] = lot;
}

/**
 * @brief Helper function to find an existing VaccineNameIndex entry.
 *
 * @param nameHashTable The hash table of vaccine names.
 * @param name The vaccine name.
 * @param size The size of the hash table.
 * @return VaccineNameIndex* The found entry, or NULL if not found.
 */
static VaccineNameIndex *
findExistingNameIndexEntry(VaccineNameIndex **nameHashTable, const char *name,
                           int size) {
  unsigned int index = hashString(name, size);
  VaccineNameIndex *current = nameHashTable[index];
  while (current != NULL) {
    if (strcmp(current->name, name) == 0) {
      return current;
    }
    current = current->next_hash;
  }
  return NULL;
}

/**
 * @brief Helper function to create a new VaccineNameIndex entry.
 *
 * @param name The vaccine name.
 * @param index The hash index for the new entry.
 * @param nameHashTable The hash table.
 * @return VaccineNameIndex* The newly created entry.
 */
static VaccineNameIndex *
createNameIndexEntry(const char *name, unsigned int index,
                     VaccineNameIndex **nameHashTable) {
  VaccineNameIndex *newNameEntry =
      (VaccineNameIndex *)malloc(sizeof(VaccineNameIndex));
  if (newNameEntry == NULL) {
    printf("No memory\n");
    exit(1);
  }
  newNameEntry->name = strdup(name);
  if (newNameEntry->name == NULL) {
    free(newNameEntry);
    printf("No memory\n");
    exit(1);
  }
  newNameEntry->capacity = 4;
  newNameEntry->lots =
      (VaccineLot **)malloc(newNameEntry->capacity * sizeof(VaccineLot *));
  if (newNameEntry->lots == NULL) {
    free(newNameEntry->name);
    free(newNameEntry);
    printf("No memory\n");
    exit(1);
  }
  newNameEntry->lotCount = 0;
  newNameEntry->next_hash = nameHashTable[index];
  nameHashTable[index] = newNameEntry;
  return newNameEntry;
}

/**
 * @brief Helper function to find or create a VaccineNameIndex entry.
 *
 * @param nameHashTable The hash table of vaccine names.
 * @param name The vaccine name.
 * @param size The size of the hash table.
 * @return VaccineNameIndex* The found or newly created VaccineNameIndex entry.
 */
static VaccineNameIndex *
findOrCreateNameIndexEntry(VaccineNameIndex **nameHashTable, const char *name,
                           int size) {
  unsigned int index = hashString(name, size);
  VaccineNameIndex *entry =
      findExistingNameIndexEntry(nameHashTable, name, size);
  if (entry == NULL) {
    entry = createNameIndexEntry(name, index, nameHashTable);
  }
  return entry;
}

/**
 * @brief Helper function to resize the lots array in VaccineNameIndex.
 *
 * @param nameEntry The VaccineNameIndex entry.
 */
static void resizeNameIndexLots(VaccineNameIndex *nameEntry) {
  int newCapacity = nameEntry->capacity * 2;
  VaccineLot **newLots = (VaccineLot **)realloc(
      nameEntry->lots, newCapacity * sizeof(VaccineLot *));
  if (newLots == NULL) {
    printf("No memory\n");
    exit(1);
  }
  nameEntry->lots = newLots;
  nameEntry->capacity = newCapacity;
}

/**
 * @brief Adds a vaccine lot to the name index.
 *
 * @param nameHashTable The hash table of vaccine names.
 * @param lot The vaccine lot to add.
 * @param size The size of the hash table.
 */
void addVaccineLotToNameIndex(VaccineNameIndex **nameHashTable, VaccineLot *lot,
                              int size) {
  VaccineNameIndex *nameEntry =
      findOrCreateNameIndexEntry(nameHashTable, lot->name, size);
  if (nameEntry->lotCount >= nameEntry->capacity) {
    resizeNameIndexLots(nameEntry);
  }
  nameEntry->lots[nameEntry->lotCount++] = lot;
}

/**
 * @brief Helper function to find an existing UserIndex entry.
 *
 * @param userHashTable The hash table of user indices.
 * @param userName The user name.
 * @param size The size of the hash table.
 * @return UserIndex* The found entry, or NULL if not found.
 */
static UserIndex *findExistingUserIndexEntry(UserIndex **userHashTable,
                                             const char *userName, int size) {
  unsigned int index = hashString(userName, size);
  UserIndex *current = userHashTable[index];
  while (current != NULL) {
    if (strcmp(current->userName, userName) == 0) {
      return current;
    }
    current = current->next_hash;
  }
  return NULL;
}

/**
 * @brief Helper function to create a new UserIndex entry.
 *
 * @param userName The user name.
 * @param index The hash index for the new entry.
 * @param userHashTable The hash table.
 * @return UserIndex* The newly created entry.
 */
static UserIndex *createUserIndexEntry(const char *userName, unsigned int index,
                                       UserIndex **userHashTable) {
  UserIndex *newUserEntry = (UserIndex *)malloc(sizeof(UserIndex));
  if (newUserEntry == NULL) {
    printf("No memory\n");
    exit(1);
  }
  newUserEntry->userName = strdup(userName);
  if (newUserEntry->userName == NULL) {
    free(newUserEntry);
    printf("No memory\n");
    exit(1);
  }
  newUserEntry->capacity = 4;
  newUserEntry->inoculations =
      (Inoculation **)malloc(newUserEntry->capacity * sizeof(Inoculation *));
  if (newUserEntry->inoculations == NULL) {
    free(newUserEntry->userName);
    free(newUserEntry);
    printf("No memory\n");
    exit(1);
  }
  newUserEntry->inoculationCount = 0;
  newUserEntry->next_hash = userHashTable[index];
  userHashTable[index] = newUserEntry;
  return newUserEntry;
}

/**
 * @brief Helper function to find or create a UserIndex entry.
 *
 * @param userHashTable The hash table of user indices.
 * @param userName The user name.
 * @param size The size of the hash table.
 * @return UserIndex* The found or newly created UserIndex entry.
 */
static UserIndex *findOrCreateUserIndexEntry(UserIndex **userHashTable,
                                             const char *userName, int size) {
  unsigned int index = hashString(userName, size);
  UserIndex *entry = findExistingUserIndexEntry(userHashTable, userName, size);
  if (entry == NULL) {
    entry = createUserIndexEntry(userName, index, userHashTable);
  }
  return entry;
}

/**
 * @brief Helper function to resize the inoculations array in UserIndex.
 *
 * @param userEntry The UserIndex entry.
 */
static void resizeUserIndexInocs(UserIndex *userEntry) {
  int newCapacity = userEntry->capacity * 2;
  Inoculation **newInocs = (Inoculation **)realloc(
      userEntry->inoculations, newCapacity * sizeof(Inoculation *));
  if (newInocs == NULL) {
    printf("No memory\n");
    exit(1);
  }
  userEntry->inoculations = newInocs;
  userEntry->capacity = newCapacity;
}

/**
 * @brief Adds an inoculation to the user index.
 *
 * @param userHashTable The hash table of user indices.
 * @param inoc The inoculation to add.
 * @param size The size of the hash table.
 */
void addInoculationToUserIndex(UserIndex **userHashTable, Inoculation *inoc,
                               int size) {
  UserIndex *userEntry =
      findOrCreateUserIndexEntry(userHashTable, inoc->user, size);
  if (userEntry->inoculationCount >= userEntry->capacity) {
    resizeUserIndexInocs(userEntry);
  }
  userEntry->inoculations[userEntry->inoculationCount++] = inoc;
}

/**
 * @brief Hash function for strings.
 *
 * @param str The string to hash.
 * @param size The size of the hash table.
 * @return unsigned int The hash value.
 */
unsigned int hashString(const char *str, int size) {
  unsigned int hash = 5381;
  int c;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash % size;
}

/**
 * @brief Frees a vaccine lot.
 *
 * @param lot The vaccine lot to free.
 */
void freeVaccineLot(VaccineLot *lot) {
  if (lot != NULL) {
    free(lot);
  }
}

/**
 * @brief Frees all vaccine lots in the hash table.
 *
 * @param hashTable The hash table of vaccine lots.
 * @param size The size of the hash table.
 */
void freeVaccineHashTable(VaccineLot **hashTable, int size) {
  if (hashTable == NULL)
    return;

  for (int i = 0; i < size; i++) {
    VaccineLot *current = hashTable[i];
    while (current != NULL) {
      VaccineLot *next = current->next_hash;
      freeVaccineLot(current);
      current = next;
    }
  }
  free(hashTable);
}

/**
 * @brief Frees the memory used by the vaccine name hash table.
 *
 * @param nameHashTable The hash table of vaccine names.
 * @param size The size of the hash table.
 */
void freeVaccineNameHashTable(VaccineNameIndex **nameHashTable, int size) {
  if (nameHashTable == NULL)
    return;

  for (int i = 0; i < size; i++) {
    VaccineNameIndex *current = nameHashTable[i];
    while (current != NULL) {
      VaccineNameIndex *next = current->next_hash;
      free(current->name);
      free(current->lots);
      free(current);
      current = next;
    }
  }
  free(nameHashTable);
}

/**
 * @brief Frees the memory used by the user hash table.
 *
 * @param userHashTable The hash table of user indices.
 * @param size The size of the hash table.
 */
void freeUserHashTable(UserIndex **userHashTable, int size) {
  if (userHashTable == NULL)
    return;

  for (int i = 0; i < size; i++) {
    UserIndex *current = userHashTable[i];
    while (current != NULL) {
      UserIndex *next = current->next_hash;
      free(current->userName);
      free(current->inoculations);
      free(current);
      current = next;
    }
  }
  free(userHashTable);
}

/**
 * @brief Frees an inoculation.
 *
 * @param inoc The inoculation to free.
 */
void freeInoculation(Inoculation *inoc) {
  if (inoc != NULL) {
    free(inoc);
  }
}

/**
 * @brief Frees all inoculations in the list.
 *
 * @param list The list of inoculations.
 */
void freeInoculationList(Inoculation *list) {
  Inoculation *current = list;

  while (current != NULL) {
    Inoculation *next = current->next_global;
    freeInoculation(current);
    current = next;
  }
}

/**
 * @brief Compares two dates.
 *
 * @param a The first date.
 * @param b The second date.
 * @return int Negative if a < b, 0 if a == b, positive if a > b.
 */
int compareDates(Date a, Date b) {
  if (a.year != b.year)
    return a.year - b.year;

  if (a.month != b.month)
    return a.month - b.month;

  return a.day - b.day;
}