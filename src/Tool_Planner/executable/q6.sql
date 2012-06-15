SELECT AVG(l_extendedprice) AS agg
FROM lineitem, part, supplier, nation
WHERE
s_suppkey = l_suppkey
AND p_size > 5
AND p_size < 10
AND p_partkey = l_partkey
AND p_type LIKE '.*BRUSH.*' 
AND n_nationkey = s_nationkey
AND n_name = 'RUSSIA' ;;
