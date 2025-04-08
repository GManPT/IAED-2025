/**
 * @file project.h
 * @brief Header file for the project.
 * 
 * This file contains the definitions of data structures and function declarations
 * used throughout the project.
 * 
 * Author: Vicente B. Duarte
 */

#ifndef PROJECT_H
#define PROJECT_H

// Structure for date
typedef struct {
  int day;
  int month;
  int year;
} Date;

// Forward declarations
typedef struct VaccineNameIndex VaccineNameIndex;
typedef struct VaccineLot VaccineLot;
typedef struct Inoculation Inoculation;  
typedef struct UserIndex UserIndex;

// Structure for vaccine name index
struct VaccineNameIndex {
  char *name;
  struct VaccineLot **lots;  // Array of pointers to lots with this name
  int lotCount;              // Number of lots with this name
  int capacity;              // Current capacity of the lots array
  struct VaccineNameIndex *next_hash;  // For hash table collision handling
};

// Structure for Vaccine data
struct VaccineLot {
  char *lot;
  char *name;
  Date validation;
  int doses;
  int dosesUsed;
  int isRemoved;             // Flag to mark removed lots
  struct VaccineLot *next_hash;     // For hash by lot
  struct VaccineLot *next_vaccine;  // For global list
};

// Structure for Inoculation data
struct Inoculation {
  char *user;
  char *lot;
  Date date;
  struct Inoculation *next_global;   // For global chronological list
};

// Structure for user inoculation index
struct UserIndex {
  char *userName;
  struct Inoculation **inoculations;  // Array of pointers to this user's inoculations
  int inoculationCount;               // Number of inoculations for this user
  int capacity;                       // Current capacity of the inoculations array
  struct UserIndex *next_hash;        // For hash table collision handling
};

// Function declarations (as in your previous version)
int isValidBatch(const char *batch);
int isValidName(const char *name);
int isValidDate(Date date, Date currentDate);
unsigned int hashBatch(const char *batch, int size);
unsigned int hashString(const char *str, int size);

// Additional function declarations for the optimized code
VaccineLot **initializeVaccineHashTable(int size);
VaccineNameIndex **initializeVaccineNameHashTable(int size);
UserIndex **initializeUserHashTable(int size);

VaccineLot *findVaccineByBatch(VaccineLot **hashTable, const char *batch, int size);
VaccineNameIndex *findVaccineByName(VaccineNameIndex **nameHashTable, const char *name, int size);
UserIndex *findUserByName(UserIndex **userHashTable, const char *user, int size);

VaccineLot *createVaccineLot(const char *batch, const char *name, Date validation, int doses);
Inoculation *createInoculation(const char *user, const char *lot, Date date);

void addVaccineLotToHash(VaccineLot **hashTable, VaccineLot *lot, int size);
void addVaccineLotToNameIndex(VaccineNameIndex **nameHashTable, VaccineLot *lot, int size);
void addInoculationToUserIndex(UserIndex **userHashTable, Inoculation *inoc, int size);

void freeVaccineLot(VaccineLot *lot);
void freeVaccineHashTable(VaccineLot **hashTable, int size);
void freeVaccineNameHashTable(VaccineNameIndex **nameHashTable, int size);
void freeUserHashTable(UserIndex **userHashTable, int size);
void freeInoculation(Inoculation *inoc);
void freeInoculationList(Inoculation *list);

int compareVaccines(VaccineLot *a, VaccineLot *b);
int compareDates(Date a, Date b);

#endif