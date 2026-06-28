#include "trip.h"

TripNode *tripRoot = NULL;

int visitedCount = 0;
char visited[200][50];

/*---------------------------------------------------------
    Creates a new B-Tree node
    leaf = 1  -> leaf node
    leaf = 0  -> internal node
---------------------------------------------------------*/
TripNode* createTripNode(int leaf)
{
    TripNode *node = malloc(sizeof(TripNode));

    if (node == NULL)
    {
        printf("Memory Allocation Failed!\n");
        exit(1);
    }

    node->leaf = leaf;     // mark node type
    node->n = 0;          // currently no keys stored

    /* Initialize all child pointers to NULL */
    for (int i = 0; i < MAX + 2; i++)
        node->child[i] = NULL;

    return node;
}


/*---------------------------------------------------------
    Splits full child parent->child[i]

    Example:
        Full child = [10 | 20 | 30]

        After split:
            parent gets 20
            left child  = [10]
            right child = [30]
---------------------------------------------------------*/
void splitChild(TripNode *parent, int i)
{
    TripNode *y = parent->child[i];              // full child
    TripNode *z = createTripNode(y->leaf);       // new sibling

    z->n = MIN;   // right side gets MIN keys

    /* Move right-half keys from y to z */
    for (int j = 0; j < MIN; j++)
        z->key[j] = y->key[j + MIN + 1];

    /* If internal node, move child pointers too */
    if (y->leaf == 0)
    {
        for (int j = 0; j <= MIN; j++)
            z->child[j] = y->child[j + MIN + 1];
    }

    /* Left child keeps only left half */
    y->n = MIN;

    /* Shift parent's child pointers right */
    for (int j = parent->n; j >= i + 1; j--)
        parent->child[j + 1] = parent->child[j];

    /* Attach new sibling */
    parent->child[i + 1] = z;

    /* Shift parent keys right */
    for (int j = parent->n - 1; j >= i; j--)
        parent->key[j + 1] = parent->key[j];

    /* Move middle key to parent */
    parent->key[i] = y->key[MIN];

    /* Parent now has one extra key */
    parent->n++;
}


/*---------------------------------------------------------
    Inserts trip into a node guaranteed NOT full
---------------------------------------------------------*/
void insertNonFull(TripNode *node, Trip t)
{
    int i = node->n - 1;

    /* Case 1: Leaf node -> insert directly */
    if (node->leaf)
    {
        /* Shift larger keys right */
        while (i >= 0 && strcmp(t.date, node->key[i].date) < 0)
        {
            node->key[i + 1] = node->key[i];
            i--;
        }

        /* Insert at correct position */
        node->key[i + 1] = t;
        node->n++;
    }
    else
    {
        /* Find child where key belongs */
        while (i >= 0 && strcmp(t.date, node->key[i].date) < 0)
            i--;

        i++;

        /* If chosen child is full, split first */
        if (node->child[i] != NULL && node->child[i]->n == MAX)
        {
            splitChild(node, i);

            /* Decide left or right child after split */
            if (strcmp(t.date, node->key[i].date) > 0)
                i++;
        }

        /* Recursive insertion */
        insertNonFull(node->child[i], t);
    }
}


/*---------------------------------------------------------
    Main insertion function

    Handles:
    1. Empty tree
    2. Full root
    3. Normal insertion
---------------------------------------------------------*/
void insertTrip(Trip t)
{
    /* Case 1: Tree empty */
    if (tripRoot == NULL)
    {
        tripRoot = createTripNode(1);   // root starts as leaf
        tripRoot->key[0] = t;
        tripRoot->n = 1;
        return;
    }

    /* Case 2: Root full */
    if (tripRoot->n == MAX)
    {
        TripNode *newRoot = createTripNode(0);

        /* Old root becomes child of new root */
        newRoot->child[0] = tripRoot;

        /* Split old root */
        splitChild(newRoot, 0);

        int i = 0;

        /* Choose correct child for insertion */
        if (strcmp(t.date, newRoot->key[0].date) > 0)
            i++;

        insertNonFull(newRoot->child[i], t);

        /* Update global root */
        tripRoot = newRoot;
    }
    else
    {
        /* Case 3: Root has space */
        insertNonFull(tripRoot, t);
    }
}

