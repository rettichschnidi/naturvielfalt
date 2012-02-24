
/* Drop Tables */

DROP TABLE IF EXISTS acl_sgroup;
DROP TABLE IF EXISTS acl_users;
DROP TABLE IF EXISTS public.sgroup;
DROP TABLE IF EXISTS acl;




/* Create Tables */

-- ACL items
CREATE TABLE acl
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
CREATE TABLE acl_sgroup
(
	-- primary key of linking table
	id bigserial NOT NULL UNIQUE,
	-- access level
	level smallint,
	-- foreign key of natutvielfalt acl_id
	acl_id bigint NOT NULL,
	-- Foreign key of swissmon sgroup
	sgroup_id bigint NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


-- Links acl items to users and groups
CREATE TABLE acl_users
(
	-- primary key of linking table
	id bigserial NOT NULL UNIQUE,
	-- access level
	level smallint,
	-- Foreign key of acl id
	acl_id bigint DEFAULT 0 NOT NULL,
	-- foreign key for swissmon user id
	users_id bigint DEFAULT 0,
	PRIMARY KEY (id)
) WITHOUT OIDS;


-- Holds user groups
CREATE TABLE public.sgroup
(
	-- Primary key for swissmon groups
	id bigserial NOT NULL UNIQUE,
	-- The name of the swissmon group
	name varchar(60) DEFAULT '''''::character varying' NOT NULL,
	-- description of group
	description text,
	-- foreign key of natutvielfalt acl_id
	acl_id bigint NOT NULL,
	-- Foreign key of naturviefalt user_id (indicates owner of group)
	users_id bigint DEFAULT 0,
	PRIMARY KEY (id)
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE acl_sgroup
	ADD FOREIGN KEY (acl_id)
	REFERENCES acl (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE acl_users
	ADD FOREIGN KEY (acl_id)
	REFERENCES acl (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.sgroup
	ADD FOREIGN KEY (acl_id)
	REFERENCES acl (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE acl_sgroup
	ADD FOREIGN KEY (sgroup_id)
	REFERENCES public.sgroup (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;



/* Comments */

COMMENT ON TABLE acl IS 'ACL items';
COMMENT ON COLUMN acl.id IS 'Primary key for swissmon acl items';
COMMENT ON COLUMN acl.description IS 'desccription of acl item';
COMMENT ON COLUMN acl.users_id IS 'foreign key for swissmon user id who created acl item';
COMMENT ON TABLE acl_sgroup IS 'Links acl items to users and groups';
COMMENT ON COLUMN acl_sgroup.id IS 'primary key of linking table';
COMMENT ON COLUMN acl_sgroup.level IS 'access level';
COMMENT ON COLUMN acl_sgroup.acl_id IS 'foreign key of natutvielfalt acl_id';
COMMENT ON COLUMN acl_sgroup.sgroup_id IS 'Foreign key of swissmon sgroup';
COMMENT ON TABLE acl_users IS 'Links acl items to users and groups';
COMMENT ON COLUMN acl_users.id IS 'primary key of linking table';
COMMENT ON COLUMN acl_users.level IS 'access level';
COMMENT ON COLUMN acl_users.acl_id IS 'Foreign key of acl id';
COMMENT ON COLUMN acl_users.users_id IS 'foreign key for swissmon user id';
COMMENT ON TABLE public.sgroup IS 'Holds user groups';
COMMENT ON COLUMN public.sgroup.id IS 'Primary key for swissmon groups';
COMMENT ON COLUMN public.sgroup.name IS 'The name of the swissmon group';
COMMENT ON COLUMN public.sgroup.description IS 'description of group';
COMMENT ON COLUMN public.sgroup.acl_id IS 'foreign key of natutvielfalt acl_id';
COMMENT ON COLUMN public.sgroup.users_id IS 'Foreign key of naturviefalt user_id (indicates owner of group)';



