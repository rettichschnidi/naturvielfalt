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

SELECT pg_catalog.setval('{organism_artgroup_id_seq}', 1, false);


--
-- Data for Name: {organism_artgroup}; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (3, 'Alle', 3, 100);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (31, 'Schnecken', 71, 100);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (32, 'Muscheln', 71, 200);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (33, 'Vielborster', 93, 300);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (34, 'Spinnentiere', 93, 400);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (35, 'Höhere Krebse', 96, 99999);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (36, 'Doppelfüßer', 93, 600);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (37, 'Insekten', 93, 700);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (39, 'Strahlenflosser (Fische)', 3, 2500);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (40, 'Amphibien', 3, 2000);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (41, 'Reptilien', 3, 2100);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (42, 'Vögel', 3, 1500);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (44, 'Flora', 3, 500);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (45, 'Eintagsfliegen', 37, 2900);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (46, 'Gleichflügler, Blattläuse', 37, 3000);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (47, 'Gleichflügler, Zikaden', 37, 3100);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (48, 'Gottesanbeterin, Schaben, Termiten', 37, 3200);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (49, 'Hautflügler, Ameisen', 37, 3300);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (50, 'Hautflügler, Goldwespen', 37, 3400);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (51, 'Hautflügler, Grab-, Weg-, Faltenwespen...', 37, 3500);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (52, 'Hautflügler, Wildbienen und Spheciden', 37, 3600);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (53, 'Heuschrecken', 37, 3700);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (54, 'Käfer, andere Gruppen', 37, 3800);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (55, 'Käfer, Holzkäfer', 37, 3900);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (56, 'Käfer, Laufkäfer und Kurzflügler', 37, 4000);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (57, 'Käfer, pflanzenfressend, Blatt-, Rüssel-, Maikäfer...', 37, 4100);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (58, 'Käfer, Wasserkäfer', 37, 4200);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (59, 'Köcherfliegen', 37, 4300);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (60, 'Krebse, Asseln, Skorpione', 93, 4400);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (61, 'Krebse, Flusskrebse', 93, 4500);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (62, 'Libellen', 37, 4600);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (63, 'Netzflügler', 37, 4700);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (64, 'Schmetterlinge, Grossschmetterlinge, Glasflügler', 37, 4800);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (65, 'Schmetterlinge, Kleinschmetterlinge', 37, 4900);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (66, 'Schmetterlinge, Tagfalter und Zygeniden', 37, 5000);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (67, 'Steinfliegen', 37, 5200);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (68, 'Tausendfüssler', 93, 5300);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (69, 'Wanzen, Landwanzen', 37, 5400);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (70, 'Wanzen, Wasserwanzen', 37, 5500);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (71, 'Weichtiere', 3, 5600);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (72, 'Zecken', 37, 5700);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (73, 'Zweiflügler, Acalyptera', 37, 5800);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (74, 'Zweiflügler, Aschiza (Phoriden, Syrphiden)', 37, 5900);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (75, 'Zweiflügler, Calyptera', 37, 6000);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (76, 'Zweiflügler, Nematocera, Culicomorpha', 37, 6100);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (77, 'Zweiflügler, Nematocera, Tipulomorpha', 37, 6200);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (78, 'Zweiflügler, Orthorrhapha', 37, 6300);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (80, 'Käfer', 96, 99999);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (81, 'Wanzen', 96, 99999);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (82, 'Zweiflügler', 96, 99999);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (83, 'Schmetterlinge', 96, 99999);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (84, 'Krebse', 96, 99999);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (86, 'Gleichflügler', 96, 99999);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (88, 'Hautflügler', 96, 99999);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (89, 'Gottesanbeterinnen', 96, 99999);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (93, 'Gliederfüssler', 3, 4000);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (95, 'Fledermäuse', 3, 1350);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (96, 'zz Alt', 3, 1000000);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (97, 'Alle', 37, 0);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (43, 'Säugetiere', 3, 1300);
INSERT INTO {organism_artgroup} (id, name, parent, pos) VALUES (38, 'Wirbeltiere', 3, 2800);


--
-- PostgreSQL database dump complete
--

