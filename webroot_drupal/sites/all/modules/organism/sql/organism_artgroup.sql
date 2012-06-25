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
-- Name: {organism_artgroup_id_seq}; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_artgroup_id_seq}', 26, true);


--
-- Data for Name: {organism_artgroup}; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (1, 'Fische', 1, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (2, 'Amphibien', 2, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (3, 'Spinnen', 3, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (4, 'Vögel', 4, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (5, 'Mollusken', 5, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (6, 'Tausendfüsser', 6, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (7, 'Käfer', 7, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (8, 'Laufkäfer', 8, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (9, 'Heuschrecken', 9, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (10, 'Zweiflügler', 10, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (11, 'Eintagsfliegen', 11, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (12, 'Zikaden', 12, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (13, 'Wanzen', 13, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (14, 'Hautflügler', 14, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (15, 'Nachtfalter', 15, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (16, 'Tagfalter', 16, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (17, 'Libellen', 17, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (18, 'Netzflügler', 18, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (19, 'Steinfliegen', 19, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (20, 'Köcherfliegen', 20, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (21, 'Krebse', 21, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (22, 'Säugetiere', 22, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (23, 'Reptilien', 23, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (24, 'Flora', 24, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (25, 'Lichens', 25, 1);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (26, 'Fungus', 26, 1);


--
-- PostgreSQL database dump complete
--