/*---------------------------------------------------------
    Display all trips in sorted order (Inorder Traversal)

    For B-Tree:
    child[0], key[0], child[1], key[1], ... child[n]

    This prints trips sorted by date if insertion key = date.
---------------------------------------------------------*/
void traverseTrips(TripNode *root)
{
    /* Base case: empty tree / empty branch */
    if (root == NULL)
        return;

    int i;

    /* Visit keys one by one */
    for (i = 0; i < root->n; i++)
    {
        /* First visit left child before current key */
        if (!root->leaf && root->child[i] != NULL)
            traverseTrips(root->child[i]);

        /* Print current trip details */
        printf("\nID: %d", root->key[i].tripID);
        printf("\nDate: %s", root->key[i].date);
        printf("\nActivity: %s", root->key[i].activity);
        printf("\nFrom: %s", root->key[i].start);
        printf("\nTo: %s", root->key[i].destination);
        printf("\nCost: %.2f", root->key[i].cost);
        printf("\n-----------------------------\n");
    }

    /* Visit last child after all keys */
    if (!root->leaf && root->child[i] != NULL)
        traverseTrips(root->child[i]);
}
/*---------------------------------------------------------
    Search Trip by ID in a B-Tree sorted by DATE

    Since tree is ordered by date, not tripID,
    we must traverse the whole tree.

    Returns:
        Pointer to Trip if found
        NULL if not found
---------------------------------------------------------*/
Trip* searchTrip(TripNode *root, int id)
{
    if (root == NULL)
        return NULL;

    int i;

    for (i = 0; i < root->n; i++)
    {
        /* Search left subtree first */
        if (!root->leaf)
        {
            Trip *found = searchTrip(root->child[i], id);
            if (found != NULL)
                return found;
        }

        /* Check current key */
        if (root->key[i].tripID == id)
            return &root->key[i];
    }

    /* Search last child */
    if (!root->leaf)
        return searchTrip(root->child[i], id);

    return NULL;
}

/* Create Navigation Node */
NavNode* createNavNode(int leaf)
{
    NavNode *node = (NavNode*)malloc(sizeof(NavNode));

    if(node == NULL)
    {
        printf("Memory Allocation Failed!\n");
        exit(1);
    }

    node->leaf = leaf;
    node->n = 0;

    for(int i = 0; i <= MAX; i++)
        node->child[i] = NULL;

    return node;
}

/* Split Full Navigation Child */
void splitNavChild(NavNode *parent, int i)
{
    NavNode *y = parent->child[i];
    NavNode *z = createNavNode(y->leaf);

    z->n = MIN;

    for(int j = 0; j < MIN; j++)
        z->key[j] = y->key[j + MIN + 1];

    if(!y->leaf)
    {
        for(int j = 0; j <= MIN; j++)
            z->child[j] = y->child[j + MIN + 1];
    }

    y->n = MIN;

    for(int j = parent->n; j >= i + 1; j--)
        parent->child[j + 1] = parent->child[j];

    parent->child[i + 1] = z;

    for(int j = parent->n - 1; j >= i; j--)
        parent->key[j + 1] = parent->key[j];

    parent->key[i] = y->key[MIN];
    parent->n++;
}

/* Insert in Non Full Nav Node */
void insertNavNonFull(NavNode *node, NavStep s)
{
    int i = node->n - 1;

    if(node->leaf)
    {
        while(i >= 0 && s.stepID < node->key[i].stepID)
        {
            node->key[i + 1] = node->key[i];
            i--;
        }

        node->key[i + 1] = s;
        node->n++;
    }
    else
    {
        while(i >= 0 && s.stepID < node->key[i].stepID)
            i--;

        i++;

        if(node->child[i]->n == MAX)
        {
            splitNavChild(node, i);

            if(s.stepID > node->key[i].stepID)
                i++;
        }

        insertNavNonFull(node->child[i], s);
    }
}

/* Main Insert Navigation */
void insertNav(NavNode **root, NavStep s)
{
    if(*root == NULL)
    {
        *root = createNavNode(1);
        (*root)->key[0] = s;
        (*root)->n = 1;
        return;
    }

    if((*root)->n == MAX)
    {
        NavNode *temp = createNavNode(0);
        temp->child[0] = *root;

        splitNavChild(temp, 0);

        int i = 0;

        if(s.stepID > temp->key[0].stepID)
            i++;

        insertNavNonFull(temp->child[i], s);

        *root = temp;
    }
    else
    {
        insertNavNonFull(*root, s);
    }
}

