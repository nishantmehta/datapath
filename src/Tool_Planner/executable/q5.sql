SELECT AVG(l_extendedprice * (1 - l_discount)) AS agg
FROM lineitem, part, orders
WHERE
l_partkey = p_partkey
AND l_orderkey = o_orderkey
AND l_shipdate >= '1997-01-01'
AND o_orderdate > '1996-12-20'
AND o_orderdate <= '1996-12-24'
AND p_container LIKE 'SM%CAN'
AND l_quantity < 8 ;;
