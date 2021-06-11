#include "SpreaderDetector.h"
#include "Constants.h"

#define DELIMITERS " " // these are the delimiters for strtok - the characters to split by
#define FILE_OPEN_FOR_READING "r"
#define FILE_OPEN_FOR_WRITING "w"
#define FILE_NOT_FOUND_ERR "ERROR: File not found!\n"
#define INFECTION_RATE_NOT_CALCULATE 1
#define SPREAD_ALREADY_CALCULATE 1
#define NO_SUCH_PERSON -1
#define ID_NOT_IN -1
#define SICK 1
#define NOT_SICK 0
#define AGE_ADDITION 0.08

SpreaderDetector *SpreaderDetectorAlloc() {
  SpreaderDetector *spreader_detector = malloc(sizeof(SpreaderDetector));
  if (!spreader_detector) {
    free(spreader_detector);
    return NULL;
  }
  // Peoples array allocation
  spreader_detector->people = (Person **) malloc(sizeof(Person) * SPREADER_DETECTOR_INITIAL_SIZE);
  if (!spreader_detector->people) {
    free(spreader_detector->people);
    return NULL;
  }
  spreader_detector->people_cap = sizeof(Person) / SPREADER_DETECTOR_INITIAL_SIZE;
  spreader_detector->people_size = 0;
  // Meetings array allocation
  spreader_detector->meetings = (Meeting **) malloc(sizeof(Meeting) * SPREADER_DETECTOR_INITIAL_SIZE);
  if (!spreader_detector->meetings) {
    free(spreader_detector->meetings);
    return NULL;
  }
  spreader_detector->meeting_cap = sizeof(Meeting) / SPREADER_DETECTOR_INITIAL_SIZE;
  spreader_detector->meeting_size = 0;
  //  Returning spreader detector
  return spreader_detector;
}

void SpreaderDetectorFree(SpreaderDetector **p_spreader_detector) {
  if (!p_spreader_detector){
    return;
  }
  if (*p_spreader_detector){
    free((*p_spreader_detector)->people);
    free((*p_spreader_detector)->meetings);
  }
  free(*p_spreader_detector);
}

/**
 * @param spreader_detector
 * @param person
 * @return -1 for no, other number bigger than -1
 */
size_t IsIdAlreadyIn(SpreaderDetector *spreader_detector , IdT id) {
  for (size_t i = 0; i < spreader_detector->people_size; ++i) {
    if (spreader_detector->people[i]->id==id) {
      return i;
    }
  }
  return -1;
}

/**
 * @param spreader_detector
 * @param person
 * @return -1 if fails
 */
size_t FindSickPersonIndexInPeople(SpreaderDetector *spreader_detector) {
  for (size_t i = 0; i < spreader_detector->people_size; ++i) {
    if (spreader_detector->people[i]->is_sick) {
      return i;
    }
  }
  return -1;
}

/**
 * @param spreader_detector
 * @param person
 * @return -1 if fails
 */
size_t FindPersonIndexInPeople(SpreaderDetector *spreader_detector, Person *person) {
  for (size_t i = 0; i < spreader_detector->people_size; ++i) {
    if (spreader_detector->people[i]->id == person->id) {
      return i;
    }
  }
  return -1;
}

int SpreaderDetectorAddPerson(SpreaderDetector *spreader_detector, Person *person) {
  if (!person || !spreader_detector) {
    return 0;
  }
  if (spreader_detector->people_size < spreader_detector->people_cap) { // still room for one more person
    spreader_detector->people[spreader_detector->people_size] = person;
    spreader_detector->people_size++;
  } else if (spreader_detector->people_size == spreader_detector->people_cap) { // no room for one more person
    Person **temp =
        realloc(spreader_detector->people,
                sizeof(Person) * spreader_detector->people_cap * SPREADER_DETECTOR_GROWTH_FACTOR);
    if (!temp) {
      free(temp);
      return 0;
    } // now one person can even bring some friends (under corona limitations of course...)
    spreader_detector->people = temp;
    spreader_detector->people_cap = (spreader_detector->people_cap * SPREADER_DETECTOR_GROWTH_FACTOR);
    spreader_detector->people[spreader_detector->people_size] = person;
    spreader_detector->people_size++;
  }
  return 1; // successful addition
}

void AddMeetingToPerson(Person *person, Meeting *meeting) {
  if (person->num_of_meetings == person->meetings_capacity) {
    Meeting **meeting_temp =
        realloc(person->meetings, sizeof(Meeting) * person->meetings_capacity * SPREADER_DETECTOR_GROWTH_FACTOR);
    if (!meeting_temp) {
      free(meeting_temp);
      return;
    }
    person->meetings = meeting_temp;
    person->meetings_capacity = (person->meetings_capacity * SPREADER_DETECTOR_GROWTH_FACTOR);
    person->meetings[person->num_of_meetings] = meeting;
    person->num_of_meetings++;
  } else {
    person->meetings[person->num_of_meetings] = meeting;
    person->num_of_meetings++;
  }
}

