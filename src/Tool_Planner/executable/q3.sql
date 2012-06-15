SELECT COUNT(l_orderkey) AS cnt
FROM lineitem, orders, customer, nation
WHERE
o_custkey = c_custkey
AND l_orderkey = o_orderkey
AND c_nationkey = n_nationkey
AND n_name = 'ALGERIA'
AND o_orderdate >= DATE(1995,1,1)
AND o_orderdate < DATE(1997,1,1);;


