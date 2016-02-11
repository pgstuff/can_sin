/*
 * Author: The maintainer's name
 * Created at: Wed Oct 14 23:12:59 -0400 2015
 *
 */

SET client_min_messages = warning;

-- SQL definitions for USAEIN type
CREATE TYPE can_sin;

-- basic i/o functions
CREATE OR REPLACE FUNCTION can_sin_in(cstring) RETURNS can_sin AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_sin_out(can_sin) RETURNS cstring AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_sin_send(can_sin) RETURNS bytea AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_sin_recv(internal) RETURNS can_sin AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE can_sin (
	input = can_sin_in,
	output = can_sin_out,
	send = can_sin_send,
	receive = can_sin_recv,
	internallength = 4,
	passedbyvalue
);

-- functions to support btree opclass
CREATE OR REPLACE FUNCTION can_sin_lt(can_sin, can_sin) RETURNS bool AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_sin_le(can_sin, can_sin) RETURNS bool AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_sin_eq(can_sin, can_sin) RETURNS bool AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_sin_ne(can_sin, can_sin) RETURNS bool AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_sin_ge(can_sin, can_sin) RETURNS bool AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_sin_gt(can_sin, can_sin) RETURNS bool AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_sin_cmp(can_sin, can_sin) RETURNS int4 AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;

-- to/from text conversion
CREATE OR REPLACE FUNCTION can_sin_to_text(can_sin) RETURNS text AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION can_sin_from_text(text) RETURNS can_sin AS '$libdir/can_sin'
LANGUAGE C IMMUTABLE STRICT;

-- operators
CREATE OPERATOR < (
	leftarg = can_sin, rightarg = can_sin, procedure = can_sin_lt,
	commutator = >, negator = >=,
	restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR <= (
	leftarg = can_sin, rightarg = can_sin, procedure = can_sin_le,
	commutator = >=, negator = >,
	restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR = (
	leftarg = can_sin, rightarg = can_sin, procedure = can_sin_eq,
	commutator = =, negator = <>,
	restrict = eqsel, join = eqjoinsel,
	merges
);
CREATE OPERATOR <> (
	leftarg = can_sin, rightarg = can_sin, procedure = can_sin_ne,
	commutator = <>, negator = =,
	restrict = neqsel, join = neqjoinsel
);
CREATE OPERATOR > (
	leftarg = can_sin, rightarg = can_sin, procedure = can_sin_gt,
	commutator = <, negator = <=,
	restrict = scalargtsel, join = scalargtjoinsel
);
CREATE OPERATOR >= (
	leftarg = can_sin, rightarg = can_sin, procedure = can_sin_ge,
	commutator = <=, negator = <,
	restrict = scalargtsel, join = scalargtjoinsel
);

-- aggregates
CREATE OR REPLACE FUNCTION can_sin_smaller(can_sin, can_sin)
RETURNS can_sin
AS '$libdir/can_sin'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION can_sin_larger(can_sin, can_sin)
RETURNS can_sin
AS '$libdir/can_sin'
    LANGUAGE C IMMUTABLE STRICT;

CREATE AGGREGATE min(can_sin)  (
    SFUNC = can_sin_smaller,
    STYPE = can_sin,
    SORTOP = <
);

CREATE AGGREGATE max(can_sin)  (
    SFUNC = can_sin_larger,
    STYPE = can_sin,
    SORTOP = >
);

-- btree operator class
CREATE OPERATOR CLASS can_sin_ops DEFAULT FOR TYPE can_sin USING btree AS
	OPERATOR 1 <,
	OPERATOR 2 <=,
	OPERATOR 3 =,
	OPERATOR 4 >=,
	OPERATOR 5 >,
	FUNCTION 1 can_sin_cmp(can_sin, can_sin);
-- cast from/to text
CREATE CAST (can_sin AS text) WITH FUNCTION can_sin_to_text(can_sin) AS ASSIGNMENT;
CREATE CAST (text AS can_sin) WITH FUNCTION can_sin_from_text(text) AS ASSIGNMENT;

/* Does this survive a pg_dump?
CREATE CAST (int     AS can_sin) WITHOUT FUNCTION AS ASSIGNMENT;
CREATE CAST (can_sin AS int)     WITHOUT FUNCTION;
*/
