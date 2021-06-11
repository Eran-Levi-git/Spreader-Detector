#include <stdio.h>
#include "SpreaderDetector.h"

int main() {
  SpreaderDetector *spr = SpreaderDetectorAlloc();
  SpreaderDetectorReadPeopleFile(spr, "/Users/eranlevi/CLionProjects/ex3/people_file.in");
  SpreaderDetectorReadMeetingsFile(spr, "/Users/eranlevi/CLionProjects/ex3/meetings_file.in");
  char *ch=(spr->meetings[5]->person_1->name);
  SpreaderDetectorCalculateInfectionChances(spr);
  SpreaderDetectorPrintRecommendTreatmentToAll(spr, "output_file.out");
//   free meeting
  for (size_t k_i = 0; k_i < spr->meeting_size; ++k_i) {
    MeetingFree(&spr->meetings[k_i]);
  }
//   free people
  for (size_t k_j = 0; k_j < spr->people_size; ++k_j) {
    PersonFree(&spr->people[k_j]);
  }
  SpreaderDetectorFree(&spr);




//  SpreaderDetector *spr = SpreaderDetectorAlloc();
//  Person *p1 = PersonAlloc(306356321, "Ofira", 53, 0);
//  Person *p2 = PersonAlloc(201336708, "Berko", 50, 0);
//  SpreaderDetectorAddPerson(spr, p1);
//  SpreaderDetectorAddPerson(spr, p2);
//  Meeting *meeting = MeetingAlloc(p1,p2,10.1,10);
//  SpreaderDetectorAddMeeting(spr, meeting);
//  size_t meetings_num = SpreaderDetectorGetNumOfMeetings(spr); // returns 1
//  printf("%lu",meetings_num);
//  PersonFree(&p1);
//  PersonFree(&p2);
//  MeetingFree(&meeting);
//  SpreaderDetectorFree(&spr);

  return 0;
}