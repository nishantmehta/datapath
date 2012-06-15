SELECT AVG(o_totalprice) AS agg
FROM orders, customer, nation
WHERE
o_custkey = c_custkey
AND o_orderdate > '1997-03-02'
AND o_orderdate < '1997-05-09'
AND n_nationkey = c_nationkey 
AND n_name = 'FRANCE' OR n_name = 'GERMANY';;
