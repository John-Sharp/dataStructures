#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdio.h>

bool <pf>Cmp(const <tp> * a, const <tp> * b)
{
    return (memcmp(a, b, sizeof(*a)) == 0);
}

<pf>List * <pf>ListAdd(<pf>List * l, <tp> * val)
{
    <pf>List * nn = malloc(sizeof(*nn));
    if (!nn)
    {
        fprintf(stderr, "error allocating memory for list\n");
        exit(1);
        return l;
    }
    nn->val = val;
    nn->next = l;
    nn->prev = NULL;
    if (l)
        l->prev = nn;

    return nn;
}

<pf>List * <pf>ListAddToEnd(<pf>List * l, <tp> * val)
{
    <pf>List * nn = malloc(sizeof(*nn));
    if (!nn)
    {
        fprintf(stderr, "error allocating memory for list\n");
        exit(1);
        return l;
    }
    nn->val = val;
    nn->next = NULL;
    nn->prev = NULL;

    if (!l)
        return nn;

    <pf>List * node;
    for (node = l; node->next != NULL; node = node->next)
    {}

    node->next = nn;
    nn->prev = node;

    return l;
}

<pf>List * <pf>ListNodeMv(<pf>List * l, <pf>List ** lOriginal, <pf>List * m)
{
    if (!m)
        return l;

    if (!m->prev)
    {
        if (*lOriginal != l)
            *lOriginal = m->next;
    }
    else
    {
        m->prev->next = m->next;
    }

    if (m->next)
    {
        m->next->prev = m->prev;
    }

    m->next = NULL;
    if (m != l)
        m->next = l;

    m->prev = NULL;
    if (l)
    {
        m->prev = l->prev;
        if (*lOriginal != l)
            l->prev = m;
    }

    return m;
}

<pf>List * <pf>ListRm(<pf>List * l, const <tp> * val, bool (*cmp)(const <tp> * a, const <tp> * b), <tp> ** removedVal)
{
    <pf>List * n;
    if (removedVal)
        *removedVal = NULL;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            if (n->next)
                n->next->prev = n->prev;
            if (!n->prev)
            {
                if (removedVal)
                    *removedVal = n->val;
                <pf>List * ret = n->next;
                free(n);
                return ret;
            }

            n->prev->next = n->next;
            if (removedVal)
                *removedVal = n->val;
            free(n);
            return l;
        }
    }

    return l;
}

<pf>List * <pf>ListSearch(<pf>List * l, const <tp> * val, bool (*cmp)(const <tp> * a, const <tp> * b))
{
    <pf>List * n;
    for (n = l; n != NULL; n = n->next)
    {
        if (cmp(val, (n->val)))
        {
            return n;
        }
    }

    return NULL;
}

<pf>List * <pf>ListUpsert(<pf>List * l, <tp> * val, bool (*cmp)(const <tp> * a, const <tp> * b), <tp> ** replacedVal)
{
    <pf>List * n = <pf>ListSearch(l, val, cmp);

    if (!n)
    {
        if (replacedVal)
            *replacedVal = NULL;
        return <pf>ListAdd(l, val);
    }
    <tp> * oldVal = n->val;

    n->val = val;
    *replacedVal = oldVal;

    return l;
}
