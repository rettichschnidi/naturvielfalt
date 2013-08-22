--
-- PostgreSQL database dump
--

-- Started on 2013-08-22 11:59:54

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

--
-- TOC entry 3887 (class 0 OID 0)
-- Dependencies: 557
-- Name: organism_classification_level_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_classification_level_id_seq}', 17, true);


--
-- TOC entry 3884 (class 0 OID 204804)
-- Dependencies: 558
-- Data for Name: organism_classification_level; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (1, 1, 1, 1, 12, 'CSCF');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (2, 1, 1, 2, 11, 'class');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (3, 2, 1, 3, 10, 'order');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (4, 3, 1, 4, 9, 'family');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (5, 4, 1, 5, 8, 'genus');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (6, 5, 1, 6, 7, 'subgenus');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (7, 7, 7, 1, 6, 'CRSF');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (8, 7, 7, 2, 5, 'family');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (9, 8, 7, 3, 4, 'genus');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (10, 10, 10, 1, 4, 'Fungus');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (11, 10, 10, 2, 3, 'class');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (12, 12, 12, 1, 4, 'Swisslichens');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (13, 12, 12, 2, 3, 'genus');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (14, 14, 14, 1, 8, 'NISM');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (15, 14, 14, 2, 7, 'class');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (16, 15, 14, 3, 6, 'family');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (17, 16, 14, 4, 5, 'genus');


-- Completed on 2013-08-22 11:59:54

--
-- PostgreSQL database dump complete
--

