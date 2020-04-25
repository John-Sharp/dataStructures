#include <stdbool.h>

// typedef struct <pf>List <pf>List;
typedef struct <pf>List <pf>List;
struct <pf>List {
    <tp> * val;
    <pf>List * next;
    <pf>List * prev;
};

bool <pf>Cmp(const <tp> * a, const <tp> * b);

// <pf>ListAdd: adds element to head of list
<pf>List * <pf>ListAdd(<pf>List * l, <tp> * val);

// <pf>ListToEnd: adds element to end of list
<pf>List * <pf>ListAddToEnd(<pf>List * l, <tp> * val);

// <pf>ListNodeMv: removes node `m` from the list `lOriginal` it is on, and
// adds it to list `l`
<pf>List * <pf>ListNodeMv(<pf>List * l, <pf>List ** lOriginal, <pf>List * m);

// <pf>ListRmremove: removes element from list
<pf>List * <pf>ListRm(<pf>List * l, const <tp> * val, bool (*cmp)(const <tp> * a, const <tp> * b), <tp> ** removedVal);

<pf>List * <pf>ListSearch(<pf>List * l, const <tp> * val, bool (*cmp)(const <tp> * a, const <tp> * b));

<pf>List * <pf>ListUpsert(<pf>List * l, <tp> * val, bool (*cmp)(const <tp> * a, const <tp> * b), <tp> ** replacedVal);
