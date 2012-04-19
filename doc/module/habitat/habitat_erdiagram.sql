
/* Drop Tables */

DROP TABLE IF EXISTS public.habitat;



/* Drop Sequences */

DROP SEQUENCE IF EXISTS public.habitat_id_seq;




/* Create Tables */

CREATE TABLE public.habitat
(
	-- PK
	id serial NOT NULL,
	-- Label inkl. Hierarchie
	label text NOT NULL UNIQUE,
	-- name
	name text NOT NULL,
	-- Bemerkungen zum Lebensraumtyp
	notes text,
	PRIMARY KEY (id)
) WITHOUT OIDS;



/* Comments */

COMMENT ON COLUMN public.habitat.id IS 'PK';
COMMENT ON COLUMN public.habitat.label IS 'Label inkl. Hierarchie';
COMMENT ON COLUMN public.habitat.name IS 'name';
COMMENT ON COLUMN public.habitat.notes IS 'Bemerkungen zum Lebensraumtyp';



