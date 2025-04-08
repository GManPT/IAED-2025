/**
 * @file utils.c
 * @brief Utility functions for the project.
 *
 * This file contains utility functions for validating batches, names, and
 * dates, as well as a hash function for batches.
 *
 * Author: Vicente B. Duarte
 */

#include "constants.h"
#include "project.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Checks if a batch is valid (maximum 20 uppercase hexadecimal digits).
 *
 * @param batch The batch identifier.
 * @return int 1 if the batch is valid, 0 otherwise.
 */
int isValidBatch(const char *batch) {
  int length = strlen(batch);
  if (length > 20) {
    return 0; // Batch with more than 20 digits
  }

  for (int i = 0; i < length; i++) {
    // Valid characters are digits (0-9) or uppercase hexadecimal letters (A-F)
    if (!(isdigit(batch[i]) || (batch[i] >= 'A' && batch[i] <= 'F'))) {
      return 0; // Character is not an uppercase hexadecimal digit
    }
  }
  return 1;
}

/**
 * @brief Checks if a name is valid (no whitespace characters and maximum 50
 * bytes).
 *
 * @param name The name to validate.
 * @return int 1 if the name is valid, 0 otherwise.
 */
int isValidName(const char *name) {
  int length = strlen(name);
  if (length > 50) {
    return 0; // Name with more than 50 bytes
  }

  for (int i = 0; i < length; i++) {
    if (isspace(name[i])) {
      return 0; // Name contains whitespace
    }
  }
  return 1;
}

/**
 * @brief Checks if the month is valid.
 *
 * @param month The month to validate.
 * @return int 1 if the month is valid, 0 otherwise.
 */
int isMonthValid(int month) { return month >= 1 && month <= 12; }

/**
 * @brief Gets the number of days in a given month, considering leap years.
 *
 * @param month The month to check.
 * @param year The year to check (for leap year adjustment).
 * @return int The number of days in the month.
 */
int getDaysInMonth(int month, int year) {
  int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 2 && (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))) {
    return 29; // February in a leap year
  }
  return daysInMonth[month];
}

/**
 * @brief Checks if the day is valid for a given month and year.
 *
 * @param day The day to validate.
 * @param month The month of the date.
 * @param year The year of the date.
 * @return int 1 if the day is valid, 0 otherwise.
 */
int isDayValid(int day, int month, int year) {
  int daysInMonth = getDaysInMonth(month, year);
  return day >= 1 && day <= daysInMonth;
}

/**
 * @brief Checks if a date is not earlier than the current date.
 *
 * @param date The date to validate.
 * @param currentDate The current date.
 * @return int 1 if the date is not earlier, 0 otherwise.
 */
int isDateNotEarlier(Date date, Date currentDate) {
  if (date.year < currentDate.year)
    return 0;
  if (date.year == currentDate.year && date.month < currentDate.month)
    return 0;
  if (date.year == currentDate.year && date.month == currentDate.month &&
      date.day < currentDate.day)
    return 0;
  return 1;
}

/**
 * @brief Checks if a date is valid and not earlier than the current date.
 *
 * @param date The date to validate.
 * @param currentDate The current date.
 * @return int 1 if the date is valid, 0 otherwise.
 */
int isValidDate(Date date, Date currentDate) {
  if (!isMonthValid(date.month))
    return 0;
  if (!isDayValid(date.day, date.month, date.year))
    return 0;
  if (!isDateNotEarlier(date, currentDate))
    return 0;
  return 1;
}

/**
 * @brief Hash function for batches.
 *
 * @param batch The batch identifier.
 * @param size The size of the hash table.
 * @return unsigned int The hash value.
 */
unsigned int hashBatch(const char *batch, int size) {
  unsigned int hash = 5381;
  int c;

  while ((c = *batch++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash % size;
}
