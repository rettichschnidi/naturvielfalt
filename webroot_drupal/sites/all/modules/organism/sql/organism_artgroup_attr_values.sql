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
-- Name: {organism_artgroup_attr_values_id_seq}; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_artgroup_attr_values_id_seq}', 77, true);


--
-- Data for Name: {organism_artgroup_attr_values}; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (1, 20, 'Natürlich oder eingebürgert');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (2, 20, 'Verwildert, nicht eingebürgert');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (3, 20, 'Adventiv/Sporadisch');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (4, 20, 'Herkunft unklar (Verdacht auf Ansiedlung)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (5, 20, 'Inoffizielle Ansiedlung (Herkunft unbekannt)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (6, 20, 'Offizielle Wiederansiedlung');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (7, 20, 'Kultiviert (Forst, Ornamental, Landwirtschaft');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (8, 15, 'Trittrasen (0,5 bis 1 m²)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (9, 15, 'Wiese und Weide (5 bis 25 m²)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (10, 15, 'Ruderalflur und Brache (10 bis 50 m²)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (11, 15, 'Ackerkrautschicht (20 bis 80 m²)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (12, 15, 'Wald und Forst (100 bis 500 m²)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (13, 19, 'r - selten, ein Exemplar)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (14, 19, '+ - wenige (2 bis 5) Exemplare - bis 1%');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (15, 19, '1 - viele (6 bis 50) Exemplare - bis 5%');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (16, 19, '2 - sehr viele (über 50) Exemplare - 5 bis 25');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (17, 19, '3 - 26 bis 50%');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (18, 19, '4 - 51 bis 75%');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (19, 19, '5 - 76 bis 100%');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (20, 18, 'oo - sehr schwach, zufällig gekeimt, sich nic');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (21, 18, 'o - geschwächt, kümmerlich');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (22, 18, 'Ø - geschwächt, kümmerlich durch sichtbare Sc');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (23, 18, '• - normal');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (24, 18, '•• - überaus kräftig');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (25, 21, '1 - Ohne Blütenstände');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (26, 21, '2 - Beginn Ähren-/ Rispenschieben/ Blüten-sta');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (27, 21, '3 - Ähren-/ Rispenschieben/ Blütenstand-schie');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (28, 21, '4 - Abschluss Ähre-/Rispenschieben/ Blüten-st');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (29, 21, '5 - Blühbeginn');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (30, 21, '6 - Vollblüte');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (31, 21, '7 - Ende der Blüte');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (32, 21, '8 - Fruchtansatz');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (33, 21, '9 - Reife');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (34, 21, '10 - Samen ausgefallen ');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (35, 4, 'Eier');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (36, 4, 'Larven');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (37, 4, 'Jungtiere');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (38, 4, 'Adulte');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (39, 10, 'Wandernde Tiere');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (40, 10, 'Balzende Tiere oder Paarung');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (41, 10, 'Eiablage');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (42, 10, 'Häutung (Haut)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (43, 10, 'Überwinterung');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (44, 10, 'Ausgesetzt');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (45, 10, 'Sonstiges');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (46, 4, 'Kokon (Ei)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (47, 4, 'Jungspinne');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (48, 4, 'Häutung (Haut)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (49, 4, 'Brut');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (50, 4, 'Jungvogel');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (51, 4, 'Larve');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (52, 4, 'Jungmuschel');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (53, 2, 'Nicht Starr');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (54, 2, 'Kältestarre');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (55, 2, 'Trockenstarre');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (56, 4, 'Ei');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (57, 4, 'Puppe');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (58, 4, 'Imago');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (59, 4, 'mehrere Stadien');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (60, 4, 'Fund, Fang');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (61, 4, 'Totfund');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (62, 4, 'Gewölle');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (63, 4, 'Winterquartier');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (64, 4, 'Wochenstube');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (65, 4, 'Indirekter Fortpflanzungsnachweis');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (66, 4, 'Schlafplatz');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (67, 4, 'Quartier (Flm.)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (68, 4, 'Nest, Bau, Burg');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (69, 4, 'Sonstiger Nachweis');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (70, 4, 'Jagdgebiet');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (71, 4, 'Detektor-/Sichtnachweis');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (72, 4, 'Wochenstube und Winterquartier');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (73, 4, 'Hinweis');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (74, 10, 'wandernde Tiere');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (75, 10, 'balzende Tiere oder Paarung');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (76, 10, 'ausgesetzt');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value) VALUES (77, 10, 'sonstiges');


--
-- PostgreSQL database dump complete
--

