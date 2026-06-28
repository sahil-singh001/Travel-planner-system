#include "trip.h"

int main()
{
    int choice;

    do
    {
        printf("\n=====================================\n");
        printf("       TRAVEL PLANNER SYSTEM\n");
        printf("=====================================\n");

        printf("\nTrip Operations\n");
        printf("1.  Create New Trip\n");
        printf("2.  Add Activity\n");
        printf("3.  Insert Activity (by date)\n");
        printf("4.  Delete Activity\n");
        printf("5.  Display Trip Plan\n");

        printf("\nNavigation Operations\n");
        printf("6.  Create Route\n");
        printf("7.  Add Direction\n");
        printf("8.  Insert Direction\n");
        printf("9.  Delete Direction\n");
        printf("10. Search Direction\n");
        printf("11. Update Direction\n");
        printf("12. Display Route\n");

        printf("\nOther Operations\n");
        printf("13. Find Path\n");
        printf("14. Detect Repeated Locations\n");
        printf("15. Sort by Cost\n");
        printf("16. Range Search\n");
        printf("17. Count Activities\n");

        printf("\nFile Operations\n");
        printf("18. Save Trip to File\n");
        printf("19. Load Trip from File\n");

        printf("\n0. Exit\n");

        printf("\nEnter Choice: ");
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                createTrip();
                break;

            case 2:
            case 3:
                addActivity();
                break;

            case 4:
            {
                int id;
                printf("Enter Trip ID to Delete: ");
                scanf("%d", &id);
                deleteTripByID(id);
                break;
            }

            case 5:
                displayTripPlan();
                break;

            case 6:
                createRoute();
                break;

            case 7:
            case 8:
                addDirection();
                break;

            case 9:
                deleteDirection();
                break;

            case 10:
                searchDirection();
                break;

            case 11:
                updateDirection();
                break;

            case 12:
                displayRoute();
                break;

            case 13:
                findPath();
                break;

            case 14:
                detectRepeatedLocations();
                break;

            case 15:
                sortByCost();
                break;

            case 16:
                rangeSearch();
                break;

            case 17:
                countActivities();
                break;

            case 18:
                saveToFile();
                break;

            case 19:
                loadFromFile();
                break;

            case 0:
                printf("\nExiting Program...\n");
                break;

            default:
                printf("\nInvalid Choice!\n");
        }

    } while(choice != 0);

    return 0;
}