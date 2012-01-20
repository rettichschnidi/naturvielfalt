
/* Drop Indexes */

DROP INDEX IF EXISTS primaryKey;
DROP INDEX IF EXISTS left_value;
DROP INDEX IF EXISTS left_value;
DROP INDEX IF EXISTS organism_id;
DROP INDEX IF EXISTS value;
DROP INDEX IF EXISTS FK_organism_11;
DROP INDEX IF EXISTS FK_organism_2;
DROP INDEX IF EXISTS idx_name_de;
DROP INDEX IF EXISTS fki_organism_file_managed_ibfk_1;
DROP INDEX IF EXISTS fki_organism_file_managed_ibfk_2;
DROP INDEX IF EXISTS organism_habitat_habitat_id_idx;
DROP INDEX IF EXISTS organism_habitat_organism_id_idx;



/* Drop Tables */

DROP TABLE IF EXISTS organism_attribute_lang;
DROP TABLE IF EXISTS organism_attribute_value;
DROP TABLE IF EXISTS organism_classification_lang;
DROP TABLE IF EXISTS organism_classification_subscription;
DROP TABLE IF EXISTS organism_classification_value_lang;
DROP TABLE IF EXISTS organism_classification;
DROP TABLE IF EXISTS organism_classifier;
DROP TABLE IF EXISTS organism_lang;
DROP TABLE IF EXISTS organism_scientific_name;
DROP TABLE IF EXISTS public.organism_attribute;
DROP TABLE IF EXISTS public.organism_file_managed;
DROP TABLE IF EXISTS public.organism_habitat_subscription;
DROP TABLE IF EXISTS public.organism;




/* Create Tables */

CREATE TABLE organism_attribute_lang
(
	-- Language code, e.g. 'de' or 'en-US'.
	languages_language varchar(12) DEFAULT '''''::character varying' NOT NULL,
	organism_attribute_id int NOT NULL,
	value text NOT NULL,
	UNIQUE (languages_language, organism_attribute_id)
) WITHOUT OIDS;


CREATE TABLE organism_attribute_value
(
	organism_attribute_id int NOT NULL UNIQUE,
	text_value text,
	boolean_value boolean,
	number_value int
) WITHOUT OIDS;


CREATE TABLE organism_classification
(
	id serial NOT NULL UNIQUE,
	-- If this classifier is on top level, then parent_id == 0
	parent_id int,
	organism_classifier_id int NOT NULL,
	left_value int DEFAULT 1 NOT NULL,
	right_value int DEFAULT 2 NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_classification_lang
(
	organism_classificaton_id int NOT NULL,
	-- Language code, e.g. 'de' or 'en-US'.
	languages_language varchar(12) DEFAULT '''''::character varying' NOT NULL,
	value text NOT NULL,
	UNIQUE (languages_language, value),
	UNIQUE (organism_classificaton_id, languages_language, value)
) WITHOUT OIDS;


CREATE TABLE organism_classification_subscription
(
	organism_id int NOT NULL,
	organism_classification_id int NOT NULL,
	UNIQUE (organism_classification_id, organism_id)
) WITHOUT OIDS;


