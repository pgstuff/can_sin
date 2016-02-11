#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

typedef uint32 can_sin_t;

#define CANSINGetDatum(x)	 Int32GetDatum(x)
#define DatumGetCANSIN(x)	 DatumGetInt32(x)
#define PG_GETARG_CANSIN(n) DatumGetCANSIN(PG_GETARG_DATUM(n))
#define PG_RETURN_CANSIN(x) return CANSINGetDatum(x)

//#define DIGITTOINT(n)	(((n) >= '0' && (n) <= '9') ? (int32) ((n) - '0') : 0)

Datum can_sin_in(PG_FUNCTION_ARGS);
Datum can_sin_out(PG_FUNCTION_ARGS);
Datum can_sin_to_text(PG_FUNCTION_ARGS);
Datum can_sin_from_text(PG_FUNCTION_ARGS);
Datum can_sin_send(PG_FUNCTION_ARGS);
Datum can_sin_recv(PG_FUNCTION_ARGS);
Datum can_sin_lt(PG_FUNCTION_ARGS);
Datum can_sin_le(PG_FUNCTION_ARGS);
Datum can_sin_eq(PG_FUNCTION_ARGS);
Datum can_sin_ne(PG_FUNCTION_ARGS);
Datum can_sin_ge(PG_FUNCTION_ARGS);
Datum can_sin_gt(PG_FUNCTION_ARGS);
Datum can_sin_cmp(PG_FUNCTION_ARGS);

static can_sin_t cstring_to_can_sin(char *can_sin_string);
static char *can_sin_to_cstring(can_sin_t can_sin);



/* generic input/output functions */
PG_FUNCTION_INFO_V1(can_sin_in);
Datum
can_sin_in(PG_FUNCTION_ARGS)
{
	can_sin_t	result;

	char   *can_sin_str = PG_GETARG_CSTRING(0);
	result = cstring_to_can_sin(can_sin_str);

	PG_RETURN_CANSIN(result);
}

PG_FUNCTION_INFO_V1(can_sin_out);
Datum
can_sin_out(PG_FUNCTION_ARGS)
{
	can_sin_t	packed_can_sin;
	char   *can_sin_string;

	packed_can_sin = PG_GETARG_CANSIN(0);
	can_sin_string = can_sin_to_cstring(packed_can_sin);

	PG_RETURN_CSTRING(can_sin_string);
}

/* type to/from text conversion routines */
PG_FUNCTION_INFO_V1(can_sin_to_text);
Datum
can_sin_to_text(PG_FUNCTION_ARGS)
{
	char	*can_sin_string;
	text	*can_sin_text;

	can_sin_t	packed_can_sin =  PG_GETARG_CANSIN(0);

	can_sin_string = can_sin_to_cstring(packed_can_sin);
	can_sin_text = DatumGetTextP(DirectFunctionCall1(textin, CStringGetDatum(can_sin_string)));

	PG_RETURN_TEXT_P(can_sin_text);
}

PG_FUNCTION_INFO_V1(can_sin_from_text);
Datum
can_sin_from_text(PG_FUNCTION_ARGS)
{
	text  *can_sin_text = PG_GETARG_TEXT_P(0);
	char  *can_sin_str = DatumGetCString(DirectFunctionCall1(textout, PointerGetDatum(can_sin_text)));
	can_sin_t can_sin = cstring_to_can_sin(can_sin_str);

	PG_RETURN_CANSIN(can_sin);
}

/* Functions to convert to/from bytea */
PG_FUNCTION_INFO_V1(can_sin_send);
Datum
can_sin_send(PG_FUNCTION_ARGS)
{
	StringInfoData buffer;
	can_sin_t can_sin = PG_GETARG_CANSIN(0);

	pq_begintypsend(&buffer);
	pq_sendint(&buffer, (int32)can_sin, 4);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buffer));
}

PG_FUNCTION_INFO_V1(can_sin_recv);
Datum can_sin_recv(PG_FUNCTION_ARGS)
{
	can_sin_t	can_sin;
	StringInfo	buffer = (StringInfo) PG_GETARG_POINTER(0);

	can_sin = pq_getmsgint(buffer, 4);
	PG_RETURN_CANSIN(can_sin);
}

