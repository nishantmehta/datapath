SELECT SUM(l_extendedprice * (1 - l_discount)) AS revenue
FROM lineitem, orders
WHERE
l_orderkey = o_orderkey
AND o_orderdate >= DATE(1997,2,1)
AND o_orderdate < DATE(1997,5,7);
;
