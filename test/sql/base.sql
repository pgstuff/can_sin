\set ECHO none
\i sql/can_sin.sql
\set ECHO all

CREATE TABLE can_sins(id serial primary key, can_sin can_sin unique);
INSERT INTO can_sins(can_sin) VALUES('046 454 286');
INSERT INTO can_sins(can_sin) VALUES('000000000'); -- test limits
INSERT INTO can_sins(can_sin) VALUES('999 999 998');

SELECT * FROM can_sins ORDER BY can_sin;

SELECT MIN(can_sin) AS min FROM can_sins;
SELECT MAX(can_sin) AS max FROM can_sins;

SELECT id,can_sin::text FROM can_sins WHERE can_sin = '999 999 998';

-- index scan
SELECT id,can_sin::text FROM can_sins WHERE can_sin = '000000000';

SET enable_seqscan = false;
SELECT id,can_sin::text FROM can_sins WHERE can_sin = '999 999 998';
RESET enable_seqscan;

-- operators and conversions
SELECT '000000000'::can_sin < '000000000'::can_sin;
SELECT '000000000'::can_sin > '000000000'::can_sin;
SELECT '000000000'::can_sin < '999999998'::can_sin;
SELECT '000000000'::can_sin > '999999998'::can_sin;
SELECT '000000000'::can_sin <= '000000000'::can_sin;
SELECT '000000000'::can_sin >= '000000000'::can_sin;
SELECT '000000000'::can_sin <= '999999998'::can_sin;
SELECT '000000000'::can_sin >= '999999998'::can_sin;
SELECT '000000000'::can_sin <> '000000000'::can_sin;
SELECT '000000000'::can_sin <> '999999998'::can_sin;
SELECT '000000000'::can_sin = '000000000'::can_sin;
SELECT '000000000'::can_sin = '999999998'::can_sin;

-- COPY FROM/TO
TRUNCATE can_sins;
COPY can_sins(can_sin) FROM STDIN;
000000000
999 999 998
\.
COPY can_sins TO STDOUT;

-- clean up --
DROP TABLE can_sins;

-- errors
SELECT ''::can_sin;
SELECT '823 456 785'::can_sin; -- 8 is not used.
SELECT '1000000000'::can_sin;
SELECT '!'::can_sin;
