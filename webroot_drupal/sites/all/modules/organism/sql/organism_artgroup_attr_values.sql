--
-- PostgreSQL database dump
--

-- Started on 2013-08-22 10:03:17

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

--
-- TOC entry 3879 (class 0 OID 0)
-- Dependencies: 541
-- Name: organism_artgroup_attr_values_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_artgroup_attr_values_id_seq}', 1, true);


--
-- TOC entry 3876 (class 0 OID 201654)
-- Dependencies: 542
-- Data for Name: organism_artgroup_attr_values; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (1, 6, 'Sichere Bestimmung', 'reliable determination', 'certain détermination', 'certo classificazione');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (2, 6, 'Unsichere Bestimmung', 'uncertain determination', 'incertain détermination', 'dubblo classificazione');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (10, 1, 'sicher im Kanton / in der Region', 'in the canton region', 'certain dans la région', 'certo nello regione');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (11, 4, NULL, NULL, NULL, NULL);
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (12, 4, '<11 Ex.', '<11 Ex.', '<11 Ex.', '<11 Ex.');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (99, 11, NULL, NULL, NULL, NULL);
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (15, 4, '51-100 Ex.', '51-100 Ex.', '51-100 Ex.', '51-100 Ex.');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (14, 4, '26-50 Ex.', '26-50 Ex.', '26-50 Ex.', '26-50 Ex.');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (13, 4, '11-25 Ex.', '11-25 Ex.', '11-25 Ex.', '11-25 Ex.');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (16, 4, '101-500 Ex.', '101-500 Ex.', '101-500 Ex.', '101-500 Ex.');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (17, 4, '501- 1000 Ex.', '501- 1000 Ex.', '501- 1000 Ex.', '501- 1000 Ex.');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (18, 4, '1001-2500 Ex.', '1001-2500 Ex.', '1001-2500 Ex.', '1001-2500 Ex.');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (19, 4, '2501-5000 Ex.', '2501-5000 Ex.', '2501-5000 Ex.', '2501-5000 Ex.');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (20, 4, '5001-10000 Ex.', '5001-10000 Ex.', '5001-10000 Ex.', '5001-10000 Ex.');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (21, 4, '>10000 Ex.', '>10000 Ex.', '>10000 Ex.', '>10000 Ex.');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (100, 11, 'Direktbeobachtung (Sichtkontakt, Ruf)', 'direct observation', 'Observation directe (vue, son)', 'Osservazione diretta (vista, suono)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (101, 11, 'Sichtbeobachtung', 'identified at sight', 'Chasse à vue', 'Caccia a vista');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (102, 11, 'Restlichtverstärker', 'residual light amplifier', 'Amplificateur lumire', 'Amplificatore luminoso');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (103, 11, 'Ultraschalldetektor', 'ultrasound detector', 'Détecteur ultrasons, Bat box', 'Detector a ultrasuoni, Batbox');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (104, 11, 'Gesang', 'song', 'Ecoute', 'Ascolto');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (105, 11, 'Flächendeckende Vorzugspunktbeobachtung', 'aera-wide preference point observation', 'Observation visuelle couvrante ciblée', 'Osservazione mirata su un''area definita');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (106, 11, 'Transektbeobachtung, Zählungen', 'counting', 'Transect, recensement', 'Transetto, censimento');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (107, 11, 'Scheinwerfer, Scheinwerfertaxation', 'flood light, taxation of flood light', 'Phare, taxation au phare', 'Faro, rilevamento con faro');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (22, 5, NULL, NULL, NULL, NULL);
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (23, 5, 'Steril', 'sterile', 'stérile', 'sterile');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (108, 11, 'Nächtliche Suche mit Taschenlampe', 'torch lamp', 'Lampe de poche', 'Lampada da tasca');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (109, 11, 'Radiotelemetrie', 'radiotelemetry', 'Radiotélémétrie', 'Radiotelemetria');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (110, 11, 'Stadtbeleuchtung', 'city lighting', 'Eclairage de ville', 'Illuminazione cittadina');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (111, 11, 'Fotofalle', 'photo-trap', 'Pigè photo', 'Trappola fotografica');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (112, 11, 'Umfrage', 'survey', 'Enqute', 'Inchiesta');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (113, 11, 'Aussetzung', 'abandonment', 'Lâcher', 'Rilascio');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (114, 11, 'Markierte Individuen', 'marked individuals', 'Individus marqués', 'Individuo marcato');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (24, 5, 'Noch nicht blühend', 'not yet blooming', 'pas encore florissant', 'non ancora in fiore');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (25, 5, 'Mit Blütenknospen', 'with flower buds', 'avec bourgeons floraux', 'Con boccioli di fiori');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (200, 11, 'Fang', 'catch', 'Capture directe', 'Cattura diretta');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (201, 11, 'Sauggerät', 'suction device', 'Aspirateur (à main)', 'Aspiratore (manuale)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (202, 11, 'Unterwasser-Sauger', 'underwater suction device', 'Aspirateur sous-marin', 'Aspiratore subacqueo');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (203, 11, 'Handfang', 'manual catch', 'Capture à la main', 'Catturato a mano');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (26, 5, 'Anfang Blüte', 'start of blossoming', 'début de la floraison', 'ai primi di flore');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (204, 11, 'Zelt', 'tent', 'Drapeau', 'Tenda');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (205, 11, 'Zucht (ex Ovo, ex Larva, ex Pupa)', 'breeding', 'Elevage', 'Allevamento');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (27, 5, 'Vollblüte', 'full flowering', 'pleine floraison', 'full flowering');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (206, 11, 'Netzfang', 'netcatches', 'Filet', 'Retino');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (207, 11, 'Entomologennetz', 'entomology net', 'Filet entomologique', 'Retino entomologica');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (208, 11, 'Kescher', 'landing net', 'Filet fauchoir', 'Retino da sfalcio');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (28, 5, 'Ende Blüte', 'end of flowering', 'fin de la floraison', 'fine flore');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (209, 11, 'Netzfischerei', 'net fishing', 'Filet de pche', 'Rete da pesca');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (210, 11, 'Planktonnetz', 'plankton net', 'Filet à plancton', 'Retino per plancton');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (29, 5, 'Fruchtend', 'fruiting', 'tiges fleuries', 'fruttare');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (30, 5, 'Absterbend', 'dying off', 'mourir', 'morire');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (31, 7, NULL, NULL, NULL, NULL);
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (32, 7, 'Natürlich oder eingebürgert', 'natural or naturalized', 'naturel ou naturalisé', 'naturale o naturalizzati');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (33, 7, 'Adventiv', 'adventitious', 'adventif', 'avventizio');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (34, 7, 'Verwildert', 'savaged', 'redevenu sauvage', 'inselvatichito');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (35, 7, 'Angesiedelt', 'resident', 'tre implanté', 'popolato');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (36, 7, 'Kultiviert', 'cultivated', 'cultiver', 'coltivare');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (211, 11, 'CO2-Begasung (RCF)', 'CO2-gassing', 'Gazage CO2', 'Gasare CO2');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (212, 11, 'Begasung, Insektizid', 'gassing, insecticide', 'Gazage, insecticide', 'Gasare, insetticida');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (213, 11, 'Auf Wirt entnommen', 'extracted from host', 'Récolté sur l''hote', 'Raccolto sull''ospite');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (37, 7, 'Wiederansiedlung', 'resettlement', 'réintroduction', 'Il reinsediamento');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (38, 7, 'Herkunft unklar', 'origin unclear', 'origine peu confus', 'origine indefinito');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (39, 8, NULL, NULL, NULL, NULL);
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (40, 8, 'vorhanden', 'available', 'existant', 'esistente');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (41, 8, 'Nicht festgestellt', 'not detected', 'n''est pas détecté', 'non percepire');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (42, 8, 'Nicht festgestellt, Vorhandensein wahrscheinlich', 'not detected, existency probable', 'n''est pas détecté, existence probable', 'non precepire, esistenza probabile');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (43, 8, 'Nicht festgestellt, Vorhandensein unwahrscheinlich', 'not detected, existency unprobable', 'n''est pas détecté, existence invraisemblable', 'non precepire, esistenza non probabile');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (44, 8, 'Erloschen/zerstört', 'extinct', 'pris fin', 'distruggere');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (45, 8, 'Standort unzugänglich', 'location inaccessible', 'exposition inaccessible', 'posizione inaccessibile');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (46, 9, NULL, NULL, NULL, NULL);
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (48, 9, 'Herbarbeleg', 'herbarium record', 'justificatif de herbacé', 'herbarium record');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (49, 9, 'Foto', 'picture', 'photo', 'foto');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (50, 9, 'Beides', 'both', 'les deux', 'entrambe le cose');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (214, 11, 'Sieb', 'colander', 'Passoire', 'Setaccio');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (215, 11, 'Biocönometer', 'bioconometer', 'Biocénomètre', 'Biocenometro');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (216, 11, 'Elektrofang', 'electrofishing', 'Pche électrique', 'Pesca elettrica');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (217, 11, 'Japanschirm, Klopfschirm', 'Japan umbrella, knock umbrella', 'Parapluie japonais, battage', 'Ombrello giapponese, battitura');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (218, 11, 'Taucherbeobachtung', 'diver observation', 'Plongée', 'Immersione');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (219, 11, 'Puppenkammer', 'puparium', 'Piochon (loge nymphale, restes)', 'Camera pupale');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (220, 11, 'Surber', 'Surber', 'Surber', 'Surber');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (300, 11, 'Lockfallen', 'lure traps', 'Pigès attractifs', 'Trappola attrattiva');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (3, 1, NULL, NULL, NULL, NULL);
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (4, 1, 'Genauigkeit von 1 - 10 Meter', 'accuracy of 1 to 10 meter', 'précision de 1-10 mètre', 'Precisione di 1-10m');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (5, 1, 'Genauigkeit von 11 - 50 Meter', 'accuracy of 11 to 50 meter', 'précision de 11-50 mètre', 'Precisione di 11-50m');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (6, 1, 'Genauigkeit von 50 - 250 Meter', 'accuracy of 50 to 250 meter', 'précision de 50-250 mètre', 'Precisione di 50-250m');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (7, 1, 'innerhalb eines km2, ohne Angabe der genauen Koordinaten', 'within one km2, without any precise geographic information', 'dans le km2, sans coordonnée', 'all''interno di 1km2, non coordinate');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (8, 1, 'in einem Perimeter der 9 km2 umfasst', 'within a perimeter of 9km2', 'dans le périmètre de 9km2', 'within a perimeter of 9km2');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (9, 1, 'in einem Perimeter der 100 km2 umfasst', 'within a perimeter of 100km2', 'dans le périmètre de 100km2', 'within a perimeter of 9km2');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (301, 11, 'Farbteller', 'color plate', 'Assiette colorée', 'Piatto colorato');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (302, 11, 'Zuckerfalle', 'sugar trap', 'Appât sucré, miellée', 'Esche zuccherine');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (303, 11, 'Köderfalle', 'bait trap', 'Pigè appât', 'Trappola con esca');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (304, 11, 'Weinfalle', 'wine trap', 'Pigè à vin', 'Trappola al vino');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (305, 11, 'Lichtfalle', 'light trap', 'Pigè lumineux', 'Trappola luminosa');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (306, 11, 'Lampe mit aktinischer Strahlung', 'lamp with actinic radiation', 'Pigè lumineux actinique', 'Trappola luminosa actinica');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (307, 11, 'Automatische Lichtfalle (Changins-Falle)', 'automatic light-trap (changings-trap)', 'Pigè lumineux automatique (Pigè Changins)', 'Trappola luminosa automatica (Trap. Changins)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (308, 11, 'Quecksilberlampe', 'mercury lamp', 'Pigè lampe au Mercure', 'Lampada al mercurio');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (309, 11, 'Robinson-Lichtfalle', 'Robinson-light trap', 'Pigè lumineux Robinson', 'Trappola luminosa di Robinson');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (310, 11, 'UV-Lichtfalle', 'UV-light trap', 'Pigè lumineux (UV)', 'Trappola luminosa a UV');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (311, 11, 'Pheromonfalle', 'pheromone trap', 'Pigè à phéromones', 'Trappola a feromoni');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (312, 11, 'Licht-Malaise-Falle', 'light-malaise trap', 'Tente malaise lumineuse', 'Tenda Malaise luminosa');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (313, 11, 'Mischlicht-Falle', 'mixed light trap', 'Pigè lumineux (Mixte)', 'Trappola luminosa mista');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (314, 11, 'Bierfalle', 'beer trap', 'Pigè à bire', 'Trappola alla birra');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (400, 11, 'Abfangfallen', 'interception trap', 'Pigès d''interception', 'Trappola a intercezione');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (401, 11, 'Mini-Malaise-Falle', 'mini-malaise trap', 'Mini-tente malaise', 'Mini trappola Malaise');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (402, 11, 'Schlingenfalle', 'loop trap', 'Pigè à lacet', 'Trappola a laccio');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (403, 11, 'Saugfalle', 'suction trap', 'Pigè à aspiration', 'Trappola ad aspirazione');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (404, 11, 'Bodenfalle (Baberfallen)', 'pitfall traps (Baber-traps)', 'Pigè Barber', 'Trappola Barber (a caduta)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (405, 11, 'Kastenfalle', 'box trap', 'Pigè-boîte', 'Trappola a scatola');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (406, 11, 'Kombinierte Falle (Fensterfalle-Farbteller)', 'combined trap (window trap, color plate)', 'Pigè combiné (fentre-assiette colorée)', 'Trappola combinata (finestra-piatto colorato)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (407, 11, 'Klebgitterfallen', 'adhesive grid trap', 'Pigè collant, plaquette colante', 'Trappola adesiva, piatto colloso');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (408, 11, 'Schlüpffalle', 'emerging trap', 'Pigè à émergence', 'Trappola a emersione');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (409, 11, 'Fensterfalle', 'window trap', 'Pigè fentre', 'Trappola a finestra');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (410, 11, 'Fadenfalle (Klebfalle)', 'thread trap (adhesive trap)', 'Pigè à fils', 'Trappola a filo');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (411, 11, 'Bodenbrett', 'bottom board', 'Planche au sol', 'Piastra al suolo');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (412, 11, 'Malaise-Falle', 'malaise trap', 'Tente malaise', 'Tenda Malaise');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (413, 11, 'Nisthilfe, Heckbauer', 'nesting aid', 'Nichoir, nid artificiel', 'Gabbia, nido artificiale');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (414, 11, 'Richtfalle', 'direction trap', 'Pigè directionnel', 'Trappola direzionale');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (415, 11, 'Trichterfalle', 'funnel trap', 'Entonnoir', 'Trappola ad imbuto');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (416, 11, 'Haarfalle', 'hair trap', 'Pigè poils', 'Trappola peli');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (417, 11, 'Spurentunnel', 'track tunnel', 'Tunnel à traces', 'Galeria a tracce');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (418, 11, 'Lebendfalle', 'living trap', 'Pigès à capturer vivant', 'Trappola per la cattura viva');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (500, 11, 'Entnahme / Stichprobe (Boden, Abfall...)', 'sample', 'Prise / tri d''échantillon (sol, déchets...)', 'Presa /scelta di campione (suolo, scarto ...)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (501, 11, 'Bodenprobe', 'soil sample', 'Carrotage (éch. de sol)', 'Carotaggio (campione di suolo)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (502, 11, 'Berlese', 'berlese', 'Berlese', 'Berlese');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (503, 11, 'Genist', 'genist', 'Débris de crues', 'Resti di depositi di piene');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (504, 11, 'Dekantieren (Auswaschen)', 'decantation (washing-out)', 'Décantation (tri par lavage)', 'Decantazione (dilavazione)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (505, 11, 'Gesiebe', 'filtered material', 'Tamisage (éch. de sol)', 'Vagliatura (suolo)');
INSERT INTO {organism_artgroup_attr_values} (id, organism_artgroup_attr_id, value, value_en, value_fr, value_it) VALUES (506, 11, 'Gewöll', 'pellet', 'Pelote de réjection', 'Borra');


-- Completed on 2013-08-22 10:03:17

--
-- PostgreSQL database dump complete
--

