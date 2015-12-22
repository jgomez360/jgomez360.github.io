#pragma once

void menu_init(void);
void menu_show(int numWorkouts, char* workoutKeyList);
void menu_deinit(void);
void addWorkout(char *workoutName, char *workoutKeyList);