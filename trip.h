#ifndef TRIP_H
#define TRIP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------- B-Tree Limits ---------- */
#define MAX 3
#define MIN 1

/* ---------- Forward Declarations ---------- */
typedef struct TripNode TripNode;
typedef struct NavNode NavNode;

/* ---------- Navigation Step ---------- */
typedef struct NavStep
{
    int stepID;
    char direction[100];
    float distance;

} NavStep;

/* ---------- Trip Record ---------- */
typedef struct Trip
{
    int tripID;
    char date[20];
    char activity[50];
    char start[50];
    char destination[50];
    float cost;

    NavNode *navRoot;

} Trip;

/* ---------- Trip B-Tree Node ---------- */
struct TripNode
{
    int n;
    int leaf;

    Trip key[MAX+1];
    TripNode *child[MAX + 2];
};

/* ---------- Navigation B-Tree Node ---------- */
struct NavNode
{
    int n;
    int leaf;

    NavStep key[MAX+1];
    NavNode *child[MAX + 2];
};

/* ---------- Global Variables ---------- */
extern TripNode *tripRoot;

extern int visitedCount;
extern char visited[200][50];

/* =======================================================
                    TRIP TREE FUNCTIONS
   ======================================================= */

TripNode* createTripNode(int leaf);

void splitChild(TripNode *parent, int i);
void insertNonFull(TripNode *node, Trip t);
void insertTrip(Trip t);

void traverseTrips(TripNode *root);
Trip* searchTrip(TripNode *root, int id);

int countTripNodes(TripNode *root);

/* ---------- Trip Delete ---------- */
void deleteTripByID(int id);
void removeTripNode(TripNode *node, int id);

int findKeyTrip(TripNode *node, int id);

Trip getPredTrip(TripNode *node, int idx);
Trip getSuccTrip(TripNode *node, int idx);

void removeFromLeafTrip(TripNode *node, int idx);
void removeFromNonLeafTrip(TripNode *node, int idx);

void fillTrip(TripNode *node, int idx);
void borrowFromPrevTrip(TripNode *node, int idx);
void borrowFromNextTrip(TripNode *node, int idx);
void mergeTrip(TripNode *node, int idx);

/* =======================================================
                 NAVIGATION TREE FUNCTIONS
   ======================================================= */

NavNode* createNavNode(int leaf);

void splitNavChild(NavNode *parent, int i);
void insertNavNonFull(NavNode *node, NavStep s);
void insertNav(NavNode **root, NavStep s);

void traverseNav(NavNode *root);

NavStep* searchNavByStepID(NavNode *root, int id);
int searchDirectionText(NavNode *root, char key[]);

/* ---------- Navigation Delete ---------- */
void removeNavNode(NavNode *node, int id);

int findKeyNav(NavNode *node, int id);

NavStep getPredNav(NavNode *node, int idx);
NavStep getSuccNav(NavNode *node, int idx);

void removeFromLeafNav(NavNode *node, int idx);
void removeFromNonLeafNav(NavNode *node, int idx);

void fillNav(NavNode *node, int idx);
void borrowFromPrevNav(NavNode *node, int idx);
void borrowFromNextNav(NavNode *node, int idx);
void mergeNav(NavNode *node, int idx);

/* =======================================================
                  USER LEVEL FUNCTIONS
   ======================================================= */

/* Trip Operations */
void createTrip();
void addActivity();
void displayTripPlan();
void rangeSearch();
void countActivities();
void sortByCost();

/* Navigation Operations */
void createRoute();
void addDirection();
void displayRoute();
void searchDirection();
void updateDirection();
void deleteDirection();

/* Other Operations */
void findPath();
int pathDFS(TripNode *node, char src[], char dest[]);
int isVisited(char place[]);
void markVisited(char place[]);

void detectRepeatedLocations();
void detectRepeatedUtil(TripNode *node, TripNode *root, int *foundAny);
int countLocation(TripNode *node, char place[]);
int isFirstOccurrence(TripNode *node, char place[], int currentID);

/* File Operations */
void saveToFile();
void loadFromFile();
void saveTripsUtil(TripNode *root, FILE *fp);
void saveRoutesUtil(TripNode *root, FILE *fp);
void saveSingleRoute(int tripID, NavNode *navRoot, FILE *fp);
/* Utility */
void fillArray(TripNode *root, Trip arr[]);
void collectTrips(TripNode *root, Trip arr[], int *total);

#endif