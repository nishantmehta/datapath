SELECT SUM(CASE WHEN n_name = 'JAPAN' 
		THEN
			l_extendedprice*(1-l_discount)
		ELSE
			0
		END) / SUM(l_extendedprice*(1-l_discount) )
FROM nation, lineitem, orders, customer, supplier
WHERE
l_orderkey = o_orderkey
AND c_custkey = o_custkey
AND s_nationkey = c_nationkey
AND s_nationkey = n_nationkey
AND o_orderdate > DATE(1997,3,1)
AND o_orderdate < DATE(1997,4,7)
AND n_name = 'JAPAN' ;

;
