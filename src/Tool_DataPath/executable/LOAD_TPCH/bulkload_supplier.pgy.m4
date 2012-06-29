/*
 * Bulkload supplier table into DataPath
 */

data = READ "TABLE_DIR/`'ifelse(NUM_STRIPES, 1, `supplier.tbl"', `supplier.tbl.%d"' : NUM_STRIPES) SEPARATOR '|' ATTRIBUTES FROM supplier;

STORE data INTO supplier;
