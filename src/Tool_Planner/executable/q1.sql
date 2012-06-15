SELECT SUM(l_extendedprice * (1 - l_discount)) AS revenue
FROM lineitem, orders
WHERE
l_orderkey = o_orderkey
AND o_orderdate >= DATE('1995-01-01')
AND o_orderdate < DATE('1995-02-01')
AND l_quantity > 300 ;;
