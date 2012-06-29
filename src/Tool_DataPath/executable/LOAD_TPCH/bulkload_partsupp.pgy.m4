/*
 * Bulkload partsupp table into DataPath
 */

data = READ "TABLE_DIR/`'ifelse(NUM_STRIPES, 1, `partsupp.tbl"', `partsupp.tbl.%d"' : NUM_STRIPES) SEPARATOR '|' ATTRIBUTES FROM partsupp;

STORE data INTO partsupp;
