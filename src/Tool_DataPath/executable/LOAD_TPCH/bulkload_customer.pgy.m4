/*
 * Bulkload customer table into DataPath
 */

data = READ "TABLE_DIR/`'ifelse(NUM_STRIPES, 1, `customer.tbl"', `customer.tbl.%d"' : NUM_STRIPES) SEPARATOR '|' ATTRIBUTES FROM customer;

STORE data INTO customer;
