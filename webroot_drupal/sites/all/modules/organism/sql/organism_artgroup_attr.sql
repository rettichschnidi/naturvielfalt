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

SELECT pg_catalog.setval('{organism_artgroup_attr_id_seq}', 21, true);


--
-- Data for Name: {organism_artgroup_attr}; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (1, 1, 0, 'Identifiziert von');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (2, 2, 0, 'Phase');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (3, 3, 0, 'Funddatum');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (4, 2, 0, 'Status');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (5, 4, 0, 'Anzahl');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (6, 1, 0, 'Beobachter');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (7, 1, 0, 'Wassertiefe');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (8, 1, 0, 'Blütenbesuch');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (9, 1, 0, 'Eiablage');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (10, 2, 0, 'Verhalten');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (11, 1, 0, 'Temperatur');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (12, 1, 0, 'Gesang');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (13, 1, 0, 'Grösse');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (14, 1, 0, 'Luftteals');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (15, 2, 0, 'Flächengrösse');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (16, 4, 0, 'Wuchshöhe');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (17, 3, 0, 'Uhrzeit');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (18, 2, 0, 'Vitalität');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (19, 2, 0, 'Artmächtigkeit');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (20, 2, 0, 'Herkunft der Population');
INSERT INTO {organism_artgroup_attr} (id, organism_artgroup_attr_type_id, users_uid, name) VALUES (21, 2, 0, 'Phänologische Phasen');


--
-- PostgreSQL database dump complete
--

