
/* Drop Tables */

DROP TABLE IF EXISTS public.sgroup_users;
DROP TABLE IF EXISTS public.sgroup;




/* Create Tables */

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

ALTER TABLE public.sgroup_users
	ADD FOREIGN KEY (sgroup_id)
	REFERENCES public.sgroup (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;



/* Comments */

COMMENT ON TABLE public.sgroup IS 'Holds user groups';
COMMENT ON COLUMN public.sgroup.id IS 'Primary key for swissmon groups';
COMMENT ON COLUMN public.sgroup.name IS 'The name of the swissmon group';
COMMENT ON TABLE public.sgroup_users IS 'Links users to groups';
COMMENT ON COLUMN public.sgroup_users.admin IS '0 if this is only a member, 1 if it is an administrator of that group.';
COMMENT ON COLUMN public.sgroup_users.sgroup_id IS 'foreign key for swissmon group id';
COMMENT ON COLUMN public.sgroup_users.users_id IS 'foreign key for swissmon user id';



