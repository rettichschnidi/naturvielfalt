
/* Drop Tables */

DROP TABLE IF EXISTS sgroup_acl_link;
DROP TABLE IF EXISTS sgroup_acl;
DROP TABLE IF EXISTS public.sgroup_users;
DROP TABLE IF EXISTS public.sgroup;




/* Create Tables */

-- ACL items
CREATE TABLE sgroup_acl
(
	-- Primary key for swissmon acl items
	id bigserial NOT NULL UNIQUE,
	-- desccription of acl item
	description text,
	-- foreign key for swissmon user id who created acl item
	users_id bigint DEFAULT 0,
	PRIMARY KEY (id)
) WITHOUT OIDS;


-- Links acl items to users and groups
CREATE TABLE sgroup_acl_link
(
	-- access level
	level smallint,
	-- foreign key for swissmon user id
	users_id bigint DEFAULT 0,
	-- Foreign key of acl id
	acl_id bigint DEFAULT 0,
	-- Foreign key of swissmon sgroup
	sgroup_id bigint DEFAULT 0,
	-- primary key of linking table
	id bigserial NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


-- Holds user groups
CREATE TABLE public.sgroup
(
	-- Primary key for swissmon groups
	id bigserial NOT NULL UNIQUE,
	-- The name of the swissmon group
	name varchar(60) DEFAULT '''''::character varying' NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


-- Links users to groups
CREATE TABLE public.sgroup_users
(
	-- 0 if this is only a member, 1 if it is an administrator of that group.
	admin int DEFAULT 0,
	-- foreign key for swissmon group id
	sgroup_id bigint NOT NULL,
	-- foreign key for swissmon user id
	users_id bigint DEFAULT 0 NOT NULL
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE sgroup_acl_link
	ADD FOREIGN KEY (acl_id)
	REFERENCES sgroup_acl (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE sgroup_acl_link
	ADD FOREIGN KEY (sgroup_id)
	REFERENCES public.sgroup (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.sgroup_users
	ADD FOREIGN KEY (sgroup_id)
	REFERENCES public.sgroup (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;



/* Comments */

COMMENT ON TABLE sgroup_acl IS 'ACL items';
COMMENT ON COLUMN sgroup_acl.id IS 'Primary key for swissmon acl items';
COMMENT ON COLUMN sgroup_acl.description IS 'desccription of acl item';
COMMENT ON COLUMN sgroup_acl.users_id IS 'foreign key for swissmon user id who created acl item';
COMMENT ON TABLE sgroup_acl_link IS 'Links acl items to users and groups';
COMMENT ON COLUMN sgroup_acl_link.level IS 'access level';
COMMENT ON COLUMN sgroup_acl_link.users_id IS 'foreign key for swissmon user id';
COMMENT ON COLUMN sgroup_acl_link.acl_id IS 'Foreign key of acl id';
COMMENT ON COLUMN sgroup_acl_link.sgroup_id IS 'Foreign key of swissmon sgroup';
COMMENT ON COLUMN sgroup_acl_link.id IS 'primary key of linking table';
COMMENT ON TABLE public.sgroup IS 'Holds user groups';
COMMENT ON COLUMN public.sgroup.id IS 'Primary key for swissmon groups';
COMMENT ON COLUMN public.sgroup.name IS 'The name of the swissmon group';
COMMENT ON TABLE public.sgroup_users IS 'Links users to groups';
COMMENT ON COLUMN public.sgroup_users.admin IS '0 if this is only a member, 1 if it is an administrator of that group.';
COMMENT ON COLUMN public.sgroup_users.sgroup_id IS 'foreign key for swissmon group id';
COMMENT ON COLUMN public.sgroup_users.users_id IS 'foreign key for swissmon user id';



