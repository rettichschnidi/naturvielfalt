
/* Drop Tables */

DROP TABLE IF EXISTS acl_link;
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
CREATE TABLE acl_link
(
	-- primary key of linking table
	id bigserial NOT NULL,
	-- access level
	level smallint,
	-- Foreign key of acl id
	acl_id bigint DEFAULT 0,
	-- foreign key for swissmon user id
	users_id bigint DEFAULT 0,
	-- Foreign key of swissmon sgroup
	sgroup_id bigint DEFAULT 0,
	PRIMARY KEY (id)
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE acl_link
	ADD FOREIGN KEY (acl_id)
	REFERENCES acl (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;



/* Comments */

COMMENT ON TABLE acl IS 'ACL items';
COMMENT ON COLUMN acl.id IS 'Primary key for swissmon acl items';
COMMENT ON COLUMN acl.description IS 'desccription of acl item';
COMMENT ON COLUMN acl.users_id IS 'foreign key for swissmon user id who created acl item';
COMMENT ON TABLE acl_link IS 'Links acl items to users and groups';
COMMENT ON COLUMN acl_link.id IS 'primary key of linking table';
COMMENT ON COLUMN acl_link.level IS 'access level';
COMMENT ON COLUMN acl_link.acl_id IS 'Foreign key of acl id';
COMMENT ON COLUMN acl_link.users_id IS 'foreign key for swissmon user id';
COMMENT ON COLUMN acl_link.sgroup_id IS 'Foreign key of swissmon sgroup';



