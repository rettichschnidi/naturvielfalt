
/* Drop Indexes */

DROP INDEX IF EXISTS fki_area_id;



/* Drop Tables */

DROP TABLE IF EXISTS public.area_file_managed;
DROP TABLE IF EXISTS public.area_habitat;
DROP TABLE IF EXISTS public.area_parcel;
DROP TABLE IF EXISTS public.area;




/* Create Tables */

CREATE TABLE public.area
(
	-- Primary Key
	id serial NOT NULL,
	-- Primary key for swissmon acl items
	acl_id int NOT NULL,
	-- Name, Flurname
	name text NOT NULL,
	-- Meter über Meer
	altitude int DEFAULT 0 NOT NULL,
	-- Fläche in m²
	surface_area bigint DEFAULT 0 NOT NULL,
	zip text NOT NULL,
	township text NOT NULL,
	canton text NOT NULL,
	country text NOT NULL,
	-- Kommentartext
	commenttext text,
	create_time timestamp,
	modify_time timestamp,
	-- Schutzziel
	protection_target text,
	-- Schutzmassnahmen
	safety_precautions text,
	-- Pflege- und Gestaltungsmassnahmen
	tending_strategies text,
	geom geometry,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.area_file_managed
(
	-- Primary Key
	area_id int,
	-- File ID.
	file_id int,
	description text
) WITHOUT OIDS;


CREATE TABLE public.area_habitat
(
	-- PK
	id serial NOT NULL,
	-- FK to area
	area_id int NOT NULL,
	-- PK
	habitat_id int NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.area_parcel
(
	id serial NOT NULL,
	-- Primary Key
	area_id int,
	parcel_owner_name text,
	parcel text,
	PRIMARY KEY (id)
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE public.area_file_managed
	ADD CONSTRAINT area_file_managed_ibfk_1 FOREIGN KEY (area_id)
	REFERENCES public.area (id)
	ON UPDATE NO ACTION
	ON DELETE CASCADE
;


ALTER TABLE public.area_habitat
	ADD CONSTRAINT area_habitat_ibfk_2 FOREIGN KEY (area_id)
	REFERENCES public.area (id)
	ON UPDATE NO ACTION
	ON DELETE CASCADE
;


ALTER TABLE public.area_parcel
	ADD CONSTRAINT area_parcel_ibfk_1 FOREIGN KEY (area_id)
	REFERENCES public.area (id)
	ON UPDATE NO ACTION
	ON DELETE CASCADE
;



/* Create Indexes */

CREATE INDEX fki_area_id ON public.area_parcel USING BTREE (area_id);



/* Comments */

COMMENT ON COLUMN public.area.id IS 'Primary Key';
COMMENT ON COLUMN public.area.acl_id IS 'Primary key for swissmon acl items';
COMMENT ON COLUMN public.area.name IS 'Name, Flurname';
COMMENT ON COLUMN public.area.altitude IS 'Meter über Meer';
COMMENT ON COLUMN public.area.surface_area IS 'Fläche in m²';
COMMENT ON COLUMN public.area.commenttext IS 'Kommentartext';
COMMENT ON COLUMN public.area.protection_target IS 'Schutzziel';
COMMENT ON COLUMN public.area.safety_precautions IS 'Schutzmassnahmen';
COMMENT ON COLUMN public.area.tending_strategies IS 'Pflege- und Gestaltungsmassnahmen';
COMMENT ON COLUMN public.area_file_managed.area_id IS 'Primary Key';
COMMENT ON COLUMN public.area_file_managed.file_id IS 'File ID.';
COMMENT ON COLUMN public.area_habitat.id IS 'PK';
COMMENT ON COLUMN public.area_habitat.area_id IS 'FK to area';
COMMENT ON COLUMN public.area_habitat.habitat_id IS 'PK';
COMMENT ON COLUMN public.area_parcel.area_id IS 'Primary Key';



