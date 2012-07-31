
/* Drop Tables */

DROP TABLE IF EXISTS public.inventory_entry_attribute_value;
DROP TABLE IF EXISTS public.inventory_entry;
DROP TABLE IF EXISTS public.inventory;
DROP TABLE IF EXISTS public.inventory_template_inventory_entry;
DROP TABLE IF EXISTS public.inventory_template_inventory;
DROP TABLE IF EXISTS public.inventory_template;
DROP TABLE IF EXISTS public.inventory_type_attribute_dropdown_value;
DROP TABLE IF EXISTS public.inventory_type_attribute;




/* Create Tables */

CREATE TABLE public.inventory
(
	-- Veraltet, PK wird ersetzt durch inventory_id
	id serial NOT NULL,
	-- veraltet: PK, wird erstert durch area_id
	area_id bigint NOT NULL,
	date_start date,
	date_end date,
	-- Name des Inventars
	name text NOT NULL,
	-- Kommentar
	comment text,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.inventory_entry
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
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.inventory_entry_attribute_value
(
	id serial NOT NULL,
	-- Id des Inventareintrags
	inventory_entry_id int NOT NULL,
	-- Falls es ein Dropdown ist, dann ist es die Id des Dropdownwertes
	inventory_type_attribute_dropdown_value_id int,
	-- Die Id des Attributs
	inventory_type_attribute_id int NOT NULL,
	-- Der Inhalt des Attributes
	value text,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.inventory_template
(
	-- The primary identifier for a template
	id serial NOT NULL,
	-- The name of the template
	name text,
	-- Long description of the tempalte
	description text,
	-- Whether this template is public or not
	public bigint DEFAULT 0::bigint,
	-- The users.uid of this template
	owner_id bigint NOT NULL,
	-- The date this template was created
	create_time date NOT NULL,
	-- The date this tempalte was last modified
	modify_time date NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.inventory_template_inventory
(
	-- The primary identifier for a template inventory
	id serial NOT NULL,
	-- The primary identifier for a template
	inventory_template_id int NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.inventory_template_inventory_entry
(
	-- The primary identifier for a template inventory entry
	id serial NOT NULL,
	-- The primary identifier for a template inventory
	inventory_template_inventory_id int NOT NULL,
	-- No description for column id available, please fix
	organism_id int NOT NULL,
	-- The position of this entry in the inventory
	position bigint NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.inventory_type_attribute
(
	id serial NOT NULL,
	-- Name des Inventars
	name text NOT NULL,
	-- Id des Attributformats
	attribute_format_id bigint NOT NULL,
	visible boolean,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.inventory_type_attribute_dropdown_value
(
	id serial NOT NULL,
	-- Id des Inventartyp Attributes
	inventory_type_attribute_id int NOT NULL,
	-- Werte des Dropdowneintrages
	value text NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE public.inventory_entry
	ADD FOREIGN KEY (inventory_id)
	REFERENCES public.inventory (id)
	ON UPDATE NO ACTION
	ON DELETE CASCADE
;


ALTER TABLE public.inventory_entry_attribute_value
	ADD CONSTRAINT 4 FOREIGN KEY (inventory_entry_id)
	REFERENCES public.inventory_entry (id)
	ON UPDATE NO ACTION
	ON DELETE CASCADE
;


ALTER TABLE public.inventory_template_inventory
	ADD FOREIGN KEY (inventory_template_id)
	REFERENCES public.inventory_template (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.inventory_template_inventory_entry
	ADD FOREIGN KEY (inventory_template_inventory_id)
	REFERENCES public.inventory_template_inventory (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.inventory_entry_attribute_value
	ADD CONSTRAINT inv_e_attr_val_2_inv_t_attr FOREIGN KEY (inventory_type_attribute_id)
	REFERENCES public.inventory_type_attribute (id)
	ON UPDATE CASCADE
	ON DELETE CASCADE
;


ALTER TABLE public.inventory_type_attribute_dropdown_value
	ADD CONSTRAINT 3 FOREIGN KEY (inventory_type_attribute_id)
	REFERENCES public.inventory_type_attribute (id)
	ON UPDATE NO ACTION
	ON DELETE CASCADE
;


ALTER TABLE public.inventory_entry_attribute_value
	ADD CONSTRAINT FK_i_e_a_v_2_i_t_a_d_v FOREIGN KEY (inventory_type_attribute_dropdown_value_id)
	REFERENCES public.inventory_type_attribute_dropdown_value (id)
	ON UPDATE NO ACTION
	ON DELETE SET NULL
;



/* Comments */

COMMENT ON COLUMN public.inventory.id IS 'Veraltet, PK wird ersetzt durch inventory_id';
COMMENT ON COLUMN public.inventory.area_id IS 'veraltet: PK, wird erstert durch area_id';
COMMENT ON COLUMN public.inventory.name IS 'Name des Inventars';
COMMENT ON COLUMN public.inventory.comment IS 'Kommentar';
COMMENT ON COLUMN public.inventory_entry.inventory_id IS 'Id des Inventars';
COMMENT ON COLUMN public.inventory_entry.users_uid IS 'Primary Key: Unique user ID.';
COMMENT ON COLUMN public.inventory_entry.organism_id IS 'No description for column id available, please fix';
COMMENT ON COLUMN public.inventory_entry.position IS 'Position in the inventory, Position in der Reihenfolge der Inevntareinträge';
COMMENT ON COLUMN public.inventory_entry.geom IS 'Localization as Postgis object';
COMMENT ON COLUMN public.inventory_entry.position_accuracy IS 'Genauigkeit in Meter.';
COMMENT ON COLUMN public.inventory_entry_attribute_value.inventory_entry_id IS 'Id des Inventareintrags';
COMMENT ON COLUMN public.inventory_entry_attribute_value.inventory_type_attribute_dropdown_value_id IS 'Falls es ein Dropdown ist, dann ist es die Id des Dropdownwertes';
COMMENT ON COLUMN public.inventory_entry_attribute_value.inventory_type_attribute_id IS 'Die Id des Attributs';
COMMENT ON COLUMN public.inventory_entry_attribute_value.value IS 'Der Inhalt des Attributes';
COMMENT ON COLUMN public.inventory_template.id IS 'The primary identifier for a template';
COMMENT ON COLUMN public.inventory_template.name IS 'The name of the template';
COMMENT ON COLUMN public.inventory_template.description IS 'Long description of the tempalte';
COMMENT ON COLUMN public.inventory_template.public IS 'Whether this template is public or not';
COMMENT ON COLUMN public.inventory_template.owner_id IS 'The users.uid of this template';
COMMENT ON COLUMN public.inventory_template.create_time IS 'The date this template was created';
COMMENT ON COLUMN public.inventory_template.modify_time IS 'The date this tempalte was last modified';
COMMENT ON COLUMN public.inventory_template_inventory.id IS 'The primary identifier for a template inventory';
COMMENT ON COLUMN public.inventory_template_inventory.inventory_template_id IS 'The primary identifier for a template';
COMMENT ON COLUMN public.inventory_template_inventory_entry.id IS 'The primary identifier for a template inventory entry';
COMMENT ON COLUMN public.inventory_template_inventory_entry.inventory_template_inventory_id IS 'The primary identifier for a template inventory';
COMMENT ON COLUMN public.inventory_template_inventory_entry.organism_id IS 'No description for column id available, please fix';
COMMENT ON COLUMN public.inventory_template_inventory_entry.position IS 'The position of this entry in the inventory';
COMMENT ON COLUMN public.inventory_type_attribute.name IS 'Name des Inventars';
COMMENT ON COLUMN public.inventory_type_attribute.attribute_format_id IS 'Id des Attributformats';
COMMENT ON COLUMN public.inventory_type_attribute_dropdown_value.inventory_type_attribute_id IS 'Id des Inventartyp Attributes';
COMMENT ON COLUMN public.inventory_type_attribute_dropdown_value.value IS 'Werte des Dropdowneintrages';



