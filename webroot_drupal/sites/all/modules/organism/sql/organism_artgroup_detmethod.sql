--
-- PostgreSQL database dump
--

-- Started on 2013-08-21 15:41:58

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

--
-- TOC entry 3879 (class 0 OID 0)
-- Dependencies: 538
-- Name: organism_artgroup_detmethod_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('{organism_artgroup_detmethod_id_seq}', 49, true);


--
-- TOC entry 3876 (class 0 OID 91741)
-- Dependencies: 539
-- Data for Name: organism_artgroup_detmethod; Type: TABLE DATA; Schema: public; Owner: postgres
--

INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (1, 'Sichtbeobachtung', 100, 'identified at sight', 'déterminé à vue', 'identificato a vista');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (2, 'Fang', 101, 'captured, identified in hand', 'capturé, déterminé en main', 'catturato, identificato in mano');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (3, 'Mit Fernglas bestimmt', 102, 'identified with binoculars', 'déterminé aux jumelles', 'identificato con il cannocchiale');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (4, 'Ruf, Schrei, Gesang', 103, 'identified by song, cry, voice', 'déterminé au cri, voix, chant (à l''oreille)', 'identificazione uditiva: verso, canto, richiamo');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (5, 'Gesang und Sichtbeobachtung', 104, 'identified by sight and song', 'déterminé au chant et à vue', 'identificato dal canto e a vista');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (6, 'Ultraschall', 105, 'identified by ultrasonic sound (batbox)', 'déterminé aux ultrasons / au chant par batbox', 'identificazione ultrasuoni, dal canto con Batbox');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (7, 'Ruf (Ohr) und Foto', 106, 'identified by song and photograph', 'déterminé au chant et sur photo', 'identificato dal canto (udito) e da fotografia');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (8, 'Video', 107, 'identified by video sequence', 'déterminé sur séquence vidéo', 'identificato su una sequenza video');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (9, 'Radiotelemetriedaten', 108, 'radio telemetric data', 'radiotélémétrie', 'dati radiotelemetrici');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (10, 'Foto', 109, 'identified on photo', 'déterminé sur photo', 'identificato su una fotografia');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (11, 'Unter Binokular bestimmt', 110, 'indenfied under binocular microscope', 'déterminé au binoculaire', 'identificato sotto lente');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (12, 'genetisch bestimmt', 111, 'identified genetically', 'déterminé génétiquement', 'identificazione genetica');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (13, 'Genitalbestimmung', 112, 'identified by genitalia, anatomically', 'déterminé au génitalia, anatomiquement', 'identificato con i genitali, anatomica');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (14, 'Morphologische Bestimmung', 113, 'identified by morphological criteria', 'déterminé morphologiquement', 'determinato con criteri morfologici');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (15, 'Kunstbau Biber', 201, 'artificial hut, lodge', 'hutte artificielle', 'rifugio artificiale');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (16, 'Nest, Biberburg', 202, 'nest, hut, lodge', 'nid, hutte', 'nido, rifugio');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (17, 'Puppenkammer', 203, 'pupal case', 'loge nymphale', 'camera ninfale');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (18, 'Erdbau, Höhle, Auswurfhügel', 204, 'burrow, retreat, molehill', 'terrier, tannire, taupinire', 'tana, cavità, cumulo di Talpa');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (19, 'Suhle', 205, 'wallow', 'bauge, souille', 'tana, brago');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (20, 'Damm', 206, 'dam', 'digue, barrage', 'diga, sbarramento');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (21, 'Burg eingestürzt', 207, 'collapsed burrow', 'terrier effondré', 'tana crollata');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (22, 'Schwimmende Burg', 208, 'floating hut', 'hutte en île', 'rifugio isola');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (23, 'Mittelbau Biber', 209, 'lodge, hut', 'terrier-hutte', 'tana rifugio');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (24, 'Schlupfloch, Fluchtröhre', 210, 'hideout', 'trou de sortie, galerie, abri, amorce', 'buco d''uscita, galleria, riparo, abbozzo');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (25, 'Totfund', 300, 'carcass', 'trouvé mort, cadavre', 'trovato morto, cadavere');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (26, 'Verunfallt (Strasse)', 301, 'run over (road)', 'accidenté, écrasé (route)', 'incidentato, schiacciato (strada)');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (27, 'Abschuss', 302, 'shot', 'tiré', 'sparato');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (28, 'Schädel', 303, 'skull', 'crâne', 'cranio');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (29, 'Knochenreste', 304, 'bones, part of insects, remains', 'ossements, parties d''insectes, restes...', 'ossa, parti d''insetto, resti');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (30, 'Haare', 305, 'hair', 'poils', 'peli');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (31, 'Gewölle', 306, 'rejection pellet', 'pelotes de réjection', 'borre');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (32, 'Reste Körpergewebe, Muskel, Blut', 307, 'remains of tissue, muscle, blood', 'restes tissus organiques, muscle, sang', 'resti tessuti organici, muscoli, sangue');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (33, 'Lebendiges Tier, frisches Gehäuse', 308, 'living individual or recent shell', 'individu vivant, coquille fraîche', 'conchiglia fresca, pop. vivente reale');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (34, 'Wahrscheinliches Vorkommen', 309, 'presence of population probable', 'population vivante probable', 'popolazione vivente probabile');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (35, 'Status unklar', 310, 'presence of population doubtful', 'population de statut peu clair', 'popolazione con status incerto');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (36, 'Vorkommen vermutlich ehemalig', 311, 'shell, washed out (occurrence probably historical)', 'coquille lessivée', 'conchiglia slavata, pop. storica probabile');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (37, 'Subfossile Gehäuse', 312, 'shell, subrecent', 'coquilles subfossiles', 'conchiglia subfossile');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (38, 'Kot, Losung', 313, 'faeces, droppings, toilets', 'crottes, latrines, toilettes, pétolires', 'feci, pallette di feci, latrine, toeletta');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (39, 'Köcher', 314, 'case', 'fourreau', 'astuccio');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (40, 'Ausstieg, Wechsel', 400, 'exit, trail, runway', 'sortie, passage, passe, coulée', 'uscita, passaggio, passo, strisciata');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (41, 'Kanal', 401, 'canal', 'canal', 'canale di Castoro');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (42, 'Fällplatz', 402, 'felling area', 'chantier', 'cantiere di Castoro');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (43, 'Trittsiegel, Spurenfolge', 403, 'footprint, track', 'empreintes, piste', 'impronte, pista');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (44, 'Schäden', 404, 'damage', 'dégâts', 'danni');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (45, 'Nahrungsreste, Einzelfrassspuren', 405, 'trace of feeding', 'restes de repas, traces de rongements', 'resti di cibo, tracce di rosura');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (46, 'Wintervorrat', 406, 'food cache', 'dépôt hivernal de nourriture', 'riserva invernale di nutrimento Castoro');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (47, 'Frassplatz', 407, 'feeding area', 'réfectoire, mangeoires', 'mangiatoia Castoro');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (48, 'Raubtierriss', 408, 'predation marks', 'traces de prédation', 'tracce di predazione');
INSERT INTO {organism_artgroup_detmethod} (id, name, cscf_id, name_en, name_fr, name_it) VALUES (49, 'Markierung, Bibergeil', 409, 'branding (castoreum)', 'marquage, castoréum', 'marca, castoreum');


-- Completed on 2013-08-21 15:41:58

--
-- PostgreSQL database dump complete
--

