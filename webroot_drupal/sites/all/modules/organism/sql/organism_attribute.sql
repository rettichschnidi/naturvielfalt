--
-- PostgreSQL database dump
--

-- Started on 2013-08-22 11:58:20

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

--
-- TOC entry 3881 (class 0 OID 0)
-- Dependencies: 567
-- Name: organism_attribute_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_attribute_id_seq}', 10, true);


--
-- TOC entry 3878 (class 0 OID 204884)
-- Dependencies: 568
-- Data for Name: organism_attribute; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_attribute} (id, valuetype, name, comment) VALUES (1, 'n', 'NUESP', 'Number which got assigned to this organism by the CSCF.');
INSERT INTO {organism_attribute} (id, valuetype, name, comment) VALUES (2, 'n', 'CRSF number', 'Number which got assigned to this organism by the CRSF.');
INSERT INTO {organism_attribute} (id, valuetype, name, comment) VALUES (3, 't', 'Author', 'Name of the scientist who classified this organism.');
INSERT INTO {organism_attribute} (id, valuetype, name, comment) VALUES (4, 'b', 'Xenophyte', 'Wether this organism is imported or not.');
INSERT INTO {organism_attribute} (id, valuetype, name, comment) VALUES (5, 't', 'Flora Helvetica', 'Number which got assigned to this organism by the Flora Helvetica.');
INSERT INTO {organism_attribute} (id, valuetype, name, comment) VALUES (6, 'n', 'Fungus number', 'Number which got assigned to this organism by the WSL.');
INSERT INTO {organism_attribute} (id, valuetype, name, comment) VALUES (7, 't', 'First find', 'The year of the first documented sight of this organism.');
INSERT INTO {organism_attribute} (id, valuetype, name, comment) VALUES (8, 'b', 'Blacklist CPS/SKEW', 'List of invasive alien plants of Switzerland that actually cause damage in the areas of biodiversity, health, and/or economy. The establishment and the spread of these species must be prevented.');
INSERT INTO {organism_attribute} (id, valuetype, name, comment) VALUES (9, 'n', 'NISM number', 'Number which got assigned to the moss organism by the NISM.');
INSERT INTO {organism_attribute} (id, valuetype, name, comment) VALUES (10, 't', 'Red List 2004 (CH)', 'The status of the organism according to the red list 2004 (CH).');


-- Completed on 2013-08-22 11:58:20

--
-- PostgreSQL database dump complete
--

