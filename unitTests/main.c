typedef int testobj;
#include "listHeaders/testobjList.h"
#include "listCode/testobjList.inc"
#include <assert.h>

#include "testMallocTracker.h"

static void testAddRm();
static void testAddEnd();
static void testSearch();
static void testUpsert();
int main()
{
    testAddRm();
    testAddEnd();
    testSearch();
    testUpsert();
}

static int listEqExpected(const testobjList * list, const testobj * expected, int expected_len);
static void testAddRm()
{
    allocatorTrackerReset();

    testobjList * list = NULL;
    testobj listSource[] = {2,4,6,8,10};
    testobj listSourceCp[] = {2,4,6,8,10};
    int listSourceSize = sizeof(listSource)/sizeof(listSource[0]);

    list = testobjListAdd(list, &listSource[listSourceSize-1]); 

    if (list == 0)
    {
        printf("testAddRm(): list pointer not set after adding to list\n");
        exit(1);
    }

    if (allocatorTrackerGetBytesAllocated() == 0)
    {
        printf("testAddRm(): no memory allocated after adding to list\n");
        exit(1);
    }

    if (!listEqExpected(list, &listSource[listSourceSize-1], 1))
    {
        printf("testAddRm(): list not as expected\n");
        exit(1);
    }

    int i;
    for (i = listSourceSize-2; i >= 0; i--)
    {
        list = testobjListAdd(list, &listSourceCp[i]); 
    }

    if (!listEqExpected(list, listSource, listSourceSize))
    {
        printf("testAddRm(): list not as expected (2)\n");
        exit(1);
    }

    testobj * valp;
    list = testobjListRm(list, &listSource[2], testobjCmp, &valp);
    if (*valp != listSource[2])
    {
        printf("testAddRm(): removal from middle of list "
                "returns incorrect removed value\n");
        exit(1);
    }

    memmove(&listSource[2], &listSource[2+1], (listSourceSize-(2+1))*sizeof(listSource[0]));
    listSourceSize--;
    if (!listEqExpected(list, listSource, listSourceSize))
    {
        printf("testAddRm(): removal from middle of list "
                "list not as expected\n");
        assert(0);
    }

    list = testobjListRm(list, &listSource[0], testobjCmp, &valp);
    if (*valp != listSource[0])
    {
        printf("testAddRm(): removal from front of list "
                "returns incorrect removed value\n");
        exit(1);
    }

    memmove(&listSource[0], &listSource[0+1], (listSourceSize-(0+1))*sizeof(listSource[0]));
    listSourceSize--;
    if (!listEqExpected(list, listSource, listSourceSize))
    {
        printf("testAddRm(): removal from front of list "
                "list not as expected\n");
        assert(0);
    }

    list = testobjListRm(list, &listSource[listSourceSize-1], testobjCmp, &valp);
    if (*valp != listSource[listSourceSize-1])
    {
        printf("testAddRm(): removal from front of list "
                "returns incorrect removed value\n");
        assert(0);
    }

    listSourceSize--;
    if (!listEqExpected(list, listSource, listSourceSize))
    {
        printf("testAddRm(): removal from end of list "
                "list not as expected\n");
        assert(0);
    }

    for (i = 0; i < listSourceSize; i++)
    {
        list = testobjListRm(
                list,
                &listSource[i],
                testobjCmp, NULL);
    }

    if (list)
    {
        printf("testAddRm(): after removing all list elements"
                " list is non-null\n");
        assert(0);
    }

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("testAddRm(): after removing all list elements there is "
                "a memory leak\n");
        assert(0);
    }
}

