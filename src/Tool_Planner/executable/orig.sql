SELECT SUM(l_extendedprice * (1 - l_discount)) AS revenue
FROM lineitem, orders
WHERE
l_orderkey = o_orderkey
AND o_orderdate >= '1997-02-01'
AND o_orderdate < '1997-05-07';

SELECT AVG(o_totalprice) AS agg
FROM orders, customer, nation
WHERE
o_custkey = c_custkey
AND o_orderdate > '1997-03-02'
AND o_orderdate < '1997-05-09'
AND n_nationkey = c_nationkey 
AND n_name = 'FRANCE' OR n_name = 'GERMANY';

SELECT COUNT(l_orderkey) AS cnt
FROM lineitem, orders, customer, nation
WHERE
o_custkey = c_custkey
AND l_orderkey = o_orderkey
AND c_nationkey = n_nationkey
AND n_name = 'ALGERIA'
AND o_orderdate >= '1997-03-01'
AND o_orderdate < '1997-04-07' ;

SELECT 
      100.00 * SUM(CASE
                WHEN p_type LIKE '%PROMO%'
                THEN l_extendedprice*(1-l_discount)
                ELSE 0
      END) / SUM(l_extendedprice * (1 - l_discount)) AS promo_revenue
FROM lineitem, part
WHERE
l_partkey = p_partkey
AND p_size = 13
AND p_brand LIKE 'Brand%';

SELECT AVG(l_extendedprice * (1 - l_discount)) AS agg
FROM lineitem, part, orders
WHERE
l_partkey = p_partkey
AND l_orderkey = o_orderkey
AND o_orderdate > '1997-2-20'
AND o_orderdate <= '1997-3-24'
AND p_container LIKE '%CAN%';

SELECT AVG(l_extendedprice) AS agg
FROM lineitem, partsupp, part, supplier
WHERE
ps_partkey = l_partkey
AND ps_suppkey = l_suppkey
AND ps_supplycost > 300 
AND ps_supplycost < 310
AND p_size > 5 
AND p_size < 10
AND p_partkey = ps_partkey
AND s_suppkey = ps_suppkey 
AND p_type LIKE '%BRUSH%' ;

SELECT 
      SUM(CASE
                WHEN n_name = 'JAPAN'
                THEN l_extendedprice*(1-l_discount)
                ELSE 0
      END) / SUM(l_extendedprice * (1 - l_discount)) AS margin_japan
FROM nation, region, lineitem, orders, customer, supplier
WHERE
l_orderkey = o_orderkey
AND c_custkey = o_custkey
AND s_nationkey = c_nationkey
AND s_nationkey = n_nationkey
AND n_regionkey = r_regionkey
AND o_orderdate > '1997-03-01'
AND o_orderdate < '1997-04-07'
AND r_name = 'ASIA' ;;
