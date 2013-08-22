--
-- PostgreSQL database dump
--

-- Started on 2013-08-22 11:55:55

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

--
-- TOC entry 3880 (class 0 OID 0)
-- Dependencies: 535
-- Name: organism_artgroup_attr_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_artgroup_attr_id_seq}', 1, false);


--
-- TOC entry 3877 (class 0 OID 204673)
-- Dependencies: 536
-- Data for Name: organism_artgroup_attr; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (2, 2, 'Comment', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (1, 3, 'Accuracy', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (4, 3, 'Commonness', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (5, 3, 'Phenology', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (6, 3, 'Determination', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (7, 3, 'Origin', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (8, 3, 'Occurence', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (9, 3, 'Evidence available', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (10, 1, 'Verified', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (11, 3, 'Evaluationmethod', 0);


-- Completed on 2013-08-22 11:55:55

--
-- PostgreSQL database dump complete
--

