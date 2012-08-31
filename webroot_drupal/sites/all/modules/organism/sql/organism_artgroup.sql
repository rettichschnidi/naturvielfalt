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

SELECT pg_catalog.setval('{organism_artgroup_id_seq}', 28, true);


--
-- Data for Name: {organism_artgroup}; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup} (id, name, parent) VALUES (1, 'Alle', NULL);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (11, 'Eintagsfliegen', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (5, 'Mollusken', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (15, 'Nachtfalter', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (18, 'Netzflügler', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (27, 'Fische', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (7, 'Käfer', 28);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (25, 'Flechten', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (24, 'Flora', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (14, 'Hautflügler', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (9, 'Heuschrecken', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (28, 'Käfer (alle)', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (20, 'Köcherfliegen', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (21, 'Krebse', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (8, 'Laufkäfer', 28);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (17, 'Libellen', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (26, 'Pilze', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (23, 'Reptilien', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (22, 'Säugetiere', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (3, 'Spinnen', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (19, 'Steinfliegen', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (16, 'Tagfalter', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (2, 'Amphibien', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (6, 'Tausendfüsser', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (4, 'Vögel', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (13, 'Wanzen', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (12, 'Zikaden', 1);
INSERT INTO {organism_artgroup} (id, name, parent) VALUES (10, 'Zweiflügler', 1);


--
-- PostgreSQL database dump complete
--

