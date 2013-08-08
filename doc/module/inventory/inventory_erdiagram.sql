
/* Drop Tables */

DROP TABLE IF EXISTS public.inventory_file_managed;
DROP TABLE IF EXISTS public.inventory;




/* Create Tables */

CREATE TABLE public.inventory
(
	-- Veraltet, PK wird ersetzt durch inventory_id
	id serial NOT NULL,
	-- veraltet: PK, wird erstert durch area_id
	area_id bigint NOT NULL,
	-- Name des Inventars
	name text NOT NULL,
	-- Kommentar
	description text,
	users_uid bigint NOT NULL,
	observer text NOT NULL,
	create_time timestamp NOT NULL,
	modify_time timestamp,
	acl_id bigint NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.inventory_file_managed
(
	description text,
	-- Veraltet, PK wird ersetzt durch inventory_id
	inventory_id int NOT NULL,
	-- File ID.
	file_managed_id int NOT NULL
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE public.inventory_file_managed
	ADD FOREIGN KEY (inventory_id)
	REFERENCES public.inventory (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


/* Create Foreign Keys */

ALTER TABLE public.inventory
	ADD FOREIGN KEY (acl_id)
	REFERENCES public.acl (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


/* Comments */

COMMENT ON COLUMN public.inventory.id IS 'Veraltet, PK wird ersetzt durch inventory_id';
COMMENT ON COLUMN public.inventory.area_id IS 'veraltet: PK, wird erstert durch area_id';
COMMENT ON COLUMN public.inventory.name IS 'Name des Inventars';
COMMENT ON COLUMN public.inventory.description IS 'Kommentar';
COMMENT ON COLUMN public.inventory_file_managed.inventory_id IS 'Veraltet, PK wird ersetzt durch inventory_id';
COMMENT ON COLUMN public.inventory_file_managed.file_managed_id IS 'File ID.';



