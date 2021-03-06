
/* Drop Indexes */

DROP INDEX IF EXISTS primaryKey;
DROP INDEX IF EXISTS index_pfather_right_oc;
DROP INDEX IF EXISTS index_pfather_left_oc;
DROP INDEX IF EXISTS classification_level;
DROP INDEX IF EXISTS prime_father;
DROP INDEX IF EXISTS name;
DROP INDEX IF EXISTS parent_id_index_oc;
DROP INDEX IF EXISTS index_left_value_oc;
DROP INDEX IF EXISTS index_right_value_oc;
DROP INDEX IF EXISTS index_pfather_left_ocl;
DROP INDEX IF EXISTS index_pfather_right_ocl;
DROP INDEX IF EXISTS id;
DROP INDEX IF EXISTS parent_id_index_ocl;
DROP INDEX IF EXISTS index_left_value_ocl;
DROP INDEX IF EXISTS index_right_value_ocl;
DROP INDEX IF EXISTS index_organism_id_osn;
DROP INDEX IF EXISTS index_osn;
DROP INDEX IF EXISTS FK_organism_11;
DROP INDEX IF EXISTS FK_organism_2;
DROP INDEX IF EXISTS idx_name_de;
DROP INDEX IF EXISTS parent_id_index_o;
DROP INDEX IF EXISTS index_left_value_o;
DROP INDEX IF EXISTS index_right_value_o;
DROP INDEX IF EXISTS organism_habitat_habitat_id_idx;
DROP INDEX IF EXISTS organism_habitat_organism_id_idx;



/* Drop Tables */

DROP TABLE IF EXISTS organism_artgroup_attr_subscription;
DROP TABLE IF EXISTS organism_artgroup_detmethod_subscription;
DROP TABLE IF EXISTS organism_artgroup_subscription;
DROP TABLE IF EXISTS organism_artgroup;
DROP TABLE IF EXISTS organism_artgroup_attr_values;
DROP TABLE IF EXISTS organism_artgroup_attr;
DROP TABLE IF EXISTS organism_artgroup_attr_type;
DROP TABLE IF EXISTS organism_artgroup_detmethod;
DROP TABLE IF EXISTS organism_attribute_value_subscription;
DROP TABLE IF EXISTS organism_attribute_value;
DROP TABLE IF EXISTS organism_classification_lang;
DROP TABLE IF EXISTS organism_classification_subscription;
DROP TABLE IF EXISTS organism_classification;
DROP TABLE IF EXISTS organism_classification_level;
DROP TABLE IF EXISTS organism_lang;
DROP TABLE IF EXISTS organism_synonym;
DROP TABLE IF EXISTS public.organism_file_managed;
DROP TABLE IF EXISTS public.organism_habitat_subscription;
DROP TABLE IF EXISTS public.organism;
DROP TABLE IF EXISTS public.organism_attribute;




/* Create Tables */

