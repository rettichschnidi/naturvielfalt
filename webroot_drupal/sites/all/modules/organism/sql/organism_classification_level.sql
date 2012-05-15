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

SELECT pg_catalog.setval('{organism_classification_level_id_seq}', 18, true);


--
-- Data for Name: {organism_classification_level}; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (1, 1, 1, 1, 16, 'CSCF');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (2, 1, 1, 2, 15, 'class');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (3, 2, 1, 3, 14, 'order');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (4, 3, 1, 4, 13, 'family');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (5, 4, 1, 5, 12, 'genus');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (6, 5, 1, 6, 11, 'subgenus');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (7, 6, 1, 7, 10, 'species');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (8, 7, 1, 8, 9, 'subspecies');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (9, 9, 9, 1, 10, 'CRSF');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (10, 9, 9, 2, 9, 'family');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (11, 10, 9, 3, 8, 'genus');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (12, 11, 9, 4, 7, 'species');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (13, 12, 9, 5, 6, 'subspecies');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (14, 14, 14, 1, 4, 'Fungus');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (15, 14, 14, 2, 3, 'class');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (16, 16, 16, 1, 6, 'Swisslichens');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (17, 16, 16, 2, 5, 'genus');
INSERT INTO {organism_classification_level} (id, parent_id, prime_father_id, left_value, right_value, name) VALUES (18, 17, 16, 3, 4, 'species');


--
-- PostgreSQL database dump complete
--

