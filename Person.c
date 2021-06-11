#include "Person.h"

#define INFECTION_RATE_NOT_CALCULATE 0
#define INFECTION_RATE_ALREADY_CALCULATE 1
#define MAX_NAME_LENGTH 80


Person *PersonAlloc(IdT id,char *name, size_t age, int is_sick) {
  Person *person = calloc(1,sizeof(Person));
  if (!person) {
    free(person);
    return NULL;
  }
  person->id = id;
  person->name = calloc(MAX_NAME_LENGTH,sizeof(char));
  if (!person->name){
    return NULL;
  }
  if (name){
    strcpy(person->name, name);
  }
  person->age = age;
  person->is_sick = is_sick;
  person->infection_rate = INFECTION_RATE_NOT_CALCULATE;
  if (is_sick) {
    person->infection_rate = INFECTION_RATE_ALREADY_CALCULATE;
  }
  person->num_of_meetings = 0;
  person->meetings = malloc(sizeof(Meeting) * PERSON_INITIAL_SIZE);
  if (!person->meetings) {
    free(person->meetings);
    return NULL;
  }
  person->meetings_capacity = PERSON_INITIAL_SIZE;
  return person;
}

void PersonFree(Person **p_person) {
  if (!p_person){
    return;
  }
  if (!*p_person) {
    return;
  }
  free((*p_person)->meetings);
  free((*p_person)->name);
  free(*p_person);
}

Meeting *PersonGetMeetingById(const Person *const person, IdT id) {
  if (!person) {
    return NULL;
  }
  for (int i = 0; i < person->num_of_meetings; ++i) {
    if (person->meetings[i]->person_1->id == id || person->meetings[i]->person_2->id == id) {
      return person->meetings[i];
    }
  }
  return NULL;
}

int PersonCompareById(const Person *person_1, const Person *person_2) {
  if (!person_1 || !person_2) {
    return 0;
  }
  if (person_1->id == person_2->id) {
    return 0;
  } else if (person_1->id < person_2->id) {
    return -1;
  } else {
    return 1;
  }
}

int PersonCompareByName(const Person *person_1, const Person *person_2) {
  if (!person_1 || !person_2) {
    return 0;
  }
  int res = strcmp(person_1->name, person_2->name);
  if (res==0){
    return 0;
  } else if (res<0){
    return -1;
  } else{
    return 1;
  }
}

int PersonCompareByInfectionRate(const Person *person_1, const Person *person_2) {
  if (!person_1 || !person_2) {
    return 0;
  }
  if (person_1->infection_rate == person_2->infection_rate) {
    return 0;
  } else if (person_1->infection_rate < person_2->infection_rate) {
    return 1;
  } else {
    return -1;
  }
}

int PersonCompareByAge(const Person *person_1, const Person *person_2){
  if (!person_1 || !person_2) {
    return 0;
  }
  if (person_1->age == person_2->age) {
    return 0;
  } else if (person_1->age < person_2->age) {
    return 1;
  } else {
    return -1;
  }
}


