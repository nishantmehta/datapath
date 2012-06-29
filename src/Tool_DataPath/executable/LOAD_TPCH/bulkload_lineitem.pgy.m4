/*
 * Bulkload lineitem table into DataPath
 */

data = READ "TABLE_DIR/`'ifelse(NUM_STRIPES, 1, `lineitem.tbl"', `lineitem.tbl.%d"' : NUM_STRIPES) SEPARATOR '|' ATTRIBUTES FROM lineitem;

STORE data INTO lineitem;