/* Display Route */
void traverseNav(NavNode *root)
{
    if(root == NULL)
        return;

    int i;

    for(i = 0; i < root->n; i++)
    {
        if(!root->leaf)
            traverseNav(root->child[i]);

        printf("\nStep ID: %d", root->key[i].stepID);
        printf("\nDirection: %s", root->key[i].direction);
        printf("\nDistance: %.2f km\n", root->key[i].distance);
        printf("----------------------\n");
    }

    if(!root->leaf)
        traverseNav(root->child[i]);
}

/* Search Specific Direction Text */
int searchDirectionText(NavNode *root, char key[])
{
    if(root == NULL)
        return 0;

    for(int i = 0; i < root->n; i++)
    {
        if(strcmp(root->key[i].direction, key) == 0)
            return 1;
    }

    if(root->leaf)
        return 0;

    for(int i = 0; i <= root->n; i++)
    {
        if(searchDirectionText(root->child[i], key))
            return 1;
    }

    return 0;
}

/* Create Fresh Trip */
void createTrip()
{
    tripRoot = NULL;
    printf("\nNew Trip Initialized Successfully!\n");
}

/* Add Activity */
void addActivity()
{
    Trip t;

    printf("\nEnter Trip ID: ");
    scanf("%d", &t.tripID);

    printf("Enter Date (YYYY-MM-DD): ");
    scanf("%s", t.date);

    printf("Enter Activity: ");
    scanf(" %[^\n]", t.activity);

    printf("Enter Start Location: ");
    scanf(" %[^\n]", t.start);

    printf("Enter Destination: ");
    scanf(" %[^\n]", t.destination);

    printf("Enter Cost: ");
    scanf("%f", &t.cost);

    t.navRoot = NULL;

    insertTrip(t);

    printf("\nActivity Added Successfully!\n");
}

/* Display Full Trip */
void displayTripPlan()
{
    if(tripRoot == NULL)
    {
        printf("\nNo Trip Data Available!\n");
        return;
    }

    printf("\n========== TRIP PLAN ==========\n");
    traverseTrips(tripRoot);
}

/* Create Route for Specific Trip */
void createRoute()
{
    int id;

    printf("\nEnter Trip ID: ");
    scanf("%d", &id);

    Trip *p = searchTrip(tripRoot, id);

    if(p == NULL)
    {
        printf("\nTrip Not Found!\n");
        return;
    }

    p->navRoot = NULL;

    printf("\nEmpty Route Created for Trip ID %d\n", id);
}

/* Add Direction to Route */
void addDirection()
{
    int id;
    NavStep s;

    printf("\nEnter Trip ID: ");
    scanf("%d", &id);

    Trip *p = searchTrip(tripRoot, id);

    if(p == NULL)
    {
        printf("\nTrip Not Found!\n");
        return;
    }

    printf("Enter Step ID: ");
    scanf("%d", &s.stepID);

    printf("Enter Direction: ");
    scanf(" %[^\n]", s.direction);

    printf("Enter Distance (km): ");
    scanf("%f", &s.distance);

    insertNav(&(p->navRoot), s);

    printf("\nDirection Added Successfully!\n");
}

/* Display Route */
void displayRoute()
{
    int id;

    printf("\nEnter Trip ID: ");
    scanf("%d", &id);

    Trip *p = searchTrip(tripRoot, id);

    if(p == NULL)
    {
        printf("\nTrip Not Found!\n");
        return;
    }

    if(p->navRoot == NULL)
    {
        printf("\nNo Route Available!\n");
        return;
    }

    printf("\n====== ROUTE FOR TRIP %d ======\n", id);
    traverseNav(p->navRoot);
}

/* Count Total Activities */
int countTripNodes(TripNode *root)
{
    if(root == NULL)
        return 0;

    int count = root->n;

    if(!root->leaf)
    {
        for(int i = 0; i <= root->n; i++)
            count += countTripNodes(root->child[i]);
    }

    return count;
}

void countActivities()
{
    printf("\nTotal Activities = %d\n", countTripNodes(tripRoot));
}

