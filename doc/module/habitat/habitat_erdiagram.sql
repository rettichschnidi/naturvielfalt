
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
	name_scientific text,
	-- Bemerkungen zum Lebensraumtyp
	notes text,
	-- Not used so far - just in case we ever need this data again.
	name_de text,
	-- Not used so far - just in case we ever need this data again.
	name_it text,
	-- Not used so far - just in case we ever need this data again.
	name_fr text,
	PRIMARY KEY (id)
) WITHOUT OIDS;



/* Comments */

COMMENT ON COLUMN public.habitat.id IS 'PK';
COMMENT ON COLUMN public.habitat.label IS 'Label inkl. Hierarchie';
COMMENT ON COLUMN public.habitat.name IS 'name';
COMMENT ON COLUMN public.habitat.notes IS 'Bemerkungen zum Lebensraumtyp';
COMMENT ON COLUMN public.habitat.name_de IS 'Not used so far - just in case we ever need this data again.';
COMMENT ON COLUMN public.habitat.name_it IS 'Not used so far - just in case we ever need this data again.';
COMMENT ON COLUMN public.habitat.name_fr IS 'Not used so far - just in case we ever need this data again.';



