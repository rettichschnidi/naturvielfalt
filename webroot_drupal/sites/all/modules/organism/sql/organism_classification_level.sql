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
-- Name: {organism_classification_level_id_seq}; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_classification_level_id_seq}', 13, true);


--
-- Data for Name: {organism_classification_level}; Type: TABLE DATA; Schema: public; Owner: postgres
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


--
-- PostgreSQL database dump complete
--

