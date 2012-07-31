
/* Drop Tables */

DROP TABLE IF EXISTS observation_file_managed;
DROP TABLE IF EXISTS public.observation_attribute;
DROP TABLE IF EXISTS public.observation;




/* Create Tables */

CREATE TABLE observation_file_managed
(
	observation_id int NOT NULL,
	-- File ID.
	file_managed_fid int NOT NULL,
	description text
) WITHOUT OIDS;


CREATE TABLE public.observation
(
	id serial NOT NULL,
	-- No description for column id available, please fix
	organism_id int NOT NULL,
	-- Id des Inventars
	inventory_id int,
	-- Primary Key: Unique user ID.
	users_uid bigint DEFAULT 0 NOT NULL,
	-- Primary Key
	area_geometry_id int NOT NULL,
	-- No description for column id available, please fix
	organism_artgroup_id int,
	-- No description for column id available, please fix
	organism_artgroup_detmethod_id int,
	observer text,
	count int,
	found_as_lang boolean,
	found_as_lat boolean,
	date int,
	c_time int,
	m_time int,
	acl_id serial,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.observation_attribute
(
	id serial NOT NULL,
	-- der boebachtung
	observation_id int NOT NULL,
	-- No description for column id available, please fix
	organism_artgroup_attr_id int,
	-- No description for column id available, please fix
	organism_artgroup_attr_values_id int,
	-- Der Inhalt des Attributes
	value text,
	PRIMARY KEY (id)
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE public.observation_attribute
	ADD FOREIGN KEY (observation_id)
	REFERENCES public.observation (id)
	ON UPDATE NO ACTION
	ON DELETE CASCADE
;


ALTER TABLE observation_file_managed
	ADD FOREIGN KEY (observation_id)
	REFERENCES public.observation_attribute (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;



/* Comments */

COMMENT ON COLUMN observation_file_managed.file_managed_fid IS 'File ID.';
COMMENT ON COLUMN public.observation.organism_id IS 'No description for column id available, please fix';
COMMENT ON COLUMN public.observation.inventory_id IS 'Id des Inventars';
COMMENT ON COLUMN public.observation.users_uid IS 'Primary Key: Unique user ID.';
COMMENT ON COLUMN public.observation.area_geometry_id IS 'Primary Key';
COMMENT ON COLUMN public.observation.organism_artgroup_id IS 'No description for column id available, please fix';
COMMENT ON COLUMN public.observation.organism_artgroup_detmethod_id IS 'No description for column id available, please fix';
COMMENT ON COLUMN public.observation_attribute.observation_id IS 'der boebachtung';
COMMENT ON COLUMN public.observation_attribute.organism_artgroup_attr_id IS 'No description for column id available, please fix';
COMMENT ON COLUMN public.observation_attribute.organism_artgroup_attr_values_id IS 'No description for column id available, please fix';
COMMENT ON COLUMN public.observation_attribute.value IS 'Der Inhalt des Attributes';



