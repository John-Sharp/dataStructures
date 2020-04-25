typedef int testobj;
#include "listHeaders/testobjList.h"
#include "listCode/testobjList.inc"
#include <assert.h>

#include "testMallocTracker.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

static void testAddRm();
static void testAddEnd();
static void testListNodeMv();
static void testSearch();
static void testUpsert();
int main()
{
    testAddRm();
    testAddEnd();
    testListNodeMv();
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


static void subTestNodeMvSingleMemberListSelf();
static void subTestNodeMvSingleMemberListEmptyList();
static void subTestNodeMvEmptyListMultiMemberList();
static void subTestNodeMvMultiMemberListMultiMemberList();
static void testListNodeMv()
{
    subTestNodeMvSingleMemberListSelf();
    subTestNodeMvSingleMemberListEmptyList();
    subTestNodeMvEmptyListMultiMemberList();
    subTestNodeMvMultiMemberListMultiMemberList();
}

static void subTestNodeMvSingleMemberListSelf()
{
    // test moving node to own list on single member list
    testobjList * list = NULL;
    testobj listSource [] = {42};
    list = testobjListAdd(list, listSource);

    allocatorTrackerReset();
    testobjList * targetList = list;
    targetList = testobjListNodeMv(targetList, &list, list);

    if (allocatorTrackerGetBytesAllocated() != 0)
    {
        printf("subTestNodeMvSingleMemberListSelf(): "
                "operation allocates memory\n");
        assert(0);
    }

    if (!listEqExpected(targetList, listSource, 1))
    {
        printf("subTestNodeMvSingleMemberListSelf(): unexpected resultant "
                "list\n");
        assert(0);
    }

    if (!listEqExpected(list, listSource, 1))
    {
        printf("subTestNodeMvSingleMemberListSelf(): unexpected original "
                "list \n");
        assert(0);
    }

    free(list);
}

static void subTestNodeMvSingleMemberListEmptyList()
{
    // test moving node on its own on a list to another, empty, list
    testobjList * list = NULL;
    testobj listSource [] = {42};
    list = testobjListAdd(list, listSource);

    allocatorTrackerReset();
    testobjList * targetList = NULL;

    targetList = testobjListNodeMv(targetList, &list, list);

    if (allocatorTrackerGetBytesAllocated() != 0)
    {
        printf("subTestNodeMvSingleMemberListEmptyList(): "
                "list allocates memory\n");
        assert(0);
    }

    if (!listEqExpected(targetList, listSource, 1))
    {

        printf("subTestNodeMvSingleMemberListEmptyList(): "
                "unexpected target list\n");
        assert(0);
    }

    if (list != NULL)
    {
        printf("subTestNodeMvSingleMemberListEmptyList(): "
                "original list not empty\n");
        assert(0);
    }

    free(targetList);
}

static void subTestNodeMvEmptyListMultiMemberList()
{
    testobjList * list = NULL;
    testobj targetSource[] = {66,3,8,90};
    int targetSourceSize = sizeof(targetSource)/sizeof(targetSource[0]);
    testobjList * targetList = NULL;

    int i;
    for (i = targetSourceSize-1; i >= 0; i--)
    {
        targetList = testobjListAdd(targetList, &targetSource[i]);
    }

    allocatorTrackerReset();

    targetList = testobjListNodeMv(targetList, &list, list);

    if (allocatorTrackerGetBytesAllocated() != 0)
    {
        printf("subTestNodeMvEmptyListMultiMemberList(): "
                "list allocates memory\n");
        assert(0);
    }

    if (!listEqExpected(targetList, targetSource, targetSourceSize))
    {

        printf("subTestNodeMvEmptyListMultiMemberList(): "
                "unexpected target list\n");
        assert(0);
    }

    if (list != NULL)
    {
        printf("subTestNodeMvEmptyListMultiMemberList(): "
                "original list not empty\n");
        assert(0);
    }

    testobjList * l;
    for (l = targetList; l;)
    {
        testobjList * lt = l;
        l = l->next;
        free(lt);
    }
}

static void subTestNodeMvMultiMemberListMultiMemberList()
{
    testobjList * list = NULL;
    testobj listSource[] = {96, 72, 8, 16};
    int listSourceSize = sizeof(listSource)/sizeof(listSource[0]);

    int i;
    for (i = listSourceSize-1; i >= 0; i--)
    {
        list = testobjListAdd(list, &listSource[i]);
    }

    testobjList * targetList = NULL;

    testobj targetSource[] = {3,1,2};
    int targetSourceSize = sizeof(targetSource)/sizeof(targetSource[0]);

    testobj listExpectedStage1[] = {listSource[0], listSource[2], listSource[3]};
    testobj listExpectedStage2[] = {listSource[0], listSource[2]};
    testobj listExpectedStage3[] = {listSource[2]};

    testobj targetListExpected[] = {
        listSource[0],listSource[3],listSource[1],
        targetSource[0], targetSource[1], targetSource[2]};
    int targetListExpectedSize = sizeof(targetListExpected)\
                                 /sizeof(targetListExpected[0]);

    for (i = targetSourceSize-1; i >= 0; i--)
    {
        targetList = testobjListAdd(targetList, &targetSource[i]);
    }

    allocatorTrackerReset();

    testobjList * nodeToMove = list->next;

    targetList = testobjListNodeMv(targetList, &list, nodeToMove);

    if (allocatorTrackerGetBytesAllocated() != 0)
    {
        printf("subTestNodeMvMultiMemberListMultiMemberList(): "
                "list allocates memory\n");
        assert(0);
    }

    if (!listEqExpected(targetList, &targetListExpected[2],
                targetListExpectedSize-2))
    {

        printf("subTestNodeMvMultiMemberListMultiMemberList(): "
                "unexpected target list\n");
        assert(0);
    }

    if (!listEqExpected(list, listExpectedStage1,
                ARRAY_SIZE(listExpectedStage1)))
    {

        printf("subTestNodeMvMultiMemberListMultiMemberList(): "
                "unexpected list\n");
        assert(0);
    }

    nodeToMove = list->next->next;

    targetList = testobjListNodeMv(targetList, &list, nodeToMove);

    if (allocatorTrackerGetBytesAllocated() != 0)
    {
        printf("subTestNodeMvMultiMemberListMultiMemberList(): "
                "list allocates memory\n");
        assert(0);
    }

    if (!listEqExpected(targetList, &targetListExpected[1],
                targetListExpectedSize-1))
    {

        printf("subTestNodeMvMultiMemberListMultiMemberList(): "
                "unexpected target list\n");
        assert(0);
    }

    if (!listEqExpected(list, listExpectedStage2,
                ARRAY_SIZE(listExpectedStage2)))
    {

        printf("subTestNodeMvMultiMemberListMultiMemberList(): "
                "unexpected list\n");
        assert(0);
    }

    nodeToMove = list;

    targetList = testobjListNodeMv(targetList, &list, nodeToMove);

    if (allocatorTrackerGetBytesAllocated() != 0)
    {
        printf("subTestNodeMvMultiMemberListMultiMemberList(): "
                "list allocates memory\n");
        assert(0);
    }

    if (!listEqExpected(targetList, targetListExpected,
                targetListExpectedSize))
    {

        printf("subTestNodeMvMultiMemberListMultiMemberList(): "
                "unexpected target list\n");
        assert(0);
    }

    if (!listEqExpected(list, listExpectedStage3,
                ARRAY_SIZE(listExpectedStage3)))
    {

        printf("subTestNodeMvMultiMemberListMultiMemberList(): "
                "unexpected list\n");
        assert(0);
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
