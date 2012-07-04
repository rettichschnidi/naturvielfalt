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
-- Name: {organism_artgroup_detmethod_id_seq}; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_artgroup_detmethod_id_seq}', 49, true);


--
-- Data for Name: {organism_artgroup_detmethod}; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (1, 'Sichtbeobachtung', 100);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (2, 'Fang', 101);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (3, 'Mit Fernglas bestimmt', 102);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (4, 'Ruf, Schrei, Gesang', 103);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (5, 'Gesang und Sichtbeobachtung', 104);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (6, 'Ultraschall', 105);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (7, 'Ruf (Ohr) und Foto', 106);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (8, 'Video', 107);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (9, 'Radiotelemetriedaten', 108);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (10, 'Foto', 109);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (11, 'Unter Binokular bestimmt', 110);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (12, 'genetisch bestimmt', 111);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (13, 'Genitalbestimmung', 112);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (14, 'Morphologische Bestimmung', 113);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (15, 'Kunstbau Biber', 201);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (16, 'Nest, Biberburg', 202);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (17, 'Puppenkammer', 203);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (18, 'Erdbau, Höhle, Auswurfhügel', 204);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (19, 'Suhle', 205);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (20, 'Damm', 206);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (21, 'Burg eingestürzt', 207);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (22, 'Schwimmende Burg', 208);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (23, 'Mittelbau Biber', 209);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (24, 'Schlupfloch, Fluchtröhre', 210);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (25, 'Totfund', 300);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (26, 'Verunfallt (Strasse)', 301);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (27, 'Abschuss', 302);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (28, 'Schädel', 303);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (29, 'Knochenreste', 304);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (30, 'Haare', 305);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (31, 'Gewölle', 306);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (32, 'Reste Körpergewebe, Muskel, Blut', 307);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (33, 'Lebendiges Tier, frisches Gehäuse', 308);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (34, 'Wahrscheinliches Vorkommen', 309);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (35, 'Status unklar', 310);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (36, 'Vorkommen vermutlich ehemalig', 311);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (37, 'Subfossile Gehäuse', 312);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (38, 'Kot, Losung', 313);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (39, 'Köcher', 314);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (40, 'Ausstieg, Wechsel', 400);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (41, 'Kanal', 401);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (42, 'Fällplatz', 402);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (43, 'Trittsiegel, Spurenfolge', 403);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (44, 'Schäden', 404);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (45, 'Nahrungsreste, Einzelfrassspuren', 405);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (46, 'Wintervorrat', 406);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (47, 'Frassplatz', 407);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (48, 'Raubtierriss', 408);
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id) VALUES (49, 'Markierung, Bibergeil', 409);


--
-- PostgreSQL database dump complete
--

