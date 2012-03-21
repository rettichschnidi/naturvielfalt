
/* Drop Tables */

DROP TABLE IF EXISTS public.observation_attribute_value;
DROP TABLE IF EXISTS public.observation;
DROP TABLE IF EXISTS public.observation_type_attribute_dropdown_value;
DROP TABLE IF EXISTS public.observation_type_attribute;




/* Create Tables */

CREATE TABLE public.observation
(
	id serial NOT NULL,
	-- Id des Inventars
	inventory_id int,
	-- Primary Key: Unique user ID.
	users_uid bigint DEFAULT 0 NOT NULL,
	-- No description for column id available, please fix
	organism_id int NOT NULL,
	-- Position in the inventory, Position in der Reihenfolge der Inevntareinträge
	position bigint,
	-- Localization as Postgis object
	geom geometry,
	locality text DEFAULT '''''::character varying' NOT NULL,
	zip text DEFAULT '''''::character varying' NOT NULL,
	township text DEFAULT '''''::character varying' NOT NULL,
	canton text DEFAULT '''''::character varying' NOT NULL,
	country text DEFAULT '''''::character varying' NOT NULL,
	-- Genauigkeit in Meter.
	position_accuracy bigint,
	observation_time date,
	acl_id serial,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.observation_attribute_value
(
	id serial NOT NULL,
	-- der boebachtung
	observation_id int NOT NULL,
	-- Falls es ein Dropdown ist, dann ist es die Id des Dropdownwertes
	observation_type_attribute_dropdown_value_id int,
	-- Die Id des Attributs
	observation_type_attribute_id int NOT NULL,
	-- Der Inhalt des Attributes
	value text,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.observation_type_attribute
(
	id serial NOT NULL,
	-- Name des Inventars
	name text NOT NULL,
	-- Id des Attributformats
	attribute_format_id bigint NOT NULL,
	visible boolean,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.observation_type_attribute_dropdown_value
(
	id serial NOT NULL,
	-- Id des Inventartyp Attributes
	observation_type_attribute_id int NOT NULL,
	-- Werte des Dropdowneintrages
	value text NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE public.observation_attribute_value
	ADD CONSTRAINT 4 FOREIGN KEY (observation_id)
	REFERENCES public.observation (id)
	ON UPDATE NO ACTION
	ON DELETE CASCADE
;


ALTER TABLE public.observation_attribute_value
	ADD CONSTRAINT inv_e_attr_val_2_inv_t_attr FOREIGN KEY (observation_type_attribute_id)
	REFERENCES public.observation_type_attribute (id)
	ON UPDATE CASCADE
	ON DELETE CASCADE
;


ALTER TABLE public.observation_type_attribute_dropdown_value
	ADD FOREIGN KEY (observation_type_attribute_id)
	REFERENCES public.observation_type_attribute (id)
	ON UPDATE NO ACTION
	ON DELETE CASCADE
;


ALTER TABLE public.observation_attribute_value
	ADD CONSTRAINT FK_i_e_a_v_2_i_t_a_d_v FOREIGN KEY (observation_type_attribute_dropdown_value_id)
	REFERENCES public.observation_type_attribute_dropdown_value (id)
	ON UPDATE NO ACTION
	ON DELETE SET NULL
;



/* Comments */

COMMENT ON COLUMN public.observation.inventory_id IS 'Id des Inventars';
COMMENT ON COLUMN public.observation.users_uid IS 'Primary Key: Unique user ID.';
COMMENT ON COLUMN public.observation.organism_id IS 'No description for column id available, please fix';
COMMENT ON COLUMN public.observation.position IS 'Position in the inventory, Position in der Reihenfolge der Inevntareinträge';
COMMENT ON COLUMN public.observation.geom IS 'Localization as Postgis object';
COMMENT ON COLUMN public.observation.position_accuracy IS 'Genauigkeit in Meter.';
COMMENT ON COLUMN public.observation_attribute_value.observation_id IS 'der boebachtung';
COMMENT ON COLUMN public.observation_attribute_value.observation_type_attribute_dropdown_value_id IS 'Falls es ein Dropdown ist, dann ist es die Id des Dropdownwertes';
COMMENT ON COLUMN public.observation_attribute_value.observation_type_attribute_id IS 'Die Id des Attributs';
COMMENT ON COLUMN public.observation_attribute_value.value IS 'Der Inhalt des Attributes';
COMMENT ON COLUMN public.observation_type_attribute.name IS 'Name des Inventars';
COMMENT ON COLUMN public.observation_type_attribute.attribute_format_id IS 'Id des Attributformats';
COMMENT ON COLUMN public.observation_type_attribute_dropdown_value.observation_type_attribute_id IS 'Id des Inventartyp Attributes';
COMMENT ON COLUMN public.observation_type_attribute_dropdown_value.value IS 'Werte des Dropdowneintrages';



