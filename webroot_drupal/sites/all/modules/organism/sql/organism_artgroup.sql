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

SELECT pg_catalog.setval('{organism_artgroup_id_seq}', 27, true);


--
-- Data for Name: {organism_artgroup}; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (1, 'Alle', NULL, 0);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (8, 'Laufkäfer', 7, 11);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (2, 'Amphibien', 1, 0);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (14, 'Hautflügler', 1, 6);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (11, 'Eintagsfliegen', 1, 2);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (24, 'Flora', 1, 4);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (26, 'Fungus', 1, 5);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (9, 'Heuschrecken', 1, 7);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (7, 'Käfer', 1, 8);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (20, 'Köcherfliegen', 1, 9);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (21, 'Krebse', 1, 10);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (17, 'Libellen', 1, 12);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (25, 'Lichens', 1, 13);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (5, 'Mollusken', 1, 14);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (15, 'Nachtfalter', 1, 15);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (18, 'Netzflügler', 1, 16);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (23, 'Reptilien', 1, 17);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (22, 'Säugetiere', 1, 18);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (3, 'Spinnen', 1, 19);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (19, 'Steinfliegen', 1, 20);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (16, 'Tagfalter', 1, 21);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (6, 'Tausendfüsser', 1, 22);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (4, 'Vögel', 1, 23);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (13, 'Wanzen', 1, 24);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (12, 'Zikaden', 1, 25);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (10, 'Zweiflügler', 1, 26);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (27, 'Fische', 1, 3);


--
-- PostgreSQL database dump complete
--