CREATE TABLE organism_classification_value_lang
(
	organism_classification_id int NOT NULL,
	-- Language code, e.g. 'de' or 'en-US'.
	languages_language varchar(12) DEFAULT '''''::character varying' NOT NULL,
	value text NOT NULL,
	UNIQUE (organism_classification_id, languages_language, value)
) WITHOUT OIDS;


CREATE TABLE organism_classifier
(
	id serial NOT NULL UNIQUE,
	-- Author, discoverer, book, etc.
	name text NOT NULL UNIQUE,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_lang
(
	id serial NOT NULL UNIQUE,
	-- Language code, e.g. 'de' or 'en-US'.
	languages_language varchar(12) DEFAULT '''''::character varying' NOT NULL,
	organism_id int NOT NULL,
	name text NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_scientific_name
(
	organism_id int NOT NULL,
	value text NOT NULL UNIQUE
) WITHOUT OIDS;


CREATE TABLE public.organism
(
	id serial NOT NULL UNIQUE,
	-- If this organism is not part of an aggregated organism, then parent_id == id
	parent_id int,
	-- Used to build a hierarchically class
	left_value int DEFAULT 1 NOT NULL,
	-- Used to build a hierarchically class
	right_value int DEFAULT 2 NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.organism_attribute
(
	id serial NOT NULL UNIQUE,
	organism_id int NOT NULL,
	-- Specify, which type this value this attribute has and thus which column is set in organism_attribute_value. t = text, b = boolean, n = number
	valuetype char NOT NULL,
	optional boolean NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.organism_file_managed
(
	organism_id int NOT NULL,
	-- file_managed_id
	file_managed_id int NOT NULL,
	description text,
	-- Stores information about the author of the document
	author text,
	UNIQUE (organism_id, file_managed_id)
) WITHOUT OIDS;


CREATE TABLE public.organism_habitat_subscription
(
	-- FK to organism.id
	organism_id int NOT NULL,
	-- FK to habitat.id
	habitat_id bigint NOT NULL,
	-- True if this organism is a characteristic organism (Leitorganismus) for this habitat.
	characteristic_organism boolean NOT NULL,
	UNIQUE (organism_id, habitat_id)
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE organism_classification
	ADD FOREIGN KEY (parent_id)
	REFERENCES organism_classification (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification_lang
	ADD FOREIGN KEY (organism_classificaton_id)
	REFERENCES organism_classification (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification_subscription
	ADD FOREIGN KEY (organism_id)
	REFERENCES organism_classification (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification_value_lang
	ADD FOREIGN KEY (organism_classification_id)
	REFERENCES organism_classification (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification
	ADD FOREIGN KEY (organism_classifier_id)
	REFERENCES organism_classifier (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification_subscription
	ADD FOREIGN KEY (organism_classification_id)
	REFERENCES public.organism (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_lang
	ADD FOREIGN KEY (organism_id)
	REFERENCES public.organism (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_scientific_name
	ADD FOREIGN KEY (organism_id)
	REFERENCES public.organism (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.organism
	ADD FOREIGN KEY (parent_id)
	REFERENCES public.organism (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.organism_attribute
	ADD FOREIGN KEY (organism_id)
	REFERENCES public.organism (id)
	ON UPDATE NO ACTION
	ON DELETE NO ACTION
;


ALTER TABLE public.organism_file_managed
	ADD FOREIGN KEY (organism_id)
	REFERENCES public.organism (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.organism_habitat_subscription
	ADD FOREIGN KEY (organism_id)
	REFERENCES public.organism (id)
	ON UPDATE NO ACTION
	ON DELETE CASCADE
;


ALTER TABLE organism_attribute_lang
	ADD FOREIGN KEY (organism_attribute_id)
	REFERENCES public.organism_attribute (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_attribute_value
	ADD FOREIGN KEY (organism_attribute_id)
	REFERENCES public.organism_attribute (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;



/* Create Indexes */

CREATE INDEX primaryKey ON organism_classification USING BTREE (id);
CREATE INDEX left_value ON organism_classification (left_value);
CREATE INDEX left_value ON organism_classification (right_value);
CREATE INDEX organism_id ON organism_scientific_name (organism_id);
CREATE INDEX value ON organism_scientific_name (value);
CREATE INDEX FK_organism_11 ON public.organism USING BTREE (id);
CREATE INDEX FK_organism_2 ON public.organism USING BTREE (left_value);
CREATE INDEX idx_name_de ON public.organism USING BTREE (right_value);
CREATE INDEX fki_organism_file_managed_ibfk_1 ON public.organism_file_managed (organism_id);
CREATE INDEX fki_organism_file_managed_ibfk_2 ON public.organism_file_managed (file_managed_id);
CREATE INDEX organism_habitat_habitat_id_idx ON public.organism_habitat_subscription (habitat_id);
CREATE INDEX organism_habitat_organism_id_idx ON public.organism_habitat_subscription (organism_id);



/* Comments */

COMMENT ON COLUMN organism_attribute_lang.languages_language IS 'Language code, e.g. ''de'' or ''en-US''.';
COMMENT ON COLUMN organism_classification.parent_id IS 'If this classifier is on top level, then parent_id == 0';
COMMENT ON COLUMN organism_classification_lang.languages_language IS 'Language code, e.g. ''de'' or ''en-US''.';
COMMENT ON COLUMN organism_classification_value_lang.languages_language IS 'Language code, e.g. ''de'' or ''en-US''.';
COMMENT ON COLUMN organism_classifier.name IS 'Author, discoverer, book, etc.';
COMMENT ON COLUMN organism_lang.languages_language IS 'Language code, e.g. ''de'' or ''en-US''.';
COMMENT ON COLUMN public.organism.parent_id IS 'If this organism is not part of an aggregated organism, then parent_id == id';
COMMENT ON COLUMN public.organism.left_value IS 'Used to build a hierarchically class';
COMMENT ON COLUMN public.organism.right_value IS 'Used to build a hierarchically class';
COMMENT ON COLUMN public.organism_attribute.valuetype IS 'Specify, which type this value this attribute has and thus which column is set in organism_attribute_value. t = text, b = boolean, n = number';
COMMENT ON COLUMN public.organism_file_managed.file_managed_id IS 'file_managed_id';
COMMENT ON COLUMN public.organism_file_managed.author IS 'Stores information about the author of the document';
COMMENT ON COLUMN public.organism_habitat_subscription.organism_id IS 'FK to organism.id';
COMMENT ON COLUMN public.organism_habitat_subscription.habitat_id IS 'FK to habitat.id';
COMMENT ON COLUMN public.organism_habitat_subscription.characteristic_organism IS 'True if this organism is a characteristic organism (Leitorganismus) for this habitat.';



