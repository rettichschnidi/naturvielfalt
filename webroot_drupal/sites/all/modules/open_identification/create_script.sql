CREATE TABLE open_identification
(
  open_identification_id bigserial NOT NULL,
  user_id integer NOT NULL,
  open_identification_create_date timestamp with time zone,
  open_identification_modified_date timestamp with time zone,
  open_identification_comment text,
  open_identification_area integer NOT NULL,
  open_identification_type character(255),
  open_identification_organismgroupid bigint,
  CONSTRAINT open_identification_pkey PRIMARY KEY (open_identification_id)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE open_identification OWNER TO postgres;


CREATE TABLE open_identification_gallery
(
  open_identification_id integer NOT NULL,
  gallery_image_id integer NOT NULL,
  CONSTRAINT open_identification_gallery_pkey PRIMARY KEY (open_identification_id, gallery_image_id),
  CONSTRAINT open_identification_gallery_gallery_image_id_fkey FOREIGN KEY (gallery_image_id)
      REFERENCES gallery_image (id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION
)
WITH (
  OIDS=FALSE
);
ALTER TABLE open_identification_gallery OWNER TO postgres;
CREATE TABLE open_identification_solved
(
  open_identification_solved_id bigserial NOT NULL,
  open_identification_solved_organismgroupid bigint,
  open_identification_id bigint,
  open_identification_solved_item_id bigint,
  open_identification_solved_date timestamp with time zone,
  CONSTRAINT open_identification_solved_pkey PRIMARY KEY (open_identification_solved_id),
  CONSTRAINT open_identification_solved_open_identification_id_fkey FOREIGN KEY (open_identification_id)
      REFERENCES open_identification (open_identification_id) MATCH SIMPLE
      ON UPDATE NO ACTION ON DELETE NO ACTION
)
WITH (
  OIDS=FALSE
);
ALTER TABLE open_identification_solved OWNER TO postgres;
CREATE TABLE open_identification_tip
(
  user_id integer NOT NULL,
  open_identification_id integer NOT NULL,
  open_identification_tip_id bigserial NOT NULL,
  open_identification_tip_item_id integer NOT NULL,
  open_identification_type character varying(255),
  open_identification_tip_organismgroupid bigint,
  open_identification_tip_date timestamp with time zone,
  CONSTRAINT pk PRIMARY KEY (open_identification_tip_id)
)
WITH (
  OIDS=FALSE
);
ALTER TABLE open_identification_tip OWNER TO postgres;
