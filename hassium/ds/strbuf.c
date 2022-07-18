#include <ds/strbuf.h>

static void expand_if_needed(struct strbuf *);

struct strbuf *strbuf_new()
{
    struct strbuf *strbuf = (struct strbuf *)calloc(1, sizeof(struct strbuf));
    strbuf->buf = (char *)calloc(STRBUF_EXPAND_AT, sizeof(char));
    strbuf->len = 0;
    strbuf->size = STRBUF_EXPAND_AT;
    return strbuf;
}

char *strbuf_done(struct strbuf *strbuf)
{
    char *ret = strbuf->buf;
    free(strbuf);
    return ret;
}

void strbuf_append(struct strbuf *strbuf, char c)
{
    expand_if_needed(strbuf);
    strbuf->buf[strbuf->len++] = c;
}

static void expand_if_needed(struct strbuf *strbuf)
{
    if (strbuf->len >= strbuf->size)
    {
        strbuf->size += STRBUF_EXPAND_AT;
        strbuf->buf = (char *)realloc(strbuf->buf, sizeof(char) * strbuf->size);
    }
}