int SpreaderDetectorAddMeeting(SpreaderDetector *spreader_detector, Meeting *meeting) {
  if (!spreader_detector || !meeting) {
    return 0;
  }
  if (!meeting->person_1 || !meeting->person_2) {
    return 0;
  }
  if (FindPersonIndexInPeople(spreader_detector, meeting->person_1)==NO_SUCH_PERSON
      || FindPersonIndexInPeople(spreader_detector, meeting->person_2)==NO_SUCH_PERSON) {
    return 0;
  }
  if (spreader_detector->meeting_size < spreader_detector->meeting_cap) { // still room for one more person
    spreader_detector->meetings[spreader_detector->meeting_size] = meeting;
    spreader_detector->meeting_size++;
  } else if (spreader_detector->meeting_size == spreader_detector->meeting_cap) { // no room even for one more person
    Meeting **temp_meeting =
        realloc(spreader_detector->meetings,
                sizeof(Meeting) * spreader_detector->meeting_cap * SPREADER_DETECTOR_GROWTH_FACTOR);
    if (!temp_meeting) {
      free(temp_meeting);
      return 0;
    } // now theres room
    spreader_detector->meetings = temp_meeting;
    spreader_detector->meeting_cap = spreader_detector->meeting_cap * SPREADER_DETECTOR_GROWTH_FACTOR;
    spreader_detector->meetings[spreader_detector->meeting_size] = meeting;
    spreader_detector->meeting_size++;
  }
  AddMeetingToPerson(meeting->person_1, meeting);
  AddMeetingToPerson(meeting->person_2, meeting);
  return 1; // successful addition
}

/**
 *
 * @param meeting
 * @return the infection rate for person2
 */
double Crna(Person *person1, Meeting *meeting, Person *person2) {
  if (person2->age > AGE_THRESHOLD) {
    return ((person1->infection_rate) * (meeting->measure / meeting->distance) * (MIN_DISTANCE / MAX_MEASURE))
        + AGE_ADDITION;
  }
  return ((person1->infection_rate) * (meeting->measure / meeting->distance) * (MIN_DISTANCE / MAX_MEASURE));
}

/**
 * Calculates infection rates for all the peoples person had met and recursively for the peoples they met and so on.
 */
void CalcMeetings(SpreaderDetector *spreader_detector, Person *person, int *spread_array) {
  if (!spreader_detector || !person) {
    return;
  }
  // Calculate infection rates for all the peoples person had met
  for (int i = 0; i < person->num_of_meetings; ++i) {
    if (person->meetings[i]->person_1->id == person->id &&
        !person->meetings[i]->person_2->infection_rate) {
      person->meetings[i]->person_2->infection_rate =
          Crna(person, person->meetings[i], person->meetings[i]->person_2);
    } else if (person->meetings[i]->person_2->id == person->id
        && !person->meetings[i]->person_1->infection_rate) {
      person->meetings[i]->person_1->infection_rate = Crna(person, person->meetings[i], person->meetings[i]->person_1);
    }
  }
  // mark person as spread already calculate
  spread_array[FindPersonIndexInPeople(spreader_detector, person)] = SPREAD_ALREADY_CALCULATE;
  // now do the same for all the peoples person had met and spread wasn't calculate
  for (int i = 0; i < person->num_of_meetings; ++i) {
    if (person->meetings[i]->person_1->id == person->id &&
        !spread_array[FindPersonIndexInPeople(spreader_detector, person->meetings[i]->person_2)]) {
      CalcMeetings(spreader_detector, person->meetings[i]->person_2, spread_array);
    } else if (person->meetings[i]->person_1->id == person->id &&
        !spread_array[FindPersonIndexInPeople(spreader_detector, person->meetings[i]->person_2)]) {
      CalcMeetings(spreader_detector, person->meetings[i]->person_1, spread_array);
    }
  }
}

void SpreaderDetectorCalculateInfectionChances(SpreaderDetector *spreader_detector) {
  if (!spreader_detector) {
    return;
  }
  // get a pointer to the sick person
  size_t sick_person_index = FindSickPersonIndexInPeople(spreader_detector);
  if (sick_person_index==NO_SUCH_PERSON){
    return;
  }
  Person *sick = spreader_detector->people[sick_person_index];
  // create a dynamic array to indicate spread calculation of a person
  int *spread_array = calloc(spreader_detector->people_size, sizeof(int));
  // compute infection rates for everyone
  CalcMeetings(spreader_detector, sick, spread_array);
  free(spread_array);
}

size_t SpreaderDetectorGetNumOfPeople(SpreaderDetector *spreader_detector) {
  if (!spreader_detector) {
    return 0;
  }
  return spreader_detector->people_size;
}

size_t SpreaderDetectorGetNumOfMeetings(SpreaderDetector *spreader_detector) {
  if (!spreader_detector) {
    return 0;
  }
  return spreader_detector->meeting_size;
}

