
/* Drop Indexes */

DROP INDEX IF EXISTS primaryKey;
DROP INDEX IF EXISTS left_value;
DROP INDEX IF EXISTS left_value;
DROP INDEX IF EXISTS levelindex;
DROP INDEX IF EXISTS classifierid;
DROP INDEX IF EXISTS nameindex;
DROP INDEX IF EXISTS parentid;
DROP INDEX IF EXISTS anotherindex;
DROP INDEX IF EXISTS index;
DROP INDEX IF EXISTS parent_id;
DROP INDEX IF EXISTS left_value;
DROP INDEX IF EXISTS right_value;
DROP INDEX IF EXISTS name;
DROP INDEX IF EXISTS classifier_id;
DROP INDEX IF EXISTS organism_id;
DROP INDEX IF EXISTS value;
DROP INDEX IF EXISTS FK_organism_11;
DROP INDEX IF EXISTS FK_organism_2;
DROP INDEX IF EXISTS idx_name_de;
DROP INDEX IF EXISTS parent_id_index;
DROP INDEX IF EXISTS fki_organism_file_managed_ibfk_1;
DROP INDEX IF EXISTS fki_organism_file_managed_ibfk_2;
DROP INDEX IF EXISTS organism_habitat_habitat_id_idx;
DROP INDEX IF EXISTS organism_habitat_organism_id_idx;



/* Drop Tables */

DROP TABLE IF EXISTS organism_attribute_value_subscription;
DROP TABLE IF EXISTS organism_attribute_value;
DROP TABLE IF EXISTS organism_classification_subscription;
DROP TABLE IF EXISTS organism_classification;
DROP TABLE IF EXISTS organism_classification_level;
DROP TABLE IF EXISTS organism_classifier;
DROP TABLE IF EXISTS organism_lang;
DROP TABLE IF EXISTS organism_scientific_name;
DROP TABLE IF EXISTS public.organism_file_managed;
DROP TABLE IF EXISTS public.organism_habitat_subscription;
DROP TABLE IF EXISTS public.organism;
DROP TABLE IF EXISTS public.organism_attribute;




/* Create Tables */

CREATE TABLE organism_attribute_value
(
	-- Used just for importing
	id serial NOT NULL UNIQUE,
	organism_attribute_id int NOT NULL,
	text_value text,
	boolean_value boolean,
	number_value int,
	PRIMARY KEY (id),
	UNIQUE (id, organism_attribute_id)
) WITHOUT OIDS;


CREATE TABLE organism_attribute_value_subscription
(
	id serial NOT NULL UNIQUE,
	organism_id int NOT NULL,
	-- Used just for importing
	organism_attribute_value_id int NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (organism_id, organism_attribute_value_id)
) WITHOUT OIDS;


CREATE TABLE organism_classification
(
	id serial NOT NULL UNIQUE,
	-- If this classifier is on top level, then parent_id = 0
	parent_id int NOT NULL,
	organism_classifier_id int NOT NULL,
	organism_classification_level_id int NOT NULL,
	left_value int DEFAULT 1 NOT NULL,
	right_value int DEFAULT 2 NOT NULL,
	-- Name of this classification level - e.g. domain, kingdom, phylum, class, order, familiy, genus
	name text NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (organism_classifier_id, organism_classification_level_id, name)
) WITHOUT OIDS;


CREATE TABLE organism_classification_level
(
	id serial NOT NULL UNIQUE,
	parent_id int NOT NULL,
	organism_classifier_id int NOT NULL,
	left_value int NOT NULL,
	right_value int NOT NULL,
	name text NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (organism_classifier_id, left_value),
	UNIQUE (organism_classifier_id, right_value),
	UNIQUE (organism_classifier_id, name)
) WITHOUT OIDS;


CREATE TABLE organism_classification_subscription
(
	id serial NOT NULL UNIQUE,
	organism_id int NOT NULL,
	organism_classification_id int NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (organism_id)
) WITHOUT OIDS;


