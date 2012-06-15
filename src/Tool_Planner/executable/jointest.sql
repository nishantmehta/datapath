SELECT SUM(l_extendedprice * (1 - l_discount)) AS revenue FROM lineitem, orders WHERE l_orderkey = o_orderkey AND o_orderkey < 2000000000 AND l_orderkey < 2000000000;
;