/* functions to support btree opclass */
PG_FUNCTION_INFO_V1(can_sin_lt);
Datum
can_sin_lt(PG_FUNCTION_ARGS)
{
	can_sin_t a = PG_GETARG_CANSIN(0);
	can_sin_t b = PG_GETARG_CANSIN(1);
	PG_RETURN_BOOL(a < b);
}

PG_FUNCTION_INFO_V1(can_sin_le);
Datum
can_sin_le(PG_FUNCTION_ARGS)
{
	can_sin_t a = PG_GETARG_CANSIN(0);
	can_sin_t b = PG_GETARG_CANSIN(1);
	PG_RETURN_BOOL (a <= b);
}

PG_FUNCTION_INFO_V1(can_sin_eq);
Datum
can_sin_eq(PG_FUNCTION_ARGS)
{
	can_sin_t a = PG_GETARG_CANSIN(0);
	can_sin_t b = PG_GETARG_CANSIN(1);
	PG_RETURN_BOOL(a == b);
}

PG_FUNCTION_INFO_V1(can_sin_ne);
Datum
can_sin_ne(PG_FUNCTION_ARGS)
{
	can_sin_t a = PG_GETARG_CANSIN(0);
	can_sin_t b = PG_GETARG_CANSIN(1);
	PG_RETURN_BOOL(a != b);
}

PG_FUNCTION_INFO_V1(can_sin_ge);
Datum
can_sin_ge(PG_FUNCTION_ARGS)
{
	can_sin_t a = PG_GETARG_CANSIN(0);
	can_sin_t b = PG_GETARG_CANSIN(1);
	PG_RETURN_BOOL(a >= b);
}

PG_FUNCTION_INFO_V1(can_sin_gt);
Datum
can_sin_gt(PG_FUNCTION_ARGS)
{
	can_sin_t a = PG_GETARG_CANSIN(0);
	can_sin_t b = PG_GETARG_CANSIN(1);
	PG_RETURN_BOOL(a > b);
}

static int32
can_sin_cmp_internal(can_sin_t a, can_sin_t b)
{
    return a - b;

    /*if (a < b)
        return -1;
    else if (a > b)
        return 1;

    return 0;*/
}

PG_FUNCTION_INFO_V1(can_sin_cmp);
Datum
can_sin_cmp(PG_FUNCTION_ARGS)
{
	can_sin_t a = PG_GETARG_CANSIN(0);
	can_sin_t b = PG_GETARG_CANSIN(1);

	PG_RETURN_INT32(can_sin_cmp_internal(a, b));
}

/*****************************************************************************
 * Aggregate functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(can_sin_smaller);

Datum
can_sin_smaller(PG_FUNCTION_ARGS)
{
   can_sin_t left  = PG_GETARG_CANSIN(0);
   can_sin_t right = PG_GETARG_CANSIN(1);
   int32 result;

   result = can_sin_cmp_internal(left, right) < 0 ? left : right;
   PG_RETURN_CANSIN(result);
}

PG_FUNCTION_INFO_V1(can_sin_larger);

Datum
can_sin_larger(PG_FUNCTION_ARGS)
{
   can_sin_t left  = PG_GETARG_CANSIN(0);
   can_sin_t right = PG_GETARG_CANSIN(1);
   int32 result;

   result = can_sin_cmp_internal(left, right) > 0 ? left : right;
   PG_RETURN_CANSIN(result);
}



/*
 * Convert a cstring to an CANSIN, validating the input.
 * Input in forms AAA BBB CCC or AAABBBCCC is accepted.
 */
