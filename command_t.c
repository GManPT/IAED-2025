/**
 * @file command_t.c
 * @brief Implementation of command T functionality to manage the simulation
 * date.
 *
 * This file contains the implementation of the commandT function and its helper
 * functions. It handles parsing the command arguments, validating the input,
 * and updating or printing the simulation date.
 *
 * Author: Vicente B. Duarte
 */

#include "constants.h"
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Parses a date string in DD-MM-YYYY format.
 *
 * @param dateStr The date string to parse.
 * @param date A pointer to the Date structure where the parsed date will be
 * stored.
 * @return int 1 if the date was successfully parsed, 0 otherwise.
 */
static int parseDate(const char *dateStr, Date *date) {
  if (dateStr == NULL || date == NULL) {
    return 0; // Return 0 if input is invalid
  }

  int day, month, year;
  // Attempt to parse the date string
  if (sscanf(dateStr, "%d-%d-%d", &day, &month, &year) != 3) {
    return 0; // Return 0 if parsing fails
  }

  // Assign the parsed values to the Date structure
  date->day = day;
  date->month = month;
  date->year = year;

  return 1; // Return 1 if parsing was successful
}

/**
 * @brief Checks if a given date has a valid format (day, month, year ranges).
 *
 * @param date The Date structure to check.
 * @return int 1 if the date format is valid, 0 otherwise.
 */
static int isDateFormatValid(Date date) {
  // Check if the month is within the valid range (1-12)
  if (date.month < 1 || date.month > 12) {
    return 0;
  }

  // Array containing the number of days in each month (0-indexed for
  // convenience)
  int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  // Adjust the number of days in February for leap years
  if (date.month == 2 &&
      (date.year % 4 == 0 && (date.year % 100 != 0 || date.year % 400 == 0))) {
    daysInMonth[2] = 29;
  }

  // Check if the day is within the valid range for the given month
  if (date.day < 1 || date.day > daysInMonth[date.month]) {
    return 0;
  }

  return 1; // Return 1 if the date format is valid
}

/**
 * @brief Prints the current date in DD-MM-YYYY format.
 *
 * @param currentDate The current date to print.
 */
static void printCurrentDate(Date currentDate) {
  printf("%02d-%02d-%d\n", currentDate.day, currentDate.month,
         currentDate.year);
}

/**
 * @brief Prints an "invalid date" message based on the language setting.
 *
 * @param portuguese Flag indicating if the output should be in Portuguese (1)
 * or English (0).
 */
static void printInvalidDateMessage(int portuguese) {
  if (portuguese) {
    printf("data inválida\n");
  } else {
    printf("invalid date\n");
  }
}

/**
 * @brief Advances the simulation time to a new date or prints the current date.
 *
 * @param args The command arguments. If NULL or empty, prints the current date.
 * Otherwise, it should contain the new date in DD-MM-YYYY format.
 * @param currentDate A pointer to the current date structure to be updated.
 * @param portuguese Flag indicating if the output should be in Portuguese (1)
 * or English (0).
 */
void commandT(char *args, Date *currentDate, int portuguese) {
  // If no arguments are provided, just print the current date
  if (args == NULL || *args == '\0') {
    printCurrentDate(*currentDate);
    return;
  }

  // Parse the new date from the arguments
  Date newDate;
  if (!parseDate(args, &newDate)) {
    printInvalidDateMessage(portuguese);
    return;
  }

  // First, check if the format of the new date is valid
  if (!isDateFormatValid(newDate)) {
    printInvalidDateMessage(portuguese);
    return;
  }

  // Check if the new date is not before the current date
  if (compareDates(*currentDate, newDate) > 0) {
    printInvalidDateMessage(portuguese);
    return;
  }

  // Update the current date to the new date
  *currentDate = newDate;
  // Print the updated current date
  printCurrentDate(*currentDate);
}