SELECT COUNT(o_orderkey) 
FROM customer, orders 
WHERE
c_custkey = o_custkey
AND o_comment LIKE '.*unusual.*';

SELECT SUM(l_extendedprice * (1 - l_discount)) AS revenue 
FROM customer, orders, lineitem 
WHERE
c_custkey = o_custkey AND
o_orderkey = l_orderkey AND
c_mktsegment = 'AUTOMOBILE' AND
o_orderdate < '1994-12-23' AND
l_shipdate > '1994-12-23' ;

SELECT SUM(l_extendedprice) AS volume
FROM lineitem, orders, customer, supplier, nation
WHERE
c_custkey = o_custkey
AND o_orderkey = l_orderkey 
AND s_suppkey = l_suppkey 
AND c_nationkey = n_nationkey
AND n_name = 'ALGERIA'
AND s_comment LIKE '.*Complaint.*'  ;

SELECT COUNT(o_orderkey) AS cnt
FROM orders, customer, nation
WHERE
o_custkey = c_custkey
AND o_orderdate <= '1994-01-01'
AND o_orderstatus = 'F'
AND n_nationkey = c_nationkey 
AND n_name = 'FRANCE' OR n_name = 'GERMANY';

SELECT SUM(l_extendedprice) AS sum_extend
FROM lineitem, orders, customer, nation
WHERE
o_custkey = c_custkey
AND l_orderkey = o_orderkey
AND c_nationkey = n_nationkey
AND n_name = 'FRANCE' OR n_name = 'GERMANY'
AND o_orderdate <= '1993-06-01';

SELECT AVG(l_extendedprice) AS agg1
FROM lineitem, orders
WHERE
l_orderkey = o_orderkey
AND l_quantity > 300 ;

SELECT AVG(l_discount) AS agg2
FROM lineitem, part, customer, orders, nation, region
WHERE
o_custkey = c_custkey
AND l_orderkey = o_orderkey
AND l_partkey = p_partkey
AND c_nationkey = n_nationkey
AND n_regionkey = r_regionkey
AND r_name = 'ASIA' 
AND p_type LIKE '.*COPPER.*'
AND l_quantity < 8 ;

SELECT SUM(l_extendedprice * (1 - l_discount)) AS revenue
FROM nation, region, lineitem, orders, customer, supplier
WHERE
l_orderkey = o_orderkey
AND c_custkey = o_custkey
AND s_nationkey = c_nationkey
AND s_nationkey = n_nationkey
AND n_regionkey = r_regionkey
AND l_shipdate >= '1995-01-01'
AND o_orderdate < '1995-02-01'
AND r_name = 'EUROPE' ;;
