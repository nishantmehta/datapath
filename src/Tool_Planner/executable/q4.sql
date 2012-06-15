SELECT 
      100.00 * SUM(CASE
                WHEN p_type LIKE 'PROMO%'
                THEN l_extendedprice*(1-l_discount)
                ELSE 0
      END) / SUM(l_extendedprice * (1 - l_discount)) AS promo_revenue
FROM lineitem, part
WHERE
l_partkey = p_partkey
AND l_shipdate >= '1993-01-01' 
AND l_shipdate <= '1993-04-01' 
AND p_brand LIKE 'Brand%13'
AND l_quantity > 300 ;;