/* Range Search by Date */
void rangeSearchUtil(TripNode *root, char d1[], char d2[])
{
    if(root == NULL)
        return;

    int i;

    for(i = 0; i < root->n; i++)
    {
        if(!root->leaf)
            rangeSearchUtil(root->child[i], d1, d2);

        if(strcmp(root->key[i].date, d1) >= 0 &&
           strcmp(root->key[i].date, d2) <= 0)
        {
            printf("\nID: %d", root->key[i].tripID);
            printf("\nDate: %s", root->key[i].date);
            printf("\n%s : %s -> %s\n",
                   root->key[i].activity,
                   root->key[i].start,
                   root->key[i].destination);
            printf("-------------------\n");
        }
    }

    if(!root->leaf)
        rangeSearchUtil(root->child[i], d1, d2);
}

void rangeSearch()
{
    char d1[20], d2[20];

    printf("\nEnter Start Date: ");
    scanf("%s", d1);

    printf("Enter End Date: ");
    scanf("%s", d2);

    printf("\n===== RANGE RESULTS =====\n");

    rangeSearchUtil(tripRoot, d1, d2);
}

/* Detect Repeated Locations */
void repeatedUtil(TripNode *root, char place[])
{
    if(root == NULL)
        return;

    int i;

    for(i = 0; i < root->n; i++)
    {
        if(!root->leaf)
            repeatedUtil(root->child[i], place);

        if(strcmp(root->key[i].start, place) == 0 ||
           strcmp(root->key[i].destination, place) == 0)
            printf(".");
    }

    if(!root->leaf)
        repeatedUtil(root->child[i], place);
}

void detectRepeatedLocations()
{
    if(tripRoot == NULL)
    {
        printf("\nNo trip data available!\n");
        return;
    }

    printf("\n===== REPEATED LOCATIONS =====\n");

    int foundAny = 0;

    detectRepeatedUtil(tripRoot, tripRoot, &foundAny);

    if(!foundAny)
        printf("No repeated locations found.\n");
}


/* Traverse main tree and check each location against whole B-tree */
void detectRepeatedUtil(TripNode *node, TripNode *root, int *foundAny)
{
    if(node == NULL)
        return;

    int i;

    for(i = 0; i < node->n; i++)
    {
        if(!node->leaf)
            detectRepeatedUtil(node->child[i], root, foundAny);

        /* check start location */
        if(isFirstOccurrence(root, node->key[i].start, node->key[i].tripID))
        {
            int c = countLocation(root, node->key[i].start);

            if(c > 1)
            {
                printf("%s visited %d times\n", node->key[i].start, c);
                *foundAny = 1;
            }
        }

        /* check destination location */
        if(isFirstOccurrence(root, node->key[i].destination, node->key[i].tripID))
        {
            int c = countLocation(root, node->key[i].destination);

            if(c > 1)
            {
                printf("%s visited %d times\n", node->key[i].destination, c);
                *foundAny = 1;
            }
        }
    }

    if(!node->leaf)
        detectRepeatedUtil(node->child[i], root, foundAny);
}


/* Count how many times a place appears in full B-tree */
int countLocation(TripNode *node, char place[])
{
    if(node == NULL)
        return 0;

    int total = 0;
    int i;

    for(i = 0; i < node->n; i++)
    {
        if(!node->leaf)
            total += countLocation(node->child[i], place);

        if(strcmp(node->key[i].start, place) == 0)
            total++;

        if(strcmp(node->key[i].destination, place) == 0)
            total++;
    }

    if(!node->leaf)
        total += countLocation(node->child[i], place);

    return total;
}


/* Print location only first time encountered */
int isFirstOccurrence(TripNode *node, char place[], int currentID)
{
    if(node == NULL)
        return 1;

    int i;

    for(i = 0; i < node->n; i++)
    {
        if(!node->leaf)
        {
            if(!isFirstOccurrence(node->child[i], place, currentID))
                return 0;
        }

        if(node->key[i].tripID == currentID)
            return 1;

        if(strcmp(node->key[i].start, place) == 0 ||
           strcmp(node->key[i].destination, place) == 0)
            return 0;
    }

    if(!node->leaf)
        return isFirstOccurrence(node->child[i], place, currentID);

    return 1;
}

