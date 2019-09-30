#include <util/string.h>

char * create_empty_str () {
    char * str;

    str = (char *)calloc (1, sizeof (char));

    return str;
}

char * append_str (char * str, char * s) {
    int str_length;
    int   s_length;

    str_length = strlen (str);
    s_length   = strlen (s);
    str        = realloc (str, str_length + s_length);

    for (int i = 0; i < s_length; i++) {
        str [str_length + i] = s [i];
    }
    str [str_length + s_length] = 0;

    return str;
}

char * append_char (char * str, char c) {
    int str_length;

    str_length = strlen  (str);
    str        = realloc (str, str_length + 2);

    str [str_length]     = c;
    str [str_length + 1] = 0;

    return str;
}