void SpreaderDetectorReadPeopleFile(SpreaderDetector *spreader_detector, const char *path) {
  if (!spreader_detector || !path) {
    return;
  }
  FILE *fd = fopen(path, FILE_OPEN_FOR_READING); // open the given file in read mode
  if (fd == NULL) { // check the file opened correctly
    fprintf(stderr, FILE_NOT_FOUND_ERR);
    return;
  }
  int sick_indicator = NOT_SICK;
  IdT id, age;
  char buffer[MAX_LEN_OF_LINE] = {0}, *token = NULL;
  while (fgets(buffer, MAX_LEN_OF_LINE, fd) != NULL) {
    if (!(token = strtok(buffer, DELIMITERS))) continue; // set the string to parse, read first token
    char *name = malloc(sizeof(token));
    if (!name) continue;
    strcpy(name, token);
    if (!(token = strtok(NULL, DELIMITERS))) continue;
    id = strtol(token, NULL, 10);
    if (!(token = strtok(NULL, DELIMITERS))) continue;
    age = strtol(token, NULL, 10);
    token = strtok(NULL, DELIMITERS);
    if (token) {
      sick_indicator = SICK;
    } else {
      sick_indicator = NOT_SICK;
    }
    if (IsIdAlreadyIn(spreader_detector, id)==ID_NOT_IN){
      SpreaderDetectorAddPerson(spreader_detector, PersonAlloc(id, name, age, sick_indicator));
    }
    free(name);
  }
  fclose(fd);
}

void SpreaderDetectorReadMeetingsFile(SpreaderDetector *spreader_detector, const char *path) {
  if (!spreader_detector || !path) {
    return;
  }
  FILE *fd = fopen(path, FILE_OPEN_FOR_READING); // open the given file in read mode
  if (fd == NULL) { // check the file opened correctly
    fprintf(stderr, FILE_NOT_FOUND_ERR);
    return;
  }
  IdT id_1, id_2;
  double measure, distance;
  char buffer[MAX_LEN_OF_LINE] = {0}, *token = NULL;
  int person1_index, person2_index;
  while (fgets(buffer, MAX_LEN_OF_LINE, fd) != NULL) {
    token = strtok(buffer, DELIMITERS);
    id_1 = strtol(token, NULL, 10);
    token = strtok(NULL, DELIMITERS);
    id_2 = strtol(token, NULL, 10);
    token = strtok(NULL, DELIMITERS);
    measure = (double) strtol(token, NULL, 10);
    token = strtok(NULL, DELIMITERS);
    distance = (double) strtol(token, NULL, 10);
    // get persons 1 and 2 locations by id
    for (int i = 0; i < spreader_detector->people_size; ++i) {
      if (spreader_detector->people[i]->id == id_1) {
        person1_index = i;
      } else if (spreader_detector->people[i]->id == id_2) {
        person2_index = i;
      }
    }
    if (id_1==id_2){
      continue;
    }
    Meeting *meeting = MeetingAlloc(spreader_detector->people[person1_index],
                                    spreader_detector->people[person2_index],
                                    measure,
                                    distance);
    SpreaderDetectorAddMeeting(spreader_detector, meeting);
  }
  fclose(fd);
}

int SpreaderDetectorPrintRecommendTreatmentToAll(SpreaderDetector *spreader_detector, const char *file_path) {
  if (!spreader_detector){
    return 0;
  }
  FILE *fd = fopen(file_path, FILE_OPEN_FOR_WRITING); // open the given file in read mode
  if (fd == NULL) { // check the file opened correctly
    fprintf(stderr, FILE_NOT_FOUND_ERR);
    return 0;
  }
  for (int i = 0; i < spreader_detector->people_size; ++i) {
    if (spreader_detector->people[i]->infection_rate > MEDICAL_SUPERVISION_THRESHOLD) {
      fprintf(fd, MEDICAL_SUPERVISION_THRESHOLD_MSG,
              spreader_detector->people[i]->name,
              spreader_detector->people[i]->id,
              spreader_detector->people[i]->age,
              spreader_detector->people[i]->infection_rate);
    } else if (spreader_detector->people[i]->infection_rate > REGULAR_QUARANTINE_THRESHOLD) {
      fprintf(fd, REGULAR_QUARANTINE_MSG,
              spreader_detector->people[i]->name,
              spreader_detector->people[i]->id,
              spreader_detector->people[i]->age,
              spreader_detector->people[i]->infection_rate);
    } else {
      fprintf(fd, CLEAN_MSG,
              spreader_detector->people[i]->name,
              spreader_detector->people[i]->id,
              spreader_detector->people[i]->age,
              spreader_detector->people[i]->infection_rate);
    }
  }
  return 1; // printed successfully
}

double SpreaderDetectorGetInfectionRateById(SpreaderDetector *spreader_detector, IdT id) {
  if (spreader_detector) {
    for (size_t i = 0; i < spreader_detector->people_size; ++i) {
      if (spreader_detector->people[i]->id == id) {
        return spreader_detector->people[i]->infection_rate;
      }
    }
  }
  return -1;
}






