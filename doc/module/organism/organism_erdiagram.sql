
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



/* Drop Sequences */

DROP SEQUENCE IF EXISTS public.area_habitat_id_seq;
DROP SEQUENCE IF EXISTS public.area_id_seq;
DROP SEQUENCE IF EXISTS public.area_parcel_id_seq;
DROP SEQUENCE IF EXISTS public.area_parcel_id_seq1;
DROP SEQUENCE IF EXISTS public.area_type_id_seq;
DROP SEQUENCE IF EXISTS public.attribute_dropdown_value_attribute_dropdown_value_id_seq;
DROP SEQUENCE IF EXISTS public.attribute_dropdown_value_organism_type_attribute_id_seq;
DROP SEQUENCE IF EXISTS public.attribute_format_id_seq;
DROP SEQUENCE IF EXISTS public.authmap_aid_seq;
DROP SEQUENCE IF EXISTS public.blocked_ips_iid_seq;
DROP SEQUENCE IF EXISTS public.block_bid_seq;
DROP SEQUENCE IF EXISTS public.block_custom_bid_seq;
DROP SEQUENCE IF EXISTS public.classifiaction_classification_id_seq;
DROP SEQUENCE IF EXISTS public.classifiaction_taxon_classification_taxon_id_seq;
DROP SEQUENCE IF EXISTS public.classifiaction_taxon_taxon_id_seq;
DROP SEQUENCE IF EXISTS public.commerce_customer_profile_profile_id_seq;
DROP SEQUENCE IF EXISTS public.commerce_customer_profile_revision_revision_id_seq;
DROP SEQUENCE IF EXISTS public.commerce_line_item_line_item_id_seq;
DROP SEQUENCE IF EXISTS public.commerce_order_order_id_seq;
DROP SEQUENCE IF EXISTS public.commerce_order_revision_revision_id_seq;
DROP SEQUENCE IF EXISTS public.commerce_payment_transaction_revision_revision_id_seq;
DROP SEQUENCE IF EXISTS public.commerce_payment_transaction_transaction_id_seq;
DROP SEQUENCE IF EXISTS public.commerce_paypal_ipn_ipn_id_seq;
DROP SEQUENCE IF EXISTS public.commerce_product_product_id_seq;
DROP SEQUENCE IF EXISTS public.contact_cid_seq;
DROP SEQUENCE IF EXISTS public.date_formats_dfid_seq;
DROP SEQUENCE IF EXISTS public.export_type_id_seq;
DROP SEQUENCE IF EXISTS public.fauna_class_id_seq;
DROP SEQUENCE IF EXISTS public.fauna_organism_id_seq;
DROP SEQUENCE IF EXISTS public.field_config_id_seq;
DROP SEQUENCE IF EXISTS public.field_config_instance_id_seq;
DROP SEQUENCE IF EXISTS public.file_managed_fid_seq;
DROP SEQUENCE IF EXISTS public.flood_fid_seq;
DROP SEQUENCE IF EXISTS public.flora_organism_id_seq;
DROP SEQUENCE IF EXISTS public.flora_red_list_id_seq;
DROP SEQUENCE IF EXISTS public.gallery_category_condition_id_seq;
DROP SEQUENCE IF EXISTS public.gallery_category_id_seq;
DROP SEQUENCE IF EXISTS public.gallery_category_option_id_seq;
DROP SEQUENCE IF EXISTS public.gallery_image_category_id_seq;
DROP SEQUENCE IF EXISTS public.gallery_image_id_seq;
DROP SEQUENCE IF EXISTS public.gallery_image_rating_id_seq;
DROP SEQUENCE IF EXISTS public.gallery_image_subtype_id_seq;
DROP SEQUENCE IF EXISTS public.gallery_rating_type_id_seq;
DROP SEQUENCE IF EXISTS public.habitat_id_seq;
DROP SEQUENCE IF EXISTS public.head_inventory_export_type_id_seq;
DROP SEQUENCE IF EXISTS public.head_inventory_id_seq;
DROP SEQUENCE IF EXISTS public.image_effects_ieid_seq;
DROP SEQUENCE IF EXISTS public.image_styles_isid_seq;
DROP SEQUENCE IF EXISTS public.inventory_custom_protection_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_custom_protection_inventory_entry_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_custom_protection_inventory_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_custom_protection_protection_level_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_entry_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_template_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_template_inventory_entry_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_template_inventory_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_type_attribute_dropdown_value_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_type_attribute_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_type_attribute_inventory_entry_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_type_id_seq;
DROP SEQUENCE IF EXISTS public.inventory_type_organism_id_seq;
DROP SEQUENCE IF EXISTS public.locales_source_lid_seq;
DROP SEQUENCE IF EXISTS public.menu_links_mlid_seq;
DROP SEQUENCE IF EXISTS public.node_nid_seq;
DROP SEQUENCE IF EXISTS public.node_revision_vid_seq;
DROP SEQUENCE IF EXISTS public.open_identification_open_identification_id_seq;
DROP SEQUENCE IF EXISTS public.open_identification_solved_open_identification_solved_id_seq;
DROP SEQUENCE IF EXISTS public.open_identification_tip_open_identification_tip_id_seq;
DROP SEQUENCE IF EXISTS public.organism_attribute_attribute_dropdown_value_id_seq;
DROP SEQUENCE IF EXISTS public.organism_attribute_organism_id_seq;
DROP SEQUENCE IF EXISTS public.organism_attribute_organism_type_attribute_id_seq;
DROP SEQUENCE IF EXISTS public.organism_habitat_id_seq;
DROP SEQUENCE IF EXISTS public.organism_id_seq;
DROP SEQUENCE IF EXISTS public.organism_type_attribute_classification_id_seq;
DROP SEQUENCE IF EXISTS public.organism_type_attribute_organism_type_attribute_id_seq;
DROP SEQUENCE IF EXISTS public.queue_item_id_seq;
DROP SEQUENCE IF EXISTS public.role_rid_seq;
DROP SEQUENCE IF EXISTS public.rules_config_id_seq;
DROP SEQUENCE IF EXISTS public.rules_scheduler_tid_seq;
DROP SEQUENCE IF EXISTS public.sequences_value_seq;
DROP SEQUENCE IF EXISTS public.services_endpoint_eid_seq;
DROP SEQUENCE IF EXISTS public.sgroup_sgid_seq;
DROP SEQUENCE IF EXISTS public.synonym_oranism_id_seq;
DROP SEQUENCE IF EXISTS public.synonym_taxon_id_seq;
DROP SEQUENCE IF EXISTS public.url_alias_pid_seq;
DROP SEQUENCE IF EXISTS public.views_view_vid_seq;
DROP SEQUENCE IF EXISTS public.watchdog_wid_seq;




