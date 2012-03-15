
/* Drop Indexes */

DROP INDEX IF EXISTS area_hidden;
DROP INDEX IF EXISTS fki_area_id;



/* Drop Tables */

DROP TABLE IF EXISTS public.area_file_managed;
DROP TABLE IF EXISTS public.area_habitat;
DROP TABLE IF EXISTS public.area_parcel;
DROP TABLE IF EXISTS public.area;
DROP TABLE IF EXISTS public.area_type;




/* Create Tables */

CREATE TABLE public.area
(
	-- Primary Key
	id serial NOT NULL,
	-- Name, Flurname
	field_name text NOT NULL,
	-- Meter √É¬ºber Meer
	altitude int DEFAULT 0 NOT NULL,
	-- Fl√É¬§che in (m2)
	surface_area bigint DEFAULT 0 NOT NULL,
	locality text NOT NULL,
	zip text NOT NULL,
	township text NOT NULL,
	canton text NOT NULL,
	country text NOT NULL,
	-- Kommentar
	comment text,
	owner_id int,
	create_time timestamp with time zone,
	modify_time timestamp with time zone,
	-- Schutzziel
	protection_target text,
	-- Schutzmassnahmen
	safety_precautions text,
	-- Pflege- und Gestaltungsmassnahmen
	tending_strategies text,
	hidden boolean,
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


CREATE TABLE public.area_type
(
	-- PK
	id serial NOT NULL,
	-- Description
	description text NOT NULL,
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

CREATE INDEX area_hidden ON public.area USING BTREE (hidden);
CREATE INDEX fki_area_id ON public.area_parcel USING BTREE (area_id);



/* Comments */

COMMENT ON COLUMN public.area.id IS 'Primary Key';
COMMENT ON COLUMN public.area.field_name IS 'Name, Flurname';
COMMENT ON COLUMN public.area.altitude IS 'Meter √É¬ºber Meer';
COMMENT ON COLUMN public.area.surface_area IS 'Fl√É¬§che in (m2)';
COMMENT ON COLUMN public.area.comment IS 'Kommentar';
COMMENT ON COLUMN public.area.protection_target IS 'Schutzziel';
COMMENT ON COLUMN public.area.safety_precautions IS 'Schutzmassnahmen';
COMMENT ON COLUMN public.area.tending_strategies IS 'Pflege- und Gestaltungsmassnahmen';
COMMENT ON COLUMN public.area_file_managed.area_id IS 'Primary Key';
COMMENT ON COLUMN public.area_file_managed.file_id IS 'File ID.';
COMMENT ON COLUMN public.area_habitat.id IS 'PK';
COMMENT ON COLUMN public.area_habitat.area_id IS 'FK to area';
COMMENT ON COLUMN public.area_habitat.habitat_id IS 'PK';
COMMENT ON COLUMN public.area_parcel.area_id IS 'Primary Key';
COMMENT ON COLUMN public.area_type.id IS 'PK';
COMMENT ON COLUMN public.area_type.description IS 'Description';



