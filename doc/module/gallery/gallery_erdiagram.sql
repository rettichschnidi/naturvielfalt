
/* Drop Indexes */

DROP INDEX IF EXISTS gallery_rating_type_role_id_idx;
DROP INDEX IF EXISTS gallery_rating_type_weight_idx;



/* Drop Tables */

DROP TABLE IF EXISTS public.gallery_category_condition;
DROP TABLE IF EXISTS public.gallery_image_category;
DROP TABLE IF EXISTS public.gallery_category_option;
DROP TABLE IF EXISTS public.gallery_category;
DROP TABLE IF EXISTS public.gallery_image_rating;
DROP TABLE IF EXISTS public.gallery_image_subtype;
DROP TABLE IF EXISTS public.gallery_image;
DROP TABLE IF EXISTS public.gallery_rating_type;




/* Create Tables */

CREATE TABLE public.gallery_category
(
	-- The primary identifier for a gallery category.
	id serial NOT NULL UNIQUE,
	-- The name of the category.
	name text,
	-- Long description of the categories.
	description text,
	-- Whether this category is required or not.
	required int,
	-- Whether multiple values may be selected for this category or not.
	multiple int,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.gallery_category_condition
(
	-- The primary identifier for a category condition.
	id serial NOT NULL UNIQUE,
	-- The primary identifier for a gallery category.
	category_id int NOT NULL UNIQUE,
	-- The name of the condition.
	name text,
	-- The name of the condition.
	condition text NOT NULL,
	-- The name of the column the value is checked against.
	columnname text,
	-- The value the required by the condition.
	value text,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.gallery_category_option
(
	-- The primary identifier for a category option.
	id serial NOT NULL UNIQUE,
	-- The primary identifier for a gallery category.
	gallery_category_id int NOT NULL UNIQUE,
	-- The name of the option.
	name text,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.gallery_image
(
	-- The primary identifier for a gallery image.
	id serial NOT NULL UNIQUE,
	-- File ID.
	file_managed_fid int NOT NULL,
	-- Primary Key: Unique user ID.
	users_uid bigint DEFAULT 0 NOT NULL,
	-- The type of object this image belongs to (inventory_entry, area, habitant, organism)
	item_type text NOT NULL,
	-- The title of the image.
	title text,
	-- Long description of the image.
	description text,
	-- The author of the image
	author text,
	-- The location of the image
	location text,
	-- Whether this image is visible for the public or not.
	visible int DEFAULT 1::bigint,
	-- The date this image was created
	created_date timestamp NOT NULL,
	-- The date this image was last modified
	modified_date timestamp NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.gallery_image_category
(
	-- The primary identifier for a gallery image category.
	id serial NOT NULL UNIQUE,
	-- The primary identifier for a gallery image.
	gallery_image_id int NOT NULL UNIQUE,
	-- The primary identifier for a category option.
	category_option_id int NOT NULL UNIQUE,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.gallery_image_rating
(
	-- The primary identifier for a gallery image rating.
	id serial NOT NULL UNIQUE,
	-- The primary identifier for a gallery image.
	gallery_image_id int NOT NULL UNIQUE,
	-- The primary identifier for a gallery image rating.
	gallery_rating_type_id int NOT NULL UNIQUE,
	-- Primary Key: Unique user ID.
	user_uid bigint DEFAULT 0 NOT NULL,
	-- The actual rating value.
	rating int,
	-- The IP address of the user who placed this rating.
	user_ip text NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.gallery_image_subtype
(
	-- The primary identifier for a gallery images subtype category.
	id serial NOT NULL UNIQUE,
	-- The primary identifier for a gallery image.
	gallery_image_id int NOT NULL UNIQUE,
	-- The subtype of the image.
	subtype text,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.gallery_rating_type
(
	-- The primary identifier for a gallery image rating.
	id serial NOT NULL UNIQUE,
	-- Primary Key: Unique role ID.
	rid int NOT NULL,
	-- The name of the rating type.
	name text,
	-- Long description of the rating type.
	description text,
	-- The role.rid required for placing ratings of this type.
	role_id bigint,
	-- The weight of this rating type in the global rating.
	weight int DEFAULT 1 NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE public.gallery_category_condition
	ADD FOREIGN KEY (category_id)
	REFERENCES public.gallery_category (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.gallery_category_option
	ADD FOREIGN KEY (gallery_category_id)
	REFERENCES public.gallery_category (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.gallery_image_category
	ADD FOREIGN KEY (category_option_id)
	REFERENCES public.gallery_category_option (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.gallery_image_category
	ADD FOREIGN KEY (gallery_image_id)
	REFERENCES public.gallery_image (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.gallery_image_rating
	ADD FOREIGN KEY (gallery_image_id)
	REFERENCES public.gallery_image (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.gallery_image_subtype
	ADD FOREIGN KEY (gallery_image_id)
	REFERENCES public.gallery_image (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.gallery_image_rating
	ADD FOREIGN KEY (gallery_rating_type_id)
	REFERENCES public.gallery_rating_type (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;



/* Create Indexes */

CREATE INDEX gallery_rating_type_role_id_idx ON public.gallery_rating_type USING BTREE (role_id);
CREATE INDEX gallery_rating_type_weight_idx ON public.gallery_rating_type USING BTREE (weight);



/* Comments */

COMMENT ON COLUMN public.gallery_category.id IS 'The primary identifier for a gallery category.';
COMMENT ON COLUMN public.gallery_category.name IS 'The name of the category.';
COMMENT ON COLUMN public.gallery_category.description IS 'Long description of the categories.';
COMMENT ON COLUMN public.gallery_category.required IS 'Whether this category is required or not.';
COMMENT ON COLUMN public.gallery_category.multiple IS 'Whether multiple values may be selected for this category or not.';
COMMENT ON COLUMN public.gallery_category_condition.id IS 'The primary identifier for a category condition.';
COMMENT ON COLUMN public.gallery_category_condition.category_id IS 'The primary identifier for a gallery category.';
COMMENT ON COLUMN public.gallery_category_condition.name IS 'The name of the condition.';
COMMENT ON COLUMN public.gallery_category_condition.condition IS 'The name of the condition.';
COMMENT ON COLUMN public.gallery_category_condition.columnname IS 'The name of the column the value is checked against.';
COMMENT ON COLUMN public.gallery_category_condition.value IS 'The value the required by the condition.';
COMMENT ON COLUMN public.gallery_category_option.id IS 'The primary identifier for a category option.';
COMMENT ON COLUMN public.gallery_category_option.gallery_category_id IS 'The primary identifier for a gallery category.';
COMMENT ON COLUMN public.gallery_category_option.name IS 'The name of the option.';
COMMENT ON COLUMN public.gallery_image.id IS 'The primary identifier for a gallery image.';
COMMENT ON COLUMN public.gallery_image.file_managed_fid IS 'File ID.';
COMMENT ON COLUMN public.gallery_image.users_uid IS 'Primary Key: Unique user ID.';
COMMENT ON COLUMN public.gallery_image.item_type IS 'The type of object this image belongs to (inventory_entry, area, habitant, organism)';
COMMENT ON COLUMN public.gallery_image.title IS 'The title of the image.';
COMMENT ON COLUMN public.gallery_image.description IS 'Long description of the image.';
COMMENT ON COLUMN public.gallery_image.author IS 'The author of the image';
COMMENT ON COLUMN public.gallery_image.location IS 'The location of the image';
COMMENT ON COLUMN public.gallery_image.visible IS 'Whether this image is visible for the public or not.';
COMMENT ON COLUMN public.gallery_image.created_date IS 'The date this image was created';
COMMENT ON COLUMN public.gallery_image.modified_date IS 'The date this image was last modified';
COMMENT ON COLUMN public.gallery_image_category.id IS 'The primary identifier for a gallery image category.';
COMMENT ON COLUMN public.gallery_image_category.gallery_image_id IS 'The primary identifier for a gallery image.';
COMMENT ON COLUMN public.gallery_image_category.category_option_id IS 'The primary identifier for a category option.';
COMMENT ON COLUMN public.gallery_image_rating.id IS 'The primary identifier for a gallery image rating.';
COMMENT ON COLUMN public.gallery_image_rating.gallery_image_id IS 'The primary identifier for a gallery image.';
COMMENT ON COLUMN public.gallery_image_rating.gallery_rating_type_id IS 'The primary identifier for a gallery image rating.';
COMMENT ON COLUMN public.gallery_image_rating.user_uid IS 'Primary Key: Unique user ID.';
COMMENT ON COLUMN public.gallery_image_rating.rating IS 'The actual rating value.';
COMMENT ON COLUMN public.gallery_image_rating.user_ip IS 'The IP address of the user who placed this rating.';
COMMENT ON COLUMN public.gallery_image_subtype.id IS 'The primary identifier for a gallery images subtype category.';
COMMENT ON COLUMN public.gallery_image_subtype.gallery_image_id IS 'The primary identifier for a gallery image.';
COMMENT ON COLUMN public.gallery_image_subtype.subtype IS 'The subtype of the image.';
COMMENT ON COLUMN public.gallery_rating_type.id IS 'The primary identifier for a gallery image rating.';
COMMENT ON COLUMN public.gallery_rating_type.rid IS 'Primary Key: Unique role ID.';
COMMENT ON COLUMN public.gallery_rating_type.name IS 'The name of the rating type.';
COMMENT ON COLUMN public.gallery_rating_type.description IS 'Long description of the rating type.';
COMMENT ON COLUMN public.gallery_rating_type.role_id IS 'The role.rid required for placing ratings of this type.';
COMMENT ON COLUMN public.gallery_rating_type.weight IS 'The weight of this rating type in the global rating.';



