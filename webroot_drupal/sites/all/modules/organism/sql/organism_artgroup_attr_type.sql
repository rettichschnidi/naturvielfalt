--
-- PostgreSQL database dump
--

-- Started on 2013-08-16 13:41:55

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

--
-- TOC entry 3875 (class 0 OID 0)
-- Dependencies: 534
-- Name: organism_artgroup_attr_type_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_artgroup_attr_type_id_seq}', 1, false);


--
-- TOC entry 3872 (class 0 OID 91719)
-- Dependencies: 535
-- Data for Name: organism_artgroup_attr_type; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup_attr_type} (id, name, format) VALUES (2, 'text', 'textarea');
INSERT INTO {organism_artgroup_attr_type} (id, name, format) VALUES (1, 'string', 'textfield');
INSERT INTO {organism_artgroup_attr_type} (id, name, format) VALUES (3, 'options', 'select');


-- Completed on 2013-08-16 13:41:55

--
-- PostgreSQL database dump complete
--