/* Create Sequences */

CREATE SEQUENCE public.area_habitat_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 333 CACHE 1;
CREATE SEQUENCE public.area_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 354 CACHE 1;
CREATE SEQUENCE public.area_parcel_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.area_parcel_id_seq1 INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 107 CACHE 1;
CREATE SEQUENCE public.area_type_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 2 CACHE 1;
CREATE SEQUENCE public.attribute_dropdown_value_attribute_dropdown_value_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 8 CACHE 1;
CREATE SEQUENCE public.attribute_dropdown_value_organism_type_attribute_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.attribute_format_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 4 CACHE 1;
CREATE SEQUENCE public.authmap_aid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.blocked_ips_iid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.block_bid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 110 CACHE 1;
CREATE SEQUENCE public.block_custom_bid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.classifiaction_classification_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 100 CACHE 1;
CREATE SEQUENCE public.classifiaction_taxon_classification_taxon_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 52157 CACHE 1;
CREATE SEQUENCE public.classifiaction_taxon_taxon_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.commerce_customer_profile_profile_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 5 CACHE 1;
CREATE SEQUENCE public.commerce_customer_profile_revision_revision_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 5 CACHE 1;
CREATE SEQUENCE public.commerce_line_item_line_item_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 20 CACHE 1;
CREATE SEQUENCE public.commerce_order_order_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 17 CACHE 1;
CREATE SEQUENCE public.commerce_order_revision_revision_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 55 CACHE 1;
CREATE SEQUENCE public.commerce_payment_transaction_revision_revision_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.commerce_payment_transaction_transaction_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.commerce_paypal_ipn_ipn_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.commerce_product_product_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.contact_cid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.date_formats_dfid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 35 CACHE 1;
CREATE SEQUENCE public.export_type_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.fauna_class_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 13 CACHE 1;
CREATE SEQUENCE public.fauna_organism_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.field_config_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 12 CACHE 1;
CREATE SEQUENCE public.field_config_instance_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 16 CACHE 1;
CREATE SEQUENCE public.flood_fid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 752 CACHE 1;
CREATE SEQUENCE public.flora_organism_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 5251 CACHE 1;
CREATE SEQUENCE public.flora_red_list_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 4922 CACHE 1;
CREATE SEQUENCE public.gallery_category_condition_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 3 CACHE 1;
CREATE SEQUENCE public.gallery_category_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 2 CACHE 1;
CREATE SEQUENCE public.gallery_category_option_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 9 CACHE 1;
CREATE SEQUENCE public.gallery_image_category_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 36 CACHE 1;
CREATE SEQUENCE public.gallery_image_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 2944 CACHE 1;
CREATE SEQUENCE public.gallery_image_rating_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 28 CACHE 1;
CREATE SEQUENCE public.gallery_image_subtype_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 5 CACHE 1;
CREATE SEQUENCE public.gallery_rating_type_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 2 CACHE 1;
CREATE SEQUENCE public.head_inventory_export_type_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.head_inventory_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 350 CACHE 1;
CREATE SEQUENCE public.image_effects_ieid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.image_styles_isid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.inventory_custom_protection_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 3 CACHE 1;
CREATE SEQUENCE public.inventory_custom_protection_inventory_entry_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.inventory_custom_protection_inventory_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.inventory_custom_protection_protection_level_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 3 CACHE 1;
CREATE SEQUENCE public.inventory_entry_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1241 CACHE 1;
CREATE SEQUENCE public.inventory_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 314 CACHE 1;
CREATE SEQUENCE public.inventory_template_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 8 CACHE 1;
CREATE SEQUENCE public.inventory_template_inventory_entry_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 119 CACHE 1;
CREATE SEQUENCE public.inventory_template_inventory_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 8 CACHE 1;
CREATE SEQUENCE public.inventory_type_attribute_dropdown_value_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 100 CACHE 1;
CREATE SEQUENCE public.inventory_type_attribute_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 98 CACHE 1;
CREATE SEQUENCE public.inventory_type_attribute_inventory_entry_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 2567 CACHE 1;
CREATE SEQUENCE public.inventory_type_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 63 CACHE 1;
CREATE SEQUENCE public.inventory_type_organism_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 26752 CACHE 1;
CREATE SEQUENCE public.locales_source_lid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 10442 CACHE 1;
CREATE SEQUENCE public.menu_links_mlid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 780 CACHE 1;
CREATE SEQUENCE public.node_nid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 24 CACHE 1;
CREATE SEQUENCE public.node_revision_vid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 24 CACHE 1;
CREATE SEQUENCE public.open_identification_open_identification_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 3 CACHE 1;
CREATE SEQUENCE public.open_identification_solved_open_identification_solved_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.open_identification_tip_open_identification_tip_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 11 CACHE 1;
CREATE SEQUENCE public.organism_attribute_attribute_dropdown_value_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.organism_attribute_organism_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.organism_attribute_organism_type_attribute_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.organism_habitat_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 30318 CACHE 1;
CREATE SEQUENCE public.organism_type_attribute_classification_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.organism_type_attribute_organism_type_attribute_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 2 CACHE 1;
CREATE SEQUENCE public.queue_item_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 26 CACHE 1;
CREATE SEQUENCE public.role_rid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 4 CACHE 1;
CREATE SEQUENCE public.rules_config_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 9 CACHE 1;
CREATE SEQUENCE public.rules_scheduler_tid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.sequences_value_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 49 CACHE 1;
CREATE SEQUENCE public.services_endpoint_eid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.sgroup_sgid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 12 CACHE 1;
CREATE SEQUENCE public.synonym_oranism_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.synonym_taxon_id_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.url_alias_pid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.views_view_vid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 1 CACHE 1;
CREATE SEQUENCE public.watchdog_wid_seq INCREMENT 1 MINVALUE 1 MAXVALUE 9223372036854775807 START 780536 CACHE 1;



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
	text text,
	boolean boolean,
	number int
) WITHOUT OIDS;


CREATE TABLE organism_classification
(
	id serial NOT NULL UNIQUE,
	-- If this classifier is on top level, then parent_id == 0
	parent_id int NOT NULL,
	organism_classifier_id int NOT NULL,
	left_value int NOT NULL,
	right_value int NOT NULL,
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
	parent_id int NOT NULL,
	-- Used to build a hierarchically class
	left_value int NOT NULL,
	-- Used to build a hierarchically class
	right_value int NOT NULL,
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