/* ---------- Store Trips into Array for Sorting ---------- */

int idx = 0;

void fillArray(TripNode *root, Trip arr[])
{
    if(root == NULL)
        return;

    int i;

    for(i = 0; i < root->n; i++)
    {
        if(!root->leaf)
            fillArray(root->child[i], arr);

        arr[idx++] = root->key[i];
    }

    if(!root->leaf)
        fillArray(root->child[i], arr);
}

/*---------------------------------------------------------
    Merge two sorted parts of Trip array
    Sort by Cost : High to Low
---------------------------------------------------------*/
void mergeTrips(Trip arr[], int left, int mid, int right)
{
    int n1 = mid - left + 1;
    int n2 = right - mid;

    Trip L[200], R[200];

    int i, j, k;

    /* Copy left part */
    for(i = 0; i < n1; i++)
        L[i] = arr[left + i];

    /* Copy right part */
    for(j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = left;

    /* Merge in descending cost order */
    while(i < n1 && j < n2)
    {
        if(L[i].cost >= R[j].cost)
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    /* Remaining left elements */
    while(i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    /* Remaining right elements */
    while(j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}


/*---------------------------------------------------------
    Merge Sort Function
---------------------------------------------------------*/
void mergeSortTrips(Trip arr[], int left, int right)
{
    if(left < right)
    {
        int mid = (left + right) / 2;

        mergeSortTrips(arr, left, mid);
        mergeSortTrips(arr, mid + 1, right);

        mergeTrips(arr, left, mid, right);
    }
}


/*---------------------------------------------------------
    Sort Trips by Cost using Merge Sort
---------------------------------------------------------*/
void sortByCost()
{
    int total = countTripNodes(tripRoot);

    if(total == 0)
    {
        printf("\nNo Activities Available!\n");
        return;
    }

    Trip arr[200];

    idx = 0;
    fillArray(tripRoot, arr);

    /* Apply Merge Sort */
    mergeSortTrips(arr, 0, total - 1);

    printf("\n===== SORTED BY COST (High to Low) =====\n");

    for(int i = 0; i < total; i++)
    {
        printf("\nID: %d", arr[i].tripID);
        printf("\nActivity: %s", arr[i].activity);
        printf("\nRoute: %s -> %s",
               arr[i].start,
               arr[i].destination);
        printf("\nDate: %s", arr[i].date);
        printf("\nCost: ₹%.2f", arr[i].cost);
        printf("\n-----------------------------\n");
    }
}

/* =====================================================
   DUAL FILE SAVE / LOAD SYSTEM
   Files Used:
   1. tripdata.txt
   2. routedata.txt
   ===================================================== */


void saveTripsUtil(TripNode *root, FILE *fp)
{
    if(root == NULL)
        return;

    int i;

    for(i = 0; i < root->n; i++)
    {
        if(!root->leaf)
            saveTripsUtil(root->child[i], fp);

        fprintf(fp,
        "%d|%s|%s|%s|%s|%.2f\n",
        root->key[i].tripID,
        root->key[i].date,
        root->key[i].activity,
        root->key[i].start,
        root->key[i].destination,
        root->key[i].cost);
    }

    if(!root->leaf)
        saveTripsUtil(root->child[i], fp);
}
/* ---------- SAVE ALL ROUTES ---------- */
void saveRoutesUtil(TripNode *root, FILE *fp)
{
    if(root == NULL)
        return;

    int i;

    for(i = 0; i < root->n; i++)
    {
        if(!root->leaf)
            saveRoutesUtil(root->child[i], fp);

        saveSingleRoute(root->key[i].tripID,
                        root->key[i].navRoot,
                        fp);
    }

    if(!root->leaf)
        saveRoutesUtil(root->child[i], fp);
}


/* Save one trip's nav tree */
void saveSingleRoute(int tripID, NavNode *navRoot, FILE *fp)
{
    if(navRoot == NULL)
        return;

    int i;

    for(i = 0; i < navRoot->n; i++)
    {
        if(!navRoot->leaf)
            saveSingleRoute(tripID,
                            navRoot->child[i],
                            fp);

        fprintf(fp,
        "%d|%d|%s|%.2f\n",
        tripID,
        navRoot->key[i].stepID,
        navRoot->key[i].direction,
        navRoot->key[i].distance);
    }

    if(!navRoot->leaf)
        saveSingleRoute(tripID,
                        navRoot->child[i],
                        fp);
}


/* ---------- SAVE COMPLETE DATA ---------- */
void saveToFile()
{
    FILE *fp1 = fopen("tripdata.txt", "w");
    FILE *fp2 = fopen("routedata.txt", "w");

    if(fp1 == NULL || fp2 == NULL)
    {
        printf("\nFile Error!\n");
        return;
    }

    /* Save Trips */
    saveTripsUtil(tripRoot, fp1);

    /* Save Routes */
    saveRoutesUtil(tripRoot, fp2);

    fclose(fp1);
    fclose(fp2);

    printf("\nTrips + Routes Saved Successfully!\n");
}



/* =====================================================
                    LOAD SYSTEM
   ===================================================== */

void loadFromFile()
{
    FILE *fp1 = fopen("tripdata.txt", "r");
    FILE *fp2 = fopen("routedata.txt", "r");

    if(fp1 == NULL)
    {
        printf("\ntripdata.txt not found!\n");
        return;
    }

    Trip t;

    /* Reset Tree */
    tripRoot = NULL;

    /* ---------- LOAD TRIPS ---------- */
    while(fscanf(fp1,
    "%d|%[^|]|%[^|]|%[^|]|%[^|]|%f\n",
    &t.tripID,
    t.date,
    t.activity,
    t.start,
    t.destination,
    &t.cost) != EOF)
    {
        t.navRoot = NULL;
        insertTrip(t);
    }

    fclose(fp1);

    /* ---------- LOAD ROUTES ---------- */
    if(fp2 != NULL)
    {
        int tripID;
        NavStep s;

        while(fscanf(fp2,
        "%d|%d|%[^|]|%f\n",
        &tripID,
        &s.stepID,
        s.direction,
        &s.distance) != EOF)
        {
            Trip *p = searchTrip(tripRoot, tripID);

            if(p != NULL)
                insertNav(&(p->navRoot), s);
        }

        fclose(fp2);
    }

    printf("\nTrips + Routes Loaded Successfully!\n");
}




/*---------------------------------------------------------
    Free complete B-tree memory
---------------------------------------------------------*/
void freeTripTree(TripNode *root)
{
    if (root == NULL)
        return;

    /* Free child nodes first */
    if (!root->leaf)
    {
        for (int i = 0; i <= root->n; i++)
            freeTripTree(root->child[i]);
    }

    free(root);
}


/*---------------------------------------------------------
    Delete Trip By ID

    Works safely even though tree is ordered by DATE.
---------------------------------------------------------*/
void deleteTripByID(int id)
{
    /* Tree empty */
    if (tripRoot == NULL)
    {
        printf("\nNo Trips Available!\n");
        return;
    }

    /* Count total trips */
    int total = countTripNodes(tripRoot);

    /* Temporary arrays */
    Trip allTrips[500];
    Trip remaining[500];

    /* Copy all trips from tree */
    idx = 0;
    fillArray(tripRoot, allTrips);

    int found = 0;
    int newCount = 0;

    /* Keep all except matching ID */
    for (int i = 0; i < total; i++)
    {
        if (allTrips[i].tripID == id)
        {
            found = 1;   /* trip to delete found */
        }
        else
        {
            remaining[newCount++] = allTrips[i];
        }
    }

    /* If ID not found */
    if (!found)
    {
        printf("\nTrip ID %d Not Found!\n", id);
        return;
    }

    /* Delete old tree */
    freeTripTree(tripRoot);
    tripRoot = NULL;

    /* Rebuild fresh tree */
    for (int i = 0; i < newCount; i++)
        insertTrip(remaining[i]);

    printf("\nTrip ID %d Deleted Successfully!\n", id);
}


/* Check visited city */
int isVisited(char place[])
{
    for(int i = 0; i < visitedCount; i++)
    {
        if(strcmp(visited[i], place) == 0)
            return 1;
    }
    return 0;
}

/* Mark visited */
void markVisited(char place[])
{
    strcpy(visited[visitedCount], place);
    visitedCount++;
}

/* Recursive path finder through B-tree */
int pathDFS(TripNode *node, char src[], char dest[])
{
    if(node == NULL)
        return 0;

    int i;

    for(i = 0; i < node->n; i++)
    {
        if(!node->leaf)
        {
            if(pathDFS(node->child[i], src, dest))
                return 1;
        }

        if(strcmp(node->key[i].start, src) == 0)
        {
            printf("%s -> %s (%s)\n",
                   node->key[i].start,
                   node->key[i].destination,
                   node->key[i].activity);

            if(strcmp(node->key[i].destination, dest) == 0)
                return 1;

            if(!isVisited(node->key[i].destination))
            {
                markVisited(node->key[i].destination);

                if(pathDFS(tripRoot,
                           node->key[i].destination,
                           dest))
                    return 1;
            }
        }
    }

    if(!node->leaf)
    {
        if(pathDFS(node->child[i], src, dest))
            return 1;
    }

    return 0;
}

/* Main function */
void findPath()
{
    char src[50], dest[50];

    printf("\nEnter Source: ");
    scanf(" %[^\n]", src);

    printf("Enter Destination: ");
    scanf(" %[^\n]", dest);

    visitedCount = 0;
    markVisited(src);

    printf("\n===== PATH RESULT =====\n");

    if(!pathDFS(tripRoot, src, dest))
        printf("No path found.\n");
}

NavStep* searchNavByStepID(NavNode *root, int id)
{
    if(root == NULL)
        return NULL;

    int i = 0;

    while(i < root->n && id > root->key[i].stepID)
        i++;

    if(i < root->n && id == root->key[i].stepID)
        return &root->key[i];

    if(root->leaf)
        return NULL;

    return searchNavByStepID(root->child[i], id);
}

void updateDirection()
{
    int tripID, stepID;

    printf("\nEnter Trip ID: ");
    scanf("%d", &tripID);

    Trip *p = searchTrip(tripRoot, tripID);

    if(p == NULL)
    {
        printf("Trip not found.\n");
        return;
    }

    printf("Enter Step ID: ");
    scanf("%d", &stepID);

    NavStep *s = searchNavByStepID(p->navRoot, stepID);

    if(s == NULL)
    {
        printf("Step not found.\n");
        return;
    }

    printf("Enter New Direction: ");
    scanf(" %[^\n]", s->direction);

    printf("Enter New Distance: ");
    scanf("%f", &s->distance);

    printf("Direction updated successfully.\n");
}

void searchDirection()
{
    int tripID, stepID;

    printf("\nEnter Trip ID: ");
    scanf("%d", &tripID);

    Trip *p = searchTrip(tripRoot, tripID);

    if(p == NULL)
    {
        printf("\nTrip not found.\n");
        return;
    }

    if(p->navRoot == NULL)
    {
        printf("\nNo directions added for this trip.\n");
        return;
    }

    printf("Enter Step ID: ");
    scanf("%d", &stepID);

    NavStep *s = searchNavByStepID(p->navRoot, stepID);

    if(s == NULL)
    {
        printf("\nDirection not found.\n");
        return;
    }

    printf("\nStep ID: %d", s->stepID);
    printf("\nDirection: %s", s->direction);
    printf("\nDistance: %.2f km\n", s->distance);
}
/* ---------- NAVIGATION B-TREE DELETE ---------- */

int findKeyNav(NavNode *node, int id)
{
    int idx = 0;

    while(idx < node->n && node->key[idx].stepID < id)
        idx++;

    return idx;
}

void removeFromLeafNav(NavNode *node, int idx)
{
    for(int i = idx + 1; i < node->n; i++)
        node->key[i - 1] = node->key[i];

    node->n--;
}

NavStep getPredNav(NavNode *node, int idx)
{
    NavNode *cur = node->child[idx];

    while(!cur->leaf)
        cur = cur->child[cur->n];

    return cur->key[cur->n - 1];
}

NavStep getSuccNav(NavNode *node, int idx)
{
    NavNode *cur = node->child[idx + 1];

    while(!cur->leaf)
        cur = cur->child[0];

    return cur->key[0];
}

void mergeNav(NavNode *node, int idx)
{
    NavNode *child = node->child[idx];
    NavNode *sib   = node->child[idx + 1];

    child->key[MIN] = node->key[idx];

    for(int i = 0; i < sib->n; i++)
        child->key[i + MIN + 1] = sib->key[i];

    if(!child->leaf)
    {
        for(int i = 0; i <= sib->n; i++)
            child->child[i + MIN + 1] = sib->child[i];
    }

    for(int i = idx + 1; i < node->n; i++)
        node->key[i - 1] = node->key[i];

    for(int i = idx + 2; i <= node->n; i++)
        node->child[i - 1] = node->child[i];

    child->n += sib->n + 1;
    node->n--;

    free(sib);
}

void borrowFromPrevNav(NavNode *node, int idx)
{
    NavNode *child = node->child[idx];
    NavNode *sib   = node->child[idx - 1];

    for(int i = child->n - 1; i >= 0; i--)
        child->key[i + 1] = child->key[i];

    if(!child->leaf)
    {
        for(int i = child->n; i >= 0; i--)
            child->child[i + 1] = child->child[i];
    }

    child->key[0] = node->key[idx - 1];

    if(!child->leaf)
        child->child[0] = sib->child[sib->n];

    node->key[idx - 1] = sib->key[sib->n - 1];

    child->n++;
    sib->n--;
}

void borrowFromNextNav(NavNode *node, int idx)
{
    NavNode *child = node->child[idx];
    NavNode *sib   = node->child[idx + 1];

    child->key[child->n] = node->key[idx];

    if(!child->leaf)
        child->child[child->n + 1] = sib->child[0];

    node->key[idx] = sib->key[0];

    for(int i = 1; i < sib->n; i++)
        sib->key[i - 1] = sib->key[i];

    if(!sib->leaf)
    {
        for(int i = 1; i <= sib->n; i++)
            sib->child[i - 1] = sib->child[i];
    }

    child->n++;
    sib->n--;
}

void fillNav(NavNode *node, int idx)
{
    if(idx != 0 && node->child[idx - 1]->n > MIN)
        borrowFromPrevNav(node, idx);

    else if(idx != node->n && node->child[idx + 1]->n > MIN)
        borrowFromNextNav(node, idx);

    else
    {
        if(idx != node->n)
            mergeNav(node, idx);
        else
            mergeNav(node, idx - 1);
    }
}

void removeFromNonLeafNav(NavNode *node, int idx)
{
    int id = node->key[idx].stepID;

    if(node->child[idx]->n > MIN)
    {
        NavStep pred = getPredNav(node, idx);
        node->key[idx] = pred;
        removeNavNode(node->child[idx], pred.stepID);
    }
    else if(node->child[idx + 1]->n > MIN)
    {
        NavStep succ = getSuccNav(node, idx);
        node->key[idx] = succ;
        removeNavNode(node->child[idx + 1], succ.stepID);
    }
    else
    {
        mergeNav(node, idx);
        removeNavNode(node->child[idx], id);
    }
}

void removeNavNode(NavNode *node, int id)
{
    int idx = findKeyNav(node, id);

    if(idx < node->n && node->key[idx].stepID == id)
    {
        if(node->leaf)
            removeFromLeafNav(node, idx);
        else
            removeFromNonLeafNav(node, idx);
    }
    else
    {
        if(node->leaf)
        {
            printf("Direction not found.\n");
            return;
        }

        int flag = (idx == node->n);

        if(node->child[idx]->n == MIN)
            fillNav(node, idx);

        if(flag && idx > node->n)
            removeNavNode(node->child[idx - 1], id);
        else
            removeNavNode(node->child[idx], id);
    }
}

void deleteDirection()
{
    int tripID, stepID;

    printf("\nEnter Trip ID: ");
    scanf("%d", &tripID);

    Trip *p = searchTrip(tripRoot, tripID);

    if(p == NULL)
    {
        printf("Trip not found.\n");
        return;
    }

    printf("Enter Step ID to delete: ");
    scanf("%d", &stepID);

    if(p->navRoot == NULL)
    {
        printf("No route exists.\n");
        return;
    }

    removeNavNode(p->navRoot, stepID);

    if(p->navRoot->n == 0)
    {
        NavNode *tmp = p->navRoot;

        if(p->navRoot->leaf)
            p->navRoot = NULL;
        else
            p->navRoot = p->navRoot->child[0];

        free(tmp);
    }

    printf("Direction deleted if existed.\n");
}

