--
-- Exercise query-finish flag in sort.
--
with t6a as(
	select
		l_skewkey,
		count(*) over (partition by l_skewkey order by l_quantity, l_orderkey)
	from skewed_lineitem
), t6b as (
	select * from skewed_lineitem
	order by l_orderkey
), t6c as (
	select l_skewkey, median(l_quantity) med
	from skewed_lineitem
	group by l_skewkey
)
select a.l_skewkey, b.l_orderkey,
	c.med
from t6a a inner join t6b b on a.l_skewkey = b.l_skewkey
inner join t6c c on b.l_skewkey = c.l_skewkey
order by 1, 2, 3
limit 2;