static void testAddEnd()
{
    testobjList * list = NULL;
    testobj listSource[] = {3,9,12,15};
    int listSourceSize = sizeof(listSource)/sizeof(listSource[0]);
    int i;
    for (i = 0; i < listSourceSize; i++)
    {
        list = testobjListAddToEnd(list, &listSource[i]);
    }

    if (!listEqExpected(list, listSource, listSourceSize))
    {
        printf("testAddEnd(): list not as expected when addToEnd() called\n");
        assert(0);
    }

    testobjList * l;
    for (l = list; l;)
    {
        testobjList * lt = l;
        l = l->next;
        free(lt);
    }
}

static void testSearch()
{
    testobjList * list = NULL;
    testobj listSource[] = {2,3,5,7,13};
    int listSourceSize = sizeof(listSource)/sizeof(listSource[0]);

    int i;
    for (i = 0; i < listSourceSize; i++)
    {
        list = testobjListAdd(list, &listSource[i]);
    }

    testobjList * res;
    res = testobjListSearch(list, &listSource[0], testobjCmp);
    if (*res->val != listSource[0])
    {
        printf("testSearch(): correct value not found when "
                "searching for first element\n");
        assert(0);
    }

    res = testobjListSearch(list, &listSource[listSourceSize-1], testobjCmp);
    if (*res->val != listSource[listSourceSize-1])
    {
        printf("testSearch(): correct value not found when "
                "searching for last element\n");
        assert(0);
    }

    res = testobjListSearch(list, &listSource[2], testobjCmp);
    if (*res->val != listSource[2])
    {
        printf("testSearch(): correct value not found when "
                "searching for middle element\n");
        assert(0);
    }

    testobj nonExisting = 15;
    res = testobjListSearch(list, &nonExisting, testobjCmp);
    if (res != NULL)
    {
        printf("testSearch(): null not returned when "
                "searching for non-existant element\n");
        assert(0);
    }

    testobjList * l;
    for (l = list; l;)
    {
        testobjList * lt = l;
        l = l->next;
        free(lt);
    }
}

bool testCmp(const testobj * a, const testobj * b)
{
    if (*b == 55)
        return true;
    return false;
}
static void testUpsert()
{
    allocatorTrackerReset();

    testobjList * list = NULL;

    testobj val1 = 55;
    testobj * valp;
    list = testobjListUpsert(list, &val1, testobjCmp, &valp);

    if (!listEqExpected(list, &val1, 1))
    {
        printf("testUpsert(): upserting into empty list leads"
                " to unexpected list\n");
        assert(0);
    }

    testobj val2 = 66;
    list = testobjListUpsert(list, &val2, testobjCmp, &valp);

    {
        testobj expected[] = {66, 55};
        if (!listEqExpected(list, expected, 2))
        {
            printf("testUpsert(): upserting new element leads"
                    " to unexpected list\n");
            assert(0);
        }
    }

    testobj val3 = 77;
    list = testobjListUpsert(list, &val3, testCmp, &valp);
    {
        testobj expected[] = {66, 77};

        if (!listEqExpected(list, expected, 2))
        {
            printf("testUpsert(): upserting updating element leads"
                    " to unexpected list\n");
            assert(0);
        }

        if (*valp != 55)
        {
            printf("testUpsert(): updating element leads to "
                    "wrong value being popped\n");
            assert(0);
        }
    }

    testobjList * l;
    for (l = list; l;)
    {
        testobjList * lt = l;
        l = l->next;
        free(lt);
    }

    if (allocatorTrackerGetBytesAllocated() != allocatorTrackerGetBytesFreed())
    {
        printf("testUpsert(): after removing all list elements there is "
                "a memory leak\n");
        assert(0);
    }
}

static int listEqExpected(
        const testobjList * list, const testobj * expected, int expected_len)
{
    int i;
    const testobjList * listp = list;

    for (i = 0; i < expected_len-1; i++)
    {
        if (*listp->val != expected[i])
            return 0;
        if (listp->next == NULL)
            return 0;

        listp = listp->next; 
    }

    if (*listp->val != expected[expected_len-1])
        return 0;
    if (listp->next != NULL)
        return 0;

    return 1;
}