CREATE TABLE organism_classifier
(
	id serial NOT NULL UNIQUE,
	-- Name of this attribute. Gets translated by Drupal.
	name text NOT NULL UNIQUE,
	scientific_classification boolean NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_lang
(
	id serial NOT NULL UNIQUE,
	-- Language code, e.g. 'de' or 'en-US'.
	languages_language varchar(12) DEFAULT '''''::character varying' NOT NULL,
	organism_id int NOT NULL,
	name text NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (languages_language, organism_id, name)
) WITHOUT OIDS;


CREATE TABLE organism_scientific_name
(
	-- Used just for importing
	id serial NOT NULL UNIQUE,
	organism_id int NOT NULL,
	-- scientific, latin name
	name text NOT NULL UNIQUE,
	PRIMARY KEY (id),
	UNIQUE (organism_id, name)
) WITHOUT OIDS;


CREATE TABLE public.organism
(
	id serial NOT NULL UNIQUE,
	-- If this organism is not part of an aggregated organism, then parent_id = 0
	parent_id int NOT NULL,
	-- Points to the root node of this element
	prime_father_id int NOT NULL,
	-- Used to build a hierarchically class
	left_value int DEFAULT 1 NOT NULL,
	-- Used to build a hierarchically class
	right_value int DEFAULT 2 NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.organism_attribute
(
	id serial NOT NULL UNIQUE,
	-- Specify, which type this value this attribute has and thus which column is set in organism_attribute_value. t = text, b = boolean, n = number
	valuetype char NOT NULL,
	-- Name of this attribute. Gets translated by Drupal.
	name text NOT NULL UNIQUE,
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

ALTER TABLE organism_attribute_value_subscription
	ADD FOREIGN KEY (organism_attribute_value_id)
	REFERENCES organism_attribute_value (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification
	ADD FOREIGN KEY (parent_id)
	REFERENCES organism_classification (id)
	ON UPDATE CASCADE
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification_subscription
	ADD FOREIGN KEY (organism_classification_id)
	REFERENCES organism_classification (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification
	ADD FOREIGN KEY (organism_classification_level_id)
	REFERENCES organism_classification_level (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification_level
	ADD FOREIGN KEY (parent_id)
	REFERENCES organism_classification_level (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification
	ADD FOREIGN KEY (organism_classifier_id)
	REFERENCES organism_classifier (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification_level
	ADD FOREIGN KEY (organism_classifier_id)
	REFERENCES organism_classifier (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_attribute_value_subscription
	ADD FOREIGN KEY (organism_id)
	REFERENCES public.organism (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification_subscription
	ADD FOREIGN KEY (organism_id)
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
	ADD FOREIGN KEY (prime_father_id)
	REFERENCES public.organism (id)
	ON UPDATE CASCADE
	ON DELETE RESTRICT
;


ALTER TABLE public.organism
	ADD FOREIGN KEY (parent_id)
	REFERENCES public.organism (id)
	ON UPDATE CASCADE
	ON DELETE RESTRICT
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


ALTER TABLE organism_attribute_value
	ADD FOREIGN KEY (organism_attribute_id)
	REFERENCES public.organism_attribute (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;



/* Create Indexes */

CREATE INDEX primaryKey ON organism_classification USING BTREE (id);
CREATE INDEX left_value ON organism_classification (left_value, organism_classifier_id);
CREATE INDEX left_value ON organism_classification (right_value, organism_classifier_id);
CREATE INDEX levelindex ON organism_classification (organism_classification_level_id);
CREATE INDEX classifierid ON organism_classification (organism_classifier_id);
CREATE INDEX nameindex ON organism_classification (name);
CREATE INDEX parentid ON organism_classification (parent_id);
CREATE INDEX anotherindex ON organism_classification (name, organism_classification_level_id);
CREATE INDEX index ON organism_classification_level (id);
CREATE INDEX parent_id ON organism_classification_level (parent_id);
CREATE INDEX left_value ON organism_classification_level (left_value);
CREATE INDEX right_value ON organism_classification_level (right_value);
CREATE INDEX name ON organism_classification_level (name);
CREATE INDEX classifier_id ON organism_classification_level (organism_classifier_id);
CREATE INDEX organism_id ON organism_scientific_name (organism_id);
CREATE INDEX value ON organism_scientific_name (name);
CREATE INDEX FK_organism_11 ON public.organism USING BTREE (id);
CREATE INDEX FK_organism_2 ON public.organism USING BTREE (left_value);
CREATE INDEX idx_name_de ON public.organism USING BTREE (right_value);
CREATE INDEX parent_id_index ON public.organism (parent_id);
CREATE INDEX fki_organism_file_managed_ibfk_1 ON public.organism_file_managed (organism_id);
CREATE INDEX fki_organism_file_managed_ibfk_2 ON public.organism_file_managed (file_managed_id);
CREATE INDEX organism_habitat_habitat_id_idx ON public.organism_habitat_subscription (habitat_id);
CREATE INDEX organism_habitat_organism_id_idx ON public.organism_habitat_subscription (organism_id);



/* Comments */

COMMENT ON COLUMN organism_attribute_value.id IS 'Used just for importing';
COMMENT ON COLUMN organism_attribute_value_subscription.organism_attribute_value_id IS 'Used just for importing';
COMMENT ON COLUMN organism_classification.parent_id IS 'If this classifier is on top level, then parent_id = 0';
COMMENT ON COLUMN organism_classification.name IS 'Name of this classification level - e.g. domain, kingdom, phylum, class, order, familiy, genus';
COMMENT ON COLUMN organism_classifier.name IS 'Name of this attribute. Gets translated by Drupal.';
COMMENT ON COLUMN organism_lang.languages_language IS 'Language code, e.g. ''de'' or ''en-US''.';
COMMENT ON COLUMN organism_scientific_name.id IS 'Used just for importing';
COMMENT ON COLUMN organism_scientific_name.name IS 'scientific, latin name';
COMMENT ON COLUMN public.organism.parent_id IS 'If this organism is not part of an aggregated organism, then parent_id = 0';
COMMENT ON COLUMN public.organism.prime_father_id IS 'Points to the root node of this element';
COMMENT ON COLUMN public.organism.left_value IS 'Used to build a hierarchically class';
COMMENT ON COLUMN public.organism.right_value IS 'Used to build a hierarchically class';
COMMENT ON COLUMN public.organism_attribute.valuetype IS 'Specify, which type this value this attribute has and thus which column is set in organism_attribute_value. t = text, b = boolean, n = number';
COMMENT ON COLUMN public.organism_attribute.name IS 'Name of this attribute. Gets translated by Drupal.';
COMMENT ON COLUMN public.organism_file_managed.file_managed_id IS 'file_managed_id';
COMMENT ON COLUMN public.organism_file_managed.author IS 'Stores information about the author of the document';
COMMENT ON COLUMN public.organism_habitat_subscription.organism_id IS 'FK to organism.id';
COMMENT ON COLUMN public.organism_habitat_subscription.habitat_id IS 'FK to habitat.id';
COMMENT ON COLUMN public.organism_habitat_subscription.characteristic_organism IS 'True if this organism is a characteristic organism (Leitorganismus) for this habitat.';



