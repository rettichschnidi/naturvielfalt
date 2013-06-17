--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

--
-- Name: {organism_artgroup_attr_id_seq}; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_artgroup_attr_id_seq}', 1, false);


--
-- Data for Name: {organism_artgroup_attr}; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (1, 1, 'Genauigkeit', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (2, 2, 'Kommentar', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (3, 3, 'Fundort', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (4, 4, 'Haufigkeit', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (5, 5, 'Phanologie', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (6, 6, 'Bestimmung', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (7, 7, 'Herkunft', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (8, 8, 'Vorkommen', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (9, 9, 'Beleg vorhanden', 0);
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, name, users_uid) VALUES (10, 10, 'Verifiziert durch', 0);


--
-- PostgreSQL database dump complete
--

