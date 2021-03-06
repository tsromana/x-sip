#include <string.h>
#include <stdio.h>

#include "Header.h"
#include "Parser.h"
#include "Parameter.h"
#include "Utils.h"
#include "utils/list/include/list.h"
#include "StringExt.h"

struct Parameters {
    t_list *parameters;
};

struct Parameter {
    char name[PARAMETER_NAME_MAX_LENGTH];
    char value[PARAMETER_VALUE_MAX_LENGTH];
};

static struct HeaderPattern ParameterPattern[] = {
    {"*", EMPTY, EQUAL, 1, OFFSETOF(struct Parameter, name), ParseString, NULL, String2String},
    {"*", EQUAL, EMPTY, 0, OFFSETOF(struct Parameter,value), ParseString, NULL, String2String},
    {NULL},
};

struct Parameter *ParseParameter(char *s)
{
    struct Parameter *p = CreateEmptyParameter();
    Parse(s, p, ParameterPattern);
    
    return p;
}

BOOL ParameterMatched(struct Parameter *p1, struct Parameter *p2)
{
    return !(StrcmpExt(p1->name, p2->name) != 0 
            || StrcmpExt(p1->value, p2->value) != 0);
}

DEFINE_STRING_MEMBER_READER(struct Parameter, GetParameterName, name);
DEFINE_STRING_MEMBER_READER(struct Parameter, GetParameterValue, value);

int ParametersLength(struct Parameters *ps)
{
    return get_list_len(ps->parameters);
}

/*
 * Only call this function when parse parameters stand alone.
 */
int ParseParametersExt(char *s, void *target)
{
    struct Parameters *p = (struct Parameters *)target;    
    char *localString = calloc(1, strlen(s) + 1);    
    char *paramStart;
    char *save_ptr = NULL;
    char *separater = NULL;
    
    strcpy(localString, s);

    if (strchr(localString, SEMICOLON) != NULL) {
        separater = ";";
    } else {
        separater = ",";
    }
    
    paramStart = strtok_r(localString, separater, &save_ptr);

    while(paramStart) {
        put_in_list(&p->parameters, ParseParameter(paramStart));
        paramStart = strtok_r(NULL, separater, &save_ptr);
    }
    
    free(localString);

    return 0;

}

/* 
 * Only call this function when parameters embedded in other structure 
 */
int ParseParameters(char *s, void *target)
{
    struct Parameters **p = target;

    ParseParametersExt(s, *p);

    return 0;
}

int AddParameter(struct Parameters *ps, char *name, char *value)
{
    struct Parameter *p = NULL;
    int i = 0;
    int length = ParametersLength(ps);

    for ( ; i < length; i ++) {
        if (StrcmpExt (((struct Parameter *) get_data_at(ps->parameters, i))->name, name) == 0)
            p = get_data_at (ps->parameters, i);
    }

    if (p == NULL) {
        p = CreateEmptyParameter();
        put_in_list(&ps->parameters, p);
    }

    Copy2Target(p, name, &ParameterPattern[0]);
    Copy2Target(p, value,&ParameterPattern[1]);

    return 0;
}

BOOL ParametersMatched(struct Parameters *ps1, struct Parameters *ps2)
{
    int i = 0;

    assert(ps1 != NULL);
    assert(ps2 != NULL);

    int length = ParametersLength(ps1);    
    
    if (length != ParametersLength(ps2)) {
        return FALSE;
    }

    for ( ; i < length; i ++) {
        struct Parameter *p1 = get_data_at(ps1->parameters, i);
        struct Parameter *p2 = get_data_at(ps2->parameters, i);
        if (!ParameterMatched(p1, p2))
            return FALSE;

    }

    return TRUE;
    
}

char *GetParameter(struct Parameters *ps, char *name)
{
    assert(ps != NULL);
    assert(name != NULL);

    int i = 0;
    int length = ParametersLength(ps);

    
    for ( ; i < length; i ++) {
        struct Parameter *p = get_data_at(ps->parameters, i);
        if (StrcmpExt (p->name, name) == 0)
            return p->value;
    }

    return NULL;
}

char *Parameters2String(char *pos, void *ps, struct HeaderPattern *p)
{
    struct Parameters **params = ps;
    
    return Parameters2StringExt(pos, *params, p);
}

char *Parameters2StringExt(char *pos, void *ps, struct HeaderPattern *pattern)
{
    int i = 0;
    struct Parameters *params = (struct Parameters *)ps;
    int length = ParametersLength(ps);

    for (; i < length; i ++) {
        struct Parameter *p = get_data_at(params->parameters, i);

        if (i == 0 && pattern != NULL)
            *pos++ = pattern->startSeparator;
        else
            *pos ++ = SEMICOLON;
        pos = ToString(pos, p, ParameterPattern);
    }

    return pos;
}

struct Parameter *CreateEmptyParameter()
{
    struct Parameter *p = calloc(1, sizeof(struct Parameter));
    
    assert(p != NULL);
    return p;
}

struct Parameter *CreateParameter(char *name, char *value)
{
    struct Parameter *p = CreateEmptyParameter();

    assert(name != NULL);
    assert(value != NULL);

    Copy2Target(p, name, &ParameterPattern[0]);
    Copy2Target(p, value,&ParameterPattern[1]);

    return p;
}

void DestroyParameter(struct Parameter *p)
{
    if (p != NULL)
        free(p);
}

void ClearParameters(struct Parameters *ps)
{
    int i = 0;
    int length = ParametersLength(ps);

    for (; i < length; i++) {
        struct Parameter *p = get_data_at(ps->parameters, 0);
        free(p);
        del_node_at(&ps->parameters, 0);
    }
}

struct Parameters *CreateParameters()
{
    return calloc(1, sizeof(struct Parameters));
}

struct Parameters *ParametersDup(struct Parameters *src)
{
    struct Parameters *dest = CreateParameters();
    int length = ParametersLength(src);
    int i = 0;
    
    for (; i < length; i++) {
        struct Parameter *p = get_data_at(src->parameters, i);
        AddParameter(dest, GetParameterName(p), GetParameterValue(p));
    }
    return dest;
}

void DestroyParameters(struct Parameters **ps)
{
    struct Parameters *pp = *ps;

    assert(pp != NULL);
    
    int length = ParametersLength(pp);
    int i = 0;
    for ( ; i < length; i ++) {
        struct Parameter *p = get_data_at(pp->parameters, i);
        free(p);
    }

    destroy_list(&pp->parameters, NULL);
    free(pp);

    *ps = NULL;
}
