/*
 * Bulkload region table into DataPath
 */

data = READ "TABLE_DIR/region.tbl" SEPARATOR '|' ATTRIBUTES FROM region;

STORE data INTO region;