static can_sin_t
cstring_to_can_sin(char *can_sin_str)
{
    char    *c;
    can_sin_t   result;
    char    dashes[] = {3, 7};
    int     dashes_no = 0;
    int     ndigits = 0;
    int     digit_value;
    int     check_digit_value;
    int     check_sum = 0;

    result = 0;

    for (c = can_sin_str; *c != 0; c++)
    {
        if (*c >= '0' && *c <= '9')
        {
            digit_value = *c - '0';
            switch (ndigits) {
                case 0:
                case 2:
                case 4:
                case 6:
                    check_sum = check_sum + digit_value;
                    if (ndigits == 0 && digit_value == 8)
                        ereport(ERROR,
                            (errmsg("CANSIN number %s is invalid.", can_sin_str)));
                break;
                case 1:
                case 3:
                case 5:
                case 7:
                    check_digit_value = digit_value * 2;
                    if (check_digit_value >= 10) {
                        check_sum = check_sum + 1 + check_digit_value % 10;
                    } else {
                        check_sum = check_sum + check_digit_value;
                    }
                break;
                case 8:
                    check_digit_value = (10 - (check_sum%10)) % 10;
                    if (check_digit_value != digit_value)
                        ereport(ERROR,
                            (errmsg("CANSIN number \"%s\" is invalid.", can_sin_str)));
                break;
            }

            result = result * 10 + digit_value;
            ndigits++;
        }
        else if (*c == ' ')
        {
            int pos = c - can_sin_str;
            if (dashes_no < 2 && pos == dashes[dashes_no])
                dashes_no++;
            else
                ereport(ERROR,
                        (errmsg("Invalid format of input data \"%s\".", can_sin_str),
                         errhint("Valid formats are: AAA BBB CCC or AAABBBCCC")));
        }
        else
            ereport(ERROR,
                    (errmsg("Unexpected character '%c' in input data \"%s\".", *c, can_sin_str),
                     errhint("Valid CANSIN consists of digits and optional spaces.")));

    }
    if (ndigits != 9)
        ereport(ERROR,
                (errmsg("Invalid number of digits (%d) in input data \"%s\".", ndigits, can_sin_str),
                 errhint("Valid CANSIN consists of 9 digits.")));

    PG_RETURN_CANSIN(result);

}
/*static can_sin_t
cstring_to_can_sin(char *can_sin_str)
{
    char   *c;
    can_sin_t   result;
    char    dashes[] = {3, 7};
    int     dashes_no = 0;
    int     ndigits = 0;
    int32   digit_group_1,
            digit_group_2,
            digit_group_3;
    int digit_value;
    int check_digit_value;
    int check_sum = 0;

    digit_group_1 = digit_group_2 = digit_group_3 = 0;

    for (c = can_sin_str; *c != 0; c++)
    {
        if (isdigit(*c))
        {
            digit_value = DIGITTOINT(*c);
            switch (ndigits) {
                case 0:
                case 2:
                case 4:
                case 6:
                    check_sum = check_sum + digit_value;
                    if (ndigits == 0 && digit_value == 8)
                        ereport(ERROR,
                            (errmsg("CANSIN number %s is invalid", can_sin_str)));
                break;
                case 1:
                case 3:
                case 5:
                case 7:
                    check_digit_value = digit_value * 2;
                    if (check_digit_value >= 10) {
                        check_sum = check_sum + 1 + check_digit_value % 10;
                    } else {
                        check_sum = check_sum + check_digit_value;
                    }
                break;
                case 8:
                    check_digit_value = (10 - (check_sum%10)) % 10;
                    if (check_digit_value != digit_value)
                        ereport(ERROR,
                            (errmsg("CANSIN number %s is invalid", can_sin_str)));
                break;
            }

            if (ndigits < 3) digit_group_1 = digit_group_1 * 10 + digit_value;
            else if (ndigits < 6) digit_group_2 = digit_group_2 * 10 + digit_value;
            else if (ndigits < 9) digit_group_3 = digit_group_3 * 10 + digit_value;
            ndigits++;
        }
        else if (*c == ' ')
        {
            int pos = c - can_sin_str;
            if (dashes_no < 2 && pos == dashes[dashes_no])
                dashes_no++;
            else
                ereport(ERROR,
                        (errmsg("invalid format of input data %s", can_sin_str),
                         errhint("Valid formats are: AAA BBB CCC or AAABBBCCC")));
        }
        else
            ereport(ERROR,
                    (errmsg("unexpected character '%c' in input data %s", *c, can_sin_str),
                     errhint("Valid CANSIN consists of digits and optional spaces")));

    }
    if (ndigits != 9)
        ereport(ERROR,
                (errmsg("invalid number of digits (%d) in input data %s", ndigits, can_sin_str),
                 errhint("Valid CANSIN consists of 9 digits")));

    result = digit_group_3 + (digit_group_2 << 12) + (digit_group_1 << 24);
    PG_RETURN_CANSIN(result);

}*/

