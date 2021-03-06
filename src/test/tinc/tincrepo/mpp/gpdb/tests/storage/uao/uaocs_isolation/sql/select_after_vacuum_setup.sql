-- start_ignore
SET gp_create_table_random_default_distribution=off;
-- end_ignore
DROP TABLE IF EXISTS ao;
DROP TABLE IF EXISTS ao2;

CREATE TABLE ao2 (a INT) WITH (appendonly=true, orientation=column);
CREATE TABLE ao (a INT) WITH (appendonly=true, orientation=column);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao select generate_series(1,1000);
insert into ao2 select generate_series(1,1000);

DROP VIEW IF EXISTS locktest;
create view locktest as select coalesce(
	case when relname like 'pg_toast%index' then 'toast index'
	when relname like 'pg_toast%' then 'toast table'
	else relname end, 'dropped table'),
	mode,
	locktype from
	pg_locks l left outer join pg_class c on (l.relation = c.oid),
	pg_database d where relation is not null and l.database = d.oid and
	l.gp_segment_id = -1 and
	relname != 'gp_fault_strategy' and
	d.datname = current_database() order by 1, 3, 2;
