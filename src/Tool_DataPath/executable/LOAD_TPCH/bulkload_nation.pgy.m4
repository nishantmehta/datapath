/*
 * Bulkload nation table into DataPath
 */

data = READ "TABLE_DIR/nation.tbl" SEPARATOR '|' ATTRIBUTES FROM nation;

STORE data INTO nation;