CREATE TABLE organism_artgroup
(
	id serial NOT NULL UNIQUE,
	name text NOT NULL,
	parent int DEFAULT 1,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_attr
(
	id serial NOT NULL UNIQUE,
	organism_artgroup_attr_type_id int NOT NULL,
	name text,
	users_uid int DEFAULT 0,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_attr_subscription
(
	id serial NOT NULL UNIQUE,
	organism_artgroup_id int NOT NULL,
	organism_artgroup_attr_id int NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_attr_type
(
	id serial NOT NULL UNIQUE,
	name text,
	format text,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_attr_values
(
	id serial NOT NULL UNIQUE,
	organism_artgroup_attr_id int NOT NULL,
	value text,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_detmethod
(
	id serial NOT NULL UNIQUE,
	name text,
	cscf_id int,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_detmethod_subscription
(
	id serial NOT NULL UNIQUE,
	organism_artgroup_id int NOT NULL,
	organism_artgroup_detmethod_id int NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (organism_artgroup_id, organism_artgroup_detmethod_id)
) WITHOUT OIDS;


CREATE TABLE organism_artgroup_subscription
(
	id serial NOT NULL UNIQUE,
	organism_artgroup_id int NOT NULL,
	-- Die eigene Id, wird fortlaufend inkrementiert.
	organism_id int NOT NULL,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE organism_attribute_value
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	id serial NOT NULL UNIQUE,
	-- Fremdschlüssel auf die Tabelle organism_attribute. 
	organism_attribute_id int NOT NULL,
	-- Falls der valuetype = t, dann kann wird der Text hier gespeichert. 
	text_value text,
	-- Falls der valuetype = b, dann kann wird der Boolean hier gespeichert. 
	boolean_value boolean,
	-- Falls der valuetype = n, dann kann wird die Nummer hier gespeichert. 
	number_value int,
	PRIMARY KEY (id),
	UNIQUE (id, organism_attribute_id)
) WITHOUT OIDS;


CREATE TABLE organism_attribute_value_subscription
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	id serial NOT NULL UNIQUE,
	-- Fremdschlüssel auf die Tabelle organism. 
	organism_id int NOT NULL,
	-- Fremdschlüssel auf die Tabelle organism_attribute_value. 
	organism_attribute_value_id int NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (organism_id, organism_attribute_value_id)
) WITHOUT OIDS;


CREATE TABLE organism_classification
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	id serial NOT NULL UNIQUE,
	-- Eine Referenz auf das Vater-Element. Bei Top-Level-Einträgen ist parent_id = id. 
	parent_id int NOT NULL,
	-- Eine Referenz auf das oberste Elements dieses Baumes. Damit einhält jede Reihe genügend Informationen, um einfach den gesamten Baum abfragen zu können. Bei Top-Level-Einträgen ist prime_father_id = id. 
	prime_father_id int NOT NULL,
	-- Fremdschlüssel auf die Tabelle organism_classification_level.
	organism_classification_level_id int NOT NULL,
	-- Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. 
	left_value int DEFAULT 1 NOT NULL,
	-- Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. 
	right_value int DEFAULT 2 NOT NULL,
	-- Der Name des Klassifizierungslevels in Latein.
	name text NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (parent_id, prime_father_id, name)
) WITHOUT OIDS;


CREATE TABLE organism_classification_lang
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	id serial NOT NULL UNIQUE,
	-- Die eigene Id, wird fortlaufend inkrementiert.
	organism_classification_lang_id int NOT NULL,
	-- Language code, e.g. 'de' or 'en-US'.
	languages_language varchar(12) DEFAULT '''''::character varying' NOT NULL,
	-- Der Name des Klassifizierungslevels in Latein.
	name text NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (organism_classification_lang_id, languages_language)
) WITHOUT OIDS;


CREATE TABLE organism_classification_level
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	id serial NOT NULL UNIQUE,
	-- Eine Referenz auf das Vater-Element. Bei Top-Level-Einträgen ist parent_id = id. 
	parent_id int NOT NULL,
	-- Eine Referenz auf das oberste Elements dieses Baumes. Damit einhält jede Reihe genügend Informationen, um einfach den gesamten Baum abfragen zu können. Bei Top-Level-Einträgen ist prime_father_id = id. 
	prime_father_id int NOT NULL,
	-- Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis.
	left_value int NOT NULL,
	-- Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis.
	right_value int NOT NULL,
	-- Enthält den Klassifizierungsrang als englischer Text (family/order/class/etc.) 
	name text NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (prime_father_id, name)
) WITHOUT OIDS;


CREATE TABLE organism_classification_subscription
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	id serial NOT NULL UNIQUE,
	-- Fremdschlüssel auf die Tabelle organism. 
	organism_id int NOT NULL,
	-- Fremdschlüssel auf die Tabelle organism_classification. 
	organism_classification_id int NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (organism_id)
) WITHOUT OIDS;


CREATE TABLE organism_lang
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	id serial NOT NULL UNIQUE,
	-- Language code, e.g. 'de' or 'en-US'.
	languages_language varchar(12) DEFAULT '''''::character varying' NOT NULL,
	-- Die eigene Id, wird fortlaufend inkrementiert.
	organism_id int NOT NULL,
	-- Enthält den Klassifizierungsrang als englischer Text (family/order/class/etc.) 
	name text NOT NULL,
	PRIMARY KEY (id),
	UNIQUE (languages_language, organism_id, name)
) WITHOUT OIDS;


CREATE TABLE organism_synonym
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	id serial NOT NULL UNIQUE,
	-- Fremdschlüssel auf die Tabelle organism. 
	organism_id int NOT NULL,
	-- Der lateinische Name. 
	name text NOT NULL UNIQUE,
	PRIMARY KEY (id),
	UNIQUE (organism_id, name)
) WITHOUT OIDS;


CREATE TABLE public.organism
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	id serial NOT NULL UNIQUE,
	-- Eine Referenz auf das Vater-Element. Bei Top-Level-Einträgen ist parent_id = id. 
	parent_id int NOT NULL,
	-- Eine Referenz auf das oberste Elements dieses Baumes. Damit einhält jede Reihe genügend Informationen, um einfach den gesamten Baum abfragen zu können. Bei Top-Level-Einträgen ist prime_father_id = id. 
	prime_father_id int NOT NULL,
	-- Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. 
	left_value int DEFAULT 1 NOT NULL,
	-- Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. 
	right_value int DEFAULT 2 NOT NULL,
	scientific_name text NOT NULL UNIQUE,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.organism_attribute
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	id serial NOT NULL UNIQUE,
	-- Gibt den Typ an, welcher in den dazugehörigen organism_attribute_value gespeichert wird. Zuordnung: n=number, b=boolean, t=text.
	valuetype char NOT NULL,
	-- Der Name des Klassifizierungslevels in Latein.
	name text NOT NULL UNIQUE,
	-- An information text about the aim of this attribute.
	comment text,
	PRIMARY KEY (id)
) WITHOUT OIDS;


CREATE TABLE public.organism_file_managed
(
	-- Die eigene Id, wird fortlaufend inkrementiert.
	organism_id int NOT NULL,
	-- file_managed_fid
	file_managed_fid int NOT NULL,
	description text,
	-- Stores information about the author of the document
	author text,
	UNIQUE (organism_id, file_managed_fid)
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

ALTER TABLE organism_artgroup_attr_subscription
	ADD FOREIGN KEY (organism_artgroup_id)
	REFERENCES organism_artgroup (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_artgroup_detmethod_subscription
	ADD FOREIGN KEY (organism_artgroup_id)
	REFERENCES organism_artgroup (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_artgroup_subscription
	ADD FOREIGN KEY (organism_artgroup_id)
	REFERENCES organism_artgroup (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_artgroup_attr_subscription
	ADD FOREIGN KEY (organism_artgroup_attr_id)
	REFERENCES organism_artgroup_attr (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_artgroup_attr_values
	ADD FOREIGN KEY (organism_artgroup_attr_id)
	REFERENCES organism_artgroup_attr (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_artgroup_attr
	ADD FOREIGN KEY (organism_artgroup_attr_type_id)
	REFERENCES organism_artgroup_attr_type (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_artgroup_detmethod_subscription
	ADD FOREIGN KEY (organism_artgroup_detmethod_id)
	REFERENCES organism_artgroup_detmethod (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


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


ALTER TABLE organism_classification
	ADD FOREIGN KEY (prime_father_id)
	REFERENCES organism_classification (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification_lang
	ADD FOREIGN KEY (organism_classification_lang_id)
	REFERENCES organism_classification (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE organism_classification_subscription
	ADD FOREIGN KEY (organism_classification_id)
	REFERENCES organism_classification (id)
	ON UPDATE CASCADE
	ON DELETE CASCADE
;


ALTER TABLE organism_classification
	ADD FOREIGN KEY (organism_classification_level_id)
	REFERENCES organism_classification_level (id)
	ON UPDATE CASCADE
	ON DELETE CASCADE
;


ALTER TABLE organism_classification_level
	ADD FOREIGN KEY (prime_father_id)
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


ALTER TABLE organism_artgroup_subscription
	ADD FOREIGN KEY (organism_id)
	REFERENCES public.organism (id)
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


ALTER TABLE organism_synonym
	ADD FOREIGN KEY (organism_id)
	REFERENCES public.organism (id)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;


ALTER TABLE public.organism
	ADD FOREIGN KEY (parent_id)
	REFERENCES public.organism (id)
	ON UPDATE CASCADE
	ON DELETE RESTRICT
;


ALTER TABLE public.organism
	ADD FOREIGN KEY (prime_father_id)
	REFERENCES public.organism (id)
	ON UPDATE CASCADE
	ON DELETE RESTRICT
;


ALTER TABLE public.organism_file_managed
	ADD CONSTRAINT organism2file FOREIGN KEY (organism_id)
	REFERENCES public.organism (id)
	ON UPDATE CASCADE
	ON DELETE CASCADE
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
CREATE INDEX index_pfather_right_oc ON organism_classification (prime_father_id, right_value);
CREATE INDEX index_pfather_left_oc ON organism_classification (prime_father_id, left_value);
CREATE INDEX classification_level ON organism_classification (organism_classification_level_id);
CREATE INDEX prime_father ON organism_classification (prime_father_id);
CREATE INDEX name ON organism_classification (name);
CREATE INDEX parent_id_index_oc ON organism_classification (parent_id);
CREATE INDEX index_left_value_oc ON organism_classification (left_value);
CREATE INDEX index_right_value_oc ON organism_classification (right_value);
CREATE INDEX index_pfather_left_ocl ON organism_classification_level (prime_father_id, left_value);
CREATE INDEX index_pfather_right_ocl ON organism_classification_level (prime_father_id, right_value);
CREATE INDEX id ON organism_classification_level (id);
CREATE INDEX parent_id_index_ocl ON organism_classification_level (parent_id);
CREATE INDEX index_left_value_ocl ON organism_classification_level (left_value);
CREATE INDEX index_right_value_ocl ON organism_classification_level (right_value);
CREATE INDEX index_organism_id_osn ON organism_synonym (organism_id);
CREATE INDEX index_osn ON organism_synonym (name);
CREATE INDEX FK_organism_11 ON public.organism USING BTREE (id);
CREATE INDEX FK_organism_2 ON public.organism USING BTREE (left_value, prime_father_id);
CREATE INDEX idx_name_de ON public.organism USING BTREE (right_value, prime_father_id);
CREATE INDEX parent_id_index_o ON public.organism (parent_id);
CREATE INDEX index_left_value_o ON public.organism (left_value);
CREATE INDEX index_right_value_o ON public.organism (right_value);
CREATE INDEX organism_habitat_habitat_id_idx ON public.organism_habitat_subscription (habitat_id);
CREATE INDEX organism_habitat_organism_id_idx ON public.organism_habitat_subscription (organism_id);



/* Comments */

COMMENT ON COLUMN organism_artgroup_subscription.organism_id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_attribute_value.id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_attribute_value.organism_attribute_id IS 'Fremdschlüssel auf die Tabelle organism_attribute. ';
COMMENT ON COLUMN organism_attribute_value.text_value IS 'Falls der valuetype = t, dann kann wird der Text hier gespeichert. ';
COMMENT ON COLUMN organism_attribute_value.boolean_value IS 'Falls der valuetype = b, dann kann wird der Boolean hier gespeichert. ';
COMMENT ON COLUMN organism_attribute_value.number_value IS 'Falls der valuetype = n, dann kann wird die Nummer hier gespeichert. ';
COMMENT ON COLUMN organism_attribute_value_subscription.id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_attribute_value_subscription.organism_id IS 'Fremdschlüssel auf die Tabelle organism. ';
COMMENT ON COLUMN organism_attribute_value_subscription.organism_attribute_value_id IS 'Fremdschlüssel auf die Tabelle organism_attribute_value. ';
COMMENT ON COLUMN organism_classification.id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_classification.parent_id IS 'Eine Referenz auf das Vater-Element. Bei Top-Level-Einträgen ist parent_id = id. ';
COMMENT ON COLUMN organism_classification.prime_father_id IS 'Eine Referenz auf das oberste Elements dieses Baumes. Damit einhält jede Reihe genügend Informationen, um einfach den gesamten Baum abfragen zu können. Bei Top-Level-Einträgen ist prime_father_id = id. ';
COMMENT ON COLUMN organism_classification.organism_classification_level_id IS 'Fremdschlüssel auf die Tabelle organism_classification_level.';
COMMENT ON COLUMN organism_classification.left_value IS 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. ';
COMMENT ON COLUMN organism_classification.right_value IS 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. ';
COMMENT ON COLUMN organism_classification.name IS 'Der Name des Klassifizierungslevels in Latein.';
COMMENT ON COLUMN organism_classification_lang.id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_classification_lang.organism_classification_lang_id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_classification_lang.languages_language IS 'Language code, e.g. ''de'' or ''en-US''.';
COMMENT ON COLUMN organism_classification_lang.name IS 'Der Name des Klassifizierungslevels in Latein.';
COMMENT ON COLUMN organism_classification_level.id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_classification_level.parent_id IS 'Eine Referenz auf das Vater-Element. Bei Top-Level-Einträgen ist parent_id = id. ';
COMMENT ON COLUMN organism_classification_level.prime_father_id IS 'Eine Referenz auf das oberste Elements dieses Baumes. Damit einhält jede Reihe genügend Informationen, um einfach den gesamten Baum abfragen zu können. Bei Top-Level-Einträgen ist prime_father_id = id. ';
COMMENT ON COLUMN organism_classification_level.left_value IS 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis.';
COMMENT ON COLUMN organism_classification_level.right_value IS 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis.';
COMMENT ON COLUMN organism_classification_level.name IS 'Enthält den Klassifizierungsrang als englischer Text (family/order/class/etc.) ';
COMMENT ON COLUMN organism_classification_subscription.id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_classification_subscription.organism_id IS 'Fremdschlüssel auf die Tabelle organism. ';
COMMENT ON COLUMN organism_classification_subscription.organism_classification_id IS 'Fremdschlüssel auf die Tabelle organism_classification. ';
COMMENT ON COLUMN organism_lang.id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_lang.languages_language IS 'Language code, e.g. ''de'' or ''en-US''.';
COMMENT ON COLUMN organism_lang.organism_id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_lang.name IS 'Enthält den Klassifizierungsrang als englischer Text (family/order/class/etc.) ';
COMMENT ON COLUMN organism_synonym.id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN organism_synonym.organism_id IS 'Fremdschlüssel auf die Tabelle organism. ';
COMMENT ON COLUMN organism_synonym.name IS 'Der lateinische Name. ';
COMMENT ON COLUMN public.organism.id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN public.organism.parent_id IS 'Eine Referenz auf das Vater-Element. Bei Top-Level-Einträgen ist parent_id = id. ';
COMMENT ON COLUMN public.organism.prime_father_id IS 'Eine Referenz auf das oberste Elements dieses Baumes. Damit einhält jede Reihe genügend Informationen, um einfach den gesamten Baum abfragen zu können. Bei Top-Level-Einträgen ist prime_father_id = id. ';
COMMENT ON COLUMN public.organism.left_value IS 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. ';
COMMENT ON COLUMN public.organism.right_value IS 'Nötig zur Strukturierung, siehe «Baumstrukturen» im Organism-Artikel des Wikis. ';
COMMENT ON COLUMN public.organism_attribute.id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN public.organism_attribute.valuetype IS 'Gibt den Typ an, welcher in den dazugehörigen organism_attribute_value gespeichert wird. Zuordnung: n=number, b=boolean, t=text.';
COMMENT ON COLUMN public.organism_attribute.name IS 'Der Name des Klassifizierungslevels in Latein.';
COMMENT ON COLUMN public.organism_attribute.comment IS 'An information text about the aim of this attribute.';
COMMENT ON COLUMN public.organism_file_managed.organism_id IS 'Die eigene Id, wird fortlaufend inkrementiert.';
COMMENT ON COLUMN public.organism_file_managed.file_managed_fid IS 'file_managed_fid';
COMMENT ON COLUMN public.organism_file_managed.author IS 'Stores information about the author of the document';
COMMENT ON COLUMN public.organism_habitat_subscription.organism_id IS 'FK to organism.id';
COMMENT ON COLUMN public.organism_habitat_subscription.habitat_id IS 'FK to habitat.id';
COMMENT ON COLUMN public.organism_habitat_subscription.characteristic_organism IS 'True if this organism is a characteristic organism (Leitorganismus) for this habitat.';



