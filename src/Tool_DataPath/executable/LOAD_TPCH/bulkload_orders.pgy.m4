/*
 * Bulkload orders table into DataPath
 */

data = READ "TABLE_DIR/`'ifelse(NUM_STRIPES, 1, `orders.tbl"', `orders.tbl.%d"' : NUM_STRIPES) SEPARATOR '|' ATTRIBUTES FROM orders;

STORE data INTO orders;
