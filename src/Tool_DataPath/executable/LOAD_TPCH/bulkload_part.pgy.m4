/*
 * Bulkload part table into DataPath
 */

data = READ "TABLE_DIR/`'ifelse(NUM_STRIPES, 1, `part.tbl"', `part.tbl.%d"' : NUM_STRIPES) SEPARATOR '|' ATTRIBUTES FROM part;

STORE data INTO part;
