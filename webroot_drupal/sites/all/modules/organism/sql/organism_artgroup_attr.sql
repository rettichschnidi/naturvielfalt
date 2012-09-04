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


--
-- PostgreSQL database dump complete
--

