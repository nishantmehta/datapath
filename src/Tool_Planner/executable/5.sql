SELECT AVG(l_extendedprice * (1 - l_discount)) AS agg
FROM lineitem, part, orders
WHERE
l_partkey = p_partkey
AND l_orderkey = o_orderkey
AND o_orderdate > DATE(1997,2,20)
AND o_orderdate <= DATE(1997,3,24)
AND p_container LIKE '.*CAN.*';
;