/* Convert the internal representation to the AAA BBB CCC output string */
static char *
can_sin_to_cstring(can_sin_t can_sin)
{
    int32   remainder = can_sin;
    int32   digit_value;
    //int32   n = can_sin;
    char   *can_sin_str = palloc(12);

    if (can_sin > 999999999 || can_sin < 0)
        ereport(ERROR,
                (errmsg("Invalid data."),
                 errhint("The SIN data is out of range.")));

    /*digit_value = remainder * .00000001;
    can_sin_str[0] = '0' + digit_value;
    remainder = remainder - digit_value * 100000000;
    digit_value = remainder * .0000001;
    can_sin_str[1] = '0' + digit_value;
    remainder = remainder - digit_value * 10000000;
    digit_value = remainder * .000001;
    can_sin_str[2] = '0' + digit_value;
    remainder = remainder - digit_value * 1000000;
    digit_value = remainder * .00001;
    can_sin_str[3] = ' ';
    can_sin_str[4] = '0' + digit_value;
    remainder = remainder - digit_value * 100000;
    digit_value = remainder * .0001;
    can_sin_str[5] = '0' + digit_value;
    remainder = remainder - digit_value * 10000;
    digit_value = remainder * .001;
    can_sin_str[6] = '0' + digit_value;
    remainder = remainder - digit_value * 1000;
    digit_value = remainder * .01;
    can_sin_str[7] = ' ';
    can_sin_str[8] = '0' + digit_value;
    remainder = remainder - digit_value * 100;
    digit_value = remainder * .1;
    can_sin_str[9] = '0' + digit_value;
    remainder = remainder - digit_value * 10;
    can_sin_str[10] = '0' + remainder;
    can_sin_str[11] = '\0';*/

    can_sin_str[11] = '\0';
    can_sin_str[10] = '0' + remainder % 10;
    remainder *= .1;
    can_sin_str[9] = '0' + remainder % 10;
    remainder *= .1;
    can_sin_str[8] = '0' + remainder % 10;
    remainder *= .1;
    can_sin_str[7] = ' ';
    can_sin_str[6] = '0' + remainder % 10;
    remainder *= .1;
    can_sin_str[5] = '0' + remainder % 10;
    remainder *= .1;
    can_sin_str[4] = '0' + remainder % 10;
    remainder *= .1;
    can_sin_str[3] = ' ';
    can_sin_str[2] = '0' + remainder % 10;
    remainder *= .1;
    can_sin_str[1] = '0' + remainder % 10;
    remainder *= .1;
    can_sin_str[0] = '0' + remainder % 10;

    return can_sin_str;
}
/*static char *
can_sin_to_cstring(can_sin_t can_sin)
{
    int32   digit_group_1,
            digit_group_2,
            digit_group_3;
    int32   ndigits;
    char   *can_sin_str = palloc(12);

    if (can_sin < 0)
        * Error out *;

    digit_group_3  = can_sin & ((1 << 2) - 1); * first 14 bits *
    digit_group_2 = (can_sin >> 12) & 2047; * next 7 bits *
    digit_group_1 = (can_sin >> 24);  * last 10 bits (highest one is always 0) *

    if ((ndigits = snprintf(can_sin_str, 12, "%03d %03d %03d", digit_group_1, digit_group_2, digit_group_3)) != 11)
        ereport(ERROR,
                (errmsg("invalid size (%d) of in input data %s", ndigits-2, can_sin_str),
                 errhint("Valid CANSIN consists of 9 digits")));

    if (can_sin_str[3] != ' ' || can_sin_str[7] != ' ')
        ereport(ERROR,
                (errmsg("invalid format of input data %s", can_sin_str),
                 errhint("Valid formats are: AAA BBB CCC or AAABBBCCC")));


    return can_sin_str;
}*/
