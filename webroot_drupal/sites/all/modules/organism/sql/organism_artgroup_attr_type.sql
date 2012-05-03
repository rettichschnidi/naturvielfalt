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
-- Name: {organism_artgroup_attr_type_id_seq}; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_artgroup_attr_type_id_seq}', 6, true);


--
-- Data for Name: {organism_artgroup_attr_type}; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup_attr_type} (id, name, format) VALUES (1, 'Zeichenkette', 'string');
INSERT INTO {organism_artgroup_attr_type} (id, name, format) VALUES (2, 'Dropdown', 'dropdown');
INSERT INTO {organism_artgroup_attr_type} (id, name, format) VALUES (3, 'Datum', 'date');
INSERT INTO {organism_artgroup_attr_type} (id, name, format) VALUES (4, 'Zahl', 'int');
INSERT INTO {organism_artgroup_attr_type} (id, name, format) VALUES (5, 'Ja/Nein', 'bool');
INSERT INTO {organism_artgroup_attr_type} (id, name, format) VALUES (6, 'Dropdown (Mehrfach)', 'multi dropdown');


--
-- PostgreSQL database dump complete
--

