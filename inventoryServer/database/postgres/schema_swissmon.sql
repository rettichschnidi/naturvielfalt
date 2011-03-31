
--
-- Name: area; Type: TABLE; Schema: public; Owner: postgres; Tablespace: 
--

CREATE TABLE area (
    id bigint NOT NULL,
    type_id integer NOT NULL,
    field_name character varying(100) NOT NULL,
    altitude integer DEFAULT 0 NOT NULL,
    surface_area integer DEFAULT 0 NOT NULL,
    locality character varying(30) NOT NULL,
    zip character varying(6) NOT NULL,
    township character varying(30) NOT NULL,
    canton character varying(30) NOT NULL,
    country character varying(30) NOT NULL,
    centroid_lat double precision NOT NULL,
    centroid_lng double precision NOT NULL,
    comment text
);

ALTER TABLE public.area OWNER TO swissmon;

--
-- Name: COLUMN area.id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area.id IS 'PK';


--
-- Name: COLUMN area.type_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area.type_id IS 'area type: marker/polygon';


--
-- Name: COLUMN area.field_name; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area.field_name IS 'Name, Flurname';


--
-- Name: COLUMN area.altitude; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area.altitude IS 'Meter Ã¼ber Meer';


--
-- Name: COLUMN area.surface_area; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area.surface_area IS 'FlÃ¤che in (m2)';


--
-- Name: COLUMN area.centroid_lat; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area.centroid_lat IS 'Schwerpunkt Latitude';


--
-- Name: COLUMN area.centroid_lng; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area.centroid_lng IS 'Schwerpunkt Longitude';


--
-- Name: COLUMN area.comment; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area.comment IS 'Kommentar';


--
-- Name: area_habitat; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE area_habitat (
    id bigint NOT NULL,
    area_id integer NOT NULL,
    habitat_id integer NOT NULL
);


ALTER TABLE public.area_habitat OWNER TO swissmon;

--
-- Name: COLUMN area_habitat.id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area_habitat.id IS 'PK';


--
-- Name: COLUMN area_habitat.area_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area_habitat.area_id IS 'FK to area';


--
-- Name: COLUMN area_habitat.habitat_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area_habitat.habitat_id IS 'FK to habitat';


--
-- Name: area_habitat_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE area_habitat_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.area_habitat_id_seq OWNER TO swissmon;

--
-- Name: area_habitat_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE area_habitat_id_seq OWNED BY area_habitat.id;


--
-- Name: area_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE area_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.area_id_seq OWNER TO swissmon;

--
-- Name: area_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE area_id_seq OWNED BY area.id;


--
-- Name: area_point; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE area_point (
    id bigint NOT NULL,
    area_id integer NOT NULL,
    lat double precision DEFAULT 0 NOT NULL,
    lng double precision DEFAULT 0 NOT NULL,
    seq integer DEFAULT 0 NOT NULL
);


ALTER TABLE public.area_point OWNER TO swissmon;

--
-- Name: COLUMN area_point.id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area_point.id IS 'PK';


--
-- Name: COLUMN area_point.area_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area_point.area_id IS 'FK to area';


--
-- Name: COLUMN area_point.lat; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area_point.lat IS 'Latitude - Geographische Breite';


--
-- Name: COLUMN area_point.lng; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area_point.lng IS 'Longitude - Geographische LÃ¤nge';


--
-- Name: COLUMN area_point.seq; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area_point.seq IS 'Reihenfolge der einzelnen Punkte pro Polygon.';


--
-- Name: area_point_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE area_point_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.area_point_id_seq OWNER TO swissmon;

--
-- Name: area_point_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE area_point_id_seq OWNED BY area_point.id;


--
-- Name: area_type; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE area_type (
    id bigint NOT NULL,
    "desc" character varying(100) NOT NULL
);


ALTER TABLE public.area_type OWNER TO swissmon;

--
-- Name: COLUMN area_type.id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area_type.id IS 'PK';


--
-- Name: COLUMN area_type."desc"; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN area_type."desc" IS 'Description';


--
-- Name: area_type_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE area_type_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.area_type_id_seq OWNER TO swissmon;

--
-- Name: area_type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE area_type_id_seq OWNED BY area_type.id;


--
-- Name: attribute_format; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE attribute_format (
    id bigint NOT NULL,
    name character varying(45) NOT NULL,
    format character varying(45) NOT NULL
);


ALTER TABLE public.attribute_format OWNER TO swissmon;

--
-- Name: COLUMN attribute_format.name; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN attribute_format.name IS 'Name des Format';


--
-- Name: COLUMN attribute_format.format; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN attribute_format.format IS 'Format';


--
-- Name: attribute_format_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE attribute_format_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.attribute_format_id_seq OWNER TO swissmon;

--
-- Name: attribute_format_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE attribute_format_id_seq OWNED BY attribute_format.id;


--
-- Name: fauna_class; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE fauna_class (
    id bigint NOT NULL,
    name_lt character varying(50) NOT NULL,
    name_de character varying(50) NOT NULL
);


ALTER TABLE public.fauna_class OWNER TO swissmon;

--
-- Name: COLUMN fauna_class.id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_class.id IS 'PK';


--
-- Name: COLUMN fauna_class.name_lt; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_class.name_lt IS 'Name Lateinisch';


--
-- Name: fauna_class_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE fauna_class_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.fauna_class_id_seq OWNER TO swissmon;

--
-- Name: fauna_class_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE fauna_class_id_seq OWNED BY fauna_class.id;


--
-- Name: fauna_organism; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE fauna_organism (
    id integer NOT NULL,
    cscf_nr integer NOT NULL,
    fauna_class_id integer NOT NULL,
    "order" character varying(50) NOT NULL,
    family character varying(50) NOT NULL,
    genus character varying(50) NOT NULL,
    sub_genus character varying(50) NOT NULL,
    species character varying(50) NOT NULL,
    sub_species character varying(50) NOT NULL,
    author_year character varying(100) DEFAULT NULL::character varying,
    name_de character varying(255) DEFAULT NULL::character varying,
    name_fr character varying(255) DEFAULT NULL::character varying,
    name_it character varying(255) DEFAULT NULL::character varying,
    name_ro character varying(255) DEFAULT NULL::character varying,
    name_en character varying(255) DEFAULT NULL::character varying,
    protection_ch integer
);


ALTER TABLE public.fauna_organism OWNER TO swissmon;

--
-- Name: TABLE fauna_organism; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON TABLE fauna_organism IS 'Index von Faunaarten; Herkunft: CSCF';


--
-- Name: COLUMN fauna_organism.id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.id IS 'PK';


--
-- Name: COLUMN fauna_organism.cscf_nr; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.cscf_nr IS 'CSCF-Artcode';


--
-- Name: COLUMN fauna_organism.fauna_class_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.fauna_class_id IS 'FK to fauna_class.id: Klasse';


--
-- Name: COLUMN fauna_organism."order"; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism."order" IS 'Ordnung';


--
-- Name: COLUMN fauna_organism.family; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.family IS 'Familie';


--
-- Name: COLUMN fauna_organism.genus; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.genus IS 'Gattung';


--
-- Name: COLUMN fauna_organism.sub_genus; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.sub_genus IS 'Untergattung';


--
-- Name: COLUMN fauna_organism.species; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.species IS 'Art';


--
-- Name: COLUMN fauna_organism.sub_species; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.sub_species IS 'Unterart';


--
-- Name: COLUMN fauna_organism.author_year; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.author_year IS 'Autor, Jahr';


--
-- Name: COLUMN fauna_organism.name_de; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.name_de IS 'Deutscher Name';


--
-- Name: COLUMN fauna_organism.name_fr; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.name_fr IS 'Französischer Name';


--
-- Name: COLUMN fauna_organism.name_it; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.name_it IS 'Italienischer Name';


--
-- Name: COLUMN fauna_organism.name_ro; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.name_ro IS 'Romanischer Name';


--
-- Name: COLUMN fauna_organism.name_en; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.name_en IS 'Englischer Name';


--
-- Name: COLUMN fauna_organism.protection_ch; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN fauna_organism.protection_ch IS 'Schutz Schweiz';


--
-- Name: fauna_organism_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE fauna_organism_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.fauna_organism_id_seq OWNER TO swissmon;

--
-- Name: fauna_organism_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE fauna_organism_id_seq OWNED BY fauna_organism.id;


--
-- Name: flora_organism; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE flora_organism (
    id bigint NOT NULL,
    sisf_nr integer,
    status character varying(1) NOT NULL,
    name character varying(255) NOT NULL,
    "Familie" character varying(255),
    "Gattung" character varying(100),
    "Art" character varying(100),
    "Autor" character varying(100),
    "Rang" character varying(50),
    "NameUnterart" character varying(100),
    "AutorUnterart" character varying(50),
    "GueltigeNamen" character varying(255),
    official_flora_orfganism_id integer,
    name_de character varying(255),
    name_fr character varying(255),
    name_it character varying(255),
    name_reference character varying(50),
    is_neophyte boolean
);


ALTER TABLE public.flora_organism OWNER TO swissmon;

--
-- Name: COLUMN flora_organism.id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism.id IS 'PK';


--
-- Name: COLUMN flora_organism.sisf_nr; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism.sisf_nr IS 'SISF-Artcode';


--
-- Name: COLUMN flora_organism.name; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism.name IS 'VollstÃ¤ndiger Artname';


--
-- Name: COLUMN flora_organism."Rang"; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism."Rang" IS 'VarietÃ¤t oder Unterart';


--
-- Name: COLUMN flora_organism."GueltigeNamen"; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism."GueltigeNamen" IS 'Verweis auf andere gÃ¼ltige Namen dieser Art (sisf_nr)';


--
-- Name: COLUMN flora_organism.official_flora_orfganism_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism.official_flora_orfganism_id IS 'Eindeutiger Verweis auf die offizielle Art mit anerkanntem Namen: FK zu flora_orfganism.id';


--
-- Name: COLUMN flora_organism.name_de; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism.name_de IS 'Vorsicht: Seit SISF2 erster von mehreren mÃ¶glichen Namen';


--
-- Name: COLUMN flora_organism.name_fr; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism.name_fr IS 'Vorsicht: Mit SISF2 bisher nicht nachgefÃ¼hrt, d.h. Stand SISF1';


--
-- Name: COLUMN flora_organism.name_it; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism.name_it IS 'Vorsicht: Mit SISF2 bisher nicht nachgefÃ¼hrt, d.h. Stand SISF1';


--
-- Name: COLUMN flora_organism.name_reference; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism.name_reference IS 'Gibt an in welchem der 7 Referenzwerke der Name als akzeptiert verwendet wird';


--
-- Name: COLUMN flora_organism.is_neophyte; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_organism.is_neophyte IS 'Gebietsfremde Arten';


--
-- Name: flora_organism_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE flora_organism_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.flora_organism_id_seq OWNER TO swissmon;

--
-- Name: flora_organism_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE flora_organism_id_seq OWNED BY flora_organism.id;


--
-- Name: flora_red_list; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE flora_red_list (
    id bigint NOT NULL,
    flora_organism_id integer NOT NULL,
    red_list_nr integer,
    is_berne_convention boolean,
    is_iucn boolean,
    protection integer,
    neophyte_type integer,
    is_invasive boolean,
    diff_isfs integer,
    red_list_ch integer,
    red_list_adv_ju integer,
    red_list_ju_tot integer,
    red_list_ju1 integer,
    red_list_ju2 integer,
    red_list_adv_mp integer,
    red_list_mp_tot integer,
    red_list_mp1 integer,
    red_list_mp2 integer,
    red_list_adv_na integer,
    red_list_na_tot integer,
    red_list_na1 integer,
    red_list_na2 integer,
    red_list_adv_wa integer,
    red_list_wa_tot integer,
    red_list_adv_ea integer,
    red_list_ea_tot integer,
    red_list_adv_sa integer,
    red_list_sa_tot integer,
    red_list_sa1 integer,
    red_list_sa2 integer,
    red_list_sa3 integer,
    ecological_grp integer
);


ALTER TABLE public.flora_red_list OWNER TO swissmon;

--
-- Name: COLUMN flora_red_list.id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.id IS 'PK';


--
-- Name: COLUMN flora_red_list.flora_organism_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.flora_organism_id IS 'FK to flora_organism.id';


--
-- Name: COLUMN flora_red_list.red_list_nr; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_nr IS 'Nummer in der Roten Liste 2002';


--
-- Name: COLUMN flora_red_list.is_berne_convention; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.is_berne_convention IS 'Nach Berner Konvention zu schÃ¤tzen';


--
-- Name: COLUMN flora_red_list.is_iucn; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.is_iucn IS 'Nach der "IUCN list of threatened plants" weltweit gefÃ¤hrdet';


--
-- Name: COLUMN flora_red_list.protection; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.protection IS 'Schutzstatus';


--
-- Name: COLUMN flora_red_list.neophyte_type; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.neophyte_type IS 'Neophyten und Herkunft';


--
-- Name: COLUMN flora_red_list.is_invasive; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.is_invasive IS 'bekanntermassen oder potentiell invasiv';


--
-- Name: COLUMN flora_red_list.diff_isfs; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.diff_isfs IS 'Differenzen zur SISF-Nomenklatur';


--
-- Name: COLUMN flora_red_list.red_list_ch; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_ch IS 'Schweiz';


--
-- Name: COLUMN flora_red_list.red_list_adv_ju; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_adv_ju IS 'synantrope Erweiterung des ursprÃ¼nglichen Verbreitungsgebiets einheimischer Arten (Jura)';


--
-- Name: COLUMN flora_red_list.red_list_ju_tot; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_ju_tot IS 'Jura';


--
-- Name: COLUMN flora_red_list.red_list_ju1; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_ju1 IS 'westliches Jura';


--
-- Name: COLUMN flora_red_list.red_list_ju2; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_ju2 IS 'Schaffhauser Jura';


--
-- Name: COLUMN flora_red_list.red_list_adv_mp; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_adv_mp IS 'synantrope Erweiterung des ursprÃ¼nglichen Verbreitungsgebiets einheimischer Arten (Mittelland)';


--
-- Name: COLUMN flora_red_list.red_list_mp_tot; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_mp_tot IS 'Mittelland';


--
-- Name: COLUMN flora_red_list.red_list_mp1; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_mp1 IS 'westliches Mittelland';


--
-- Name: COLUMN flora_red_list.red_list_mp2; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_mp2 IS 'Ã–stliches Mittelland';


--
-- Name: COLUMN flora_red_list.red_list_adv_na; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_adv_na IS 'synantrope Erweiterung des ursprÃ¼nglichen Verbreitungsgebiets einheimischer Arten (Alpennordflanke)';


--
-- Name: COLUMN flora_red_list.red_list_na_tot; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_na_tot IS 'Alpennordflanke';


--
-- Name: COLUMN flora_red_list.red_list_na1; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_na1 IS 'westliche Alpennordflanke';


--
-- Name: COLUMN flora_red_list.red_list_na2; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_na2 IS 'Ã–stliche Alpennordflanke';


--
-- Name: COLUMN flora_red_list.red_list_adv_wa; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_adv_wa IS 'synantrope Erweiterung des ursprÃ¼nglichen Verbreitungsgebiets einheimischer Arten (Wallis)';


--
-- Name: COLUMN flora_red_list.red_list_wa_tot; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_wa_tot IS 'Wallis';


--
-- Name: COLUMN flora_red_list.red_list_adv_ea; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_adv_ea IS 'synantrope Erweiterung des ursprÃ¼nglichen Verbreitungsgebiets einheimischer Arten (Ã–stliche Zentralalpen)';


--
-- Name: COLUMN flora_red_list.red_list_ea_tot; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_ea_tot IS 'Ã–stliche Zentralalpen';


--
-- Name: COLUMN flora_red_list.red_list_adv_sa; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_adv_sa IS 'synantrope Erweiterung des ursprÃ¼nglichen Verbreitungsgebiets einheimischer Arten (AlpensÃ¼dflanke)';


--
-- Name: COLUMN flora_red_list.red_list_sa_tot; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_sa_tot IS 'AlpensÃ¼dflanke';


--
-- Name: COLUMN flora_red_list.red_list_sa1; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_sa1 IS 'Tessin';


--
-- Name: COLUMN flora_red_list.red_list_sa2; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_sa2 IS 'Bergell';


--
-- Name: COLUMN flora_red_list.red_list_sa3; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.red_list_sa3 IS 'MÃ¼nstertal';


--
-- Name: COLUMN flora_red_list.ecological_grp; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN flora_red_list.ecological_grp IS 'Ã–kologische Gruppe';


--
-- Name: flora_red_list_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE flora_red_list_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.flora_red_list_id_seq OWNER TO swissmon;

--
-- Name: flora_red_list_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE flora_red_list_id_seq OWNED BY flora_red_list.id;



--
-- Name: habitat; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE habitat (
    id bigint NOT NULL,
    "LrMethodId" integer NOT NULL,
    "ENr" integer NOT NULL,
    label character varying(10),
    name_de character varying(100) NOT NULL,
    name_lt character varying(100),
    name_en character varying(100),
    name_fr character varying(100),
    name_it character varying(100),
    notes text
);


ALTER TABLE public.habitat OWNER TO swissmon;

--
-- Name: COLUMN habitat.id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN habitat.id IS 'PK';


--
-- Name: COLUMN habitat."LrMethodId"; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN habitat."LrMethodId" IS 'FK to LrMethode';


--
-- Name: COLUMN habitat."ENr"; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN habitat."ENr" IS 'FK to tblLrBezÃ¼ge';


--
-- Name: COLUMN habitat.label; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN habitat.label IS 'Label inkl. Hierarchie';


--
-- Name: COLUMN habitat.name_de; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN habitat.name_de IS 'Name Deutsch';


--
-- Name: COLUMN habitat.name_lt; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN habitat.name_lt IS 'Name Lateinisch';


--
-- Name: COLUMN habitat.name_en; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN habitat.name_en IS 'Name Englisch';


--
-- Name: COLUMN habitat.name_fr; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN habitat.name_fr IS 'Name FranzÃ¶sisch';


--
-- Name: COLUMN habitat.name_it; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN habitat.name_it IS 'Name Italienisch';


--
-- Name: COLUMN habitat.notes; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN habitat.notes IS 'Bemerkungen zum Lebensraumtyp';


--
-- Name: habitat_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE habitat_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.habitat_id_seq OWNER TO swissmon;

--
-- Name: habitat_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE habitat_id_seq OWNED BY habitat.id;


--
-- Name: head_inventory; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE head_inventory (
    id bigint NOT NULL,
    area_id integer NOT NULL,
    name character varying(512)
);


ALTER TABLE public.head_inventory OWNER TO swissmon;

--
-- Name: COLUMN head_inventory.area_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN head_inventory.area_id IS 'Die der Area';


--
-- Name: COLUMN head_inventory.name; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN head_inventory.name IS 'Name des Inventars';


--
-- Name: head_inventory_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE head_inventory_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.head_inventory_id_seq OWNER TO swissmon;

--
-- Name: head_inventory_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE head_inventory_id_seq OWNED BY head_inventory.id;


--
-- Name: inventory; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE inventory (
    id bigint NOT NULL,
    inventory_type_id bigint NOT NULL,
    comment text DEFAULT ''::text NOT NULL,
    head_inventory_id integer NOT NULL
);


ALTER TABLE public.inventory OWNER TO swissmon;

--
-- Name: COLUMN inventory.inventory_type_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory.inventory_type_id IS 'Die Id des Inventartyps';


--
-- Name: COLUMN inventory.comment; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory.comment IS 'Kommentar';


--
-- Name: COLUMN inventory.head_inventory_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory.head_inventory_id IS 'Die Id des Hauptinventars';


--
-- Name: inventory_entry; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE inventory_entry (
    id bigint NOT NULL,
    organism_id bigint NOT NULL,
    inventory_id bigint NOT NULL
);


ALTER TABLE public.inventory_entry OWNER TO swissmon;

--
-- Name: COLUMN inventory_entry.organism_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_entry.organism_id IS 'Id des Organismus';


--
-- Name: COLUMN inventory_entry.inventory_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_entry.inventory_id IS 'Id des Inventars';


--
-- Name: inventory_entry_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE inventory_entry_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.inventory_entry_id_seq OWNER TO swissmon;

--
-- Name: inventory_entry_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE inventory_entry_id_seq OWNED BY inventory_entry.id;


--
-- Name: inventory_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE inventory_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.inventory_id_seq OWNER TO swissmon;

--
-- Name: inventory_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE inventory_id_seq OWNED BY inventory.id;


--
-- Name: inventory_type; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE inventory_type (
    id bigint NOT NULL,
    name character varying(50) NOT NULL
);


ALTER TABLE public.inventory_type OWNER TO swissmon;

--
-- Name: COLUMN inventory_type.name; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_type.name IS 'Name des Types';


--
-- Name: inventory_type_attribute; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE inventory_type_attribute (
    id bigint NOT NULL,
    inventory_type_id bigint NOT NULL,
    name character varying(45) NOT NULL,
    attribute_format_id bigint NOT NULL
);


ALTER TABLE public.inventory_type_attribute OWNER TO swissmon;

--
-- Name: COLUMN inventory_type_attribute.inventory_type_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_type_attribute.inventory_type_id IS 'Id des Inventar Types';


--
-- Name: COLUMN inventory_type_attribute.name; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_type_attribute.name IS 'Name des Attributes';


--
-- Name: COLUMN inventory_type_attribute.attribute_format_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_type_attribute.attribute_format_id IS 'Id des Attributformats';


--
-- Name: inventory_type_attribute_dropdown_value; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE inventory_type_attribute_dropdown_value (
    id bigint NOT NULL,
    inventory_type_attribute_id bigint NOT NULL,
    value character varying(45) NOT NULL
);


ALTER TABLE public.inventory_type_attribute_dropdown_value OWNER TO swissmon;

--
-- Name: COLUMN inventory_type_attribute_dropdown_value.inventory_type_attribute_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_type_attribute_dropdown_value.inventory_type_attribute_id IS 'Id des Inventartyp Attributes';


--
-- Name: COLUMN inventory_type_attribute_dropdown_value.value; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_type_attribute_dropdown_value.value IS 'Werte des Dropdowneintrages';


--
-- Name: inventory_type_attribute_dropdown_value_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE inventory_type_attribute_dropdown_value_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.inventory_type_attribute_dropdown_value_id_seq OWNER TO swissmon;

--
-- Name: inventory_type_attribute_dropdown_value_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE inventory_type_attribute_dropdown_value_id_seq OWNED BY inventory_type_attribute_dropdown_value.id;


--
-- Name: inventory_type_attribute_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE inventory_type_attribute_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.inventory_type_attribute_id_seq OWNER TO swissmon;

--
-- Name: inventory_type_attribute_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE inventory_type_attribute_id_seq OWNED BY inventory_type_attribute.id;


--
-- Name: inventory_type_attribute_inventory_entry; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE inventory_type_attribute_inventory_entry (
    id bigint NOT NULL,
    inventory_entry_id bigint NOT NULL,
    inventory_type_attribute_dropdown_value_id bigint,
    value text,
    inventory_type_attribute_id bigint NOT NULL
);


ALTER TABLE public.inventory_type_attribute_inventory_entry OWNER TO swissmon;

--
-- Name: COLUMN inventory_type_attribute_inventory_entry.inventory_entry_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_type_attribute_inventory_entry.inventory_entry_id IS 'Id des Inventareintrags';


--
-- Name: COLUMN inventory_type_attribute_inventory_entry.inventory_type_attribute_dropdown_value_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_type_attribute_inventory_entry.inventory_type_attribute_dropdown_value_id IS 'Falls es ein Dropdown ist, dann ist es die Id des Dropdownwertes';


--
-- Name: COLUMN inventory_type_attribute_inventory_entry.value; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_type_attribute_inventory_entry.value IS 'Der Inhalt des Attributes';


--
-- Name: COLUMN inventory_type_attribute_inventory_entry.inventory_type_attribute_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN inventory_type_attribute_inventory_entry.inventory_type_attribute_id IS 'Die Id des Attributs';


--
-- Name: inventory_type_attribute_inventory_entry_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE inventory_type_attribute_inventory_entry_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.inventory_type_attribute_inventory_entry_id_seq OWNER TO swissmon;

--
-- Name: inventory_type_attribute_inventory_entry_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE inventory_type_attribute_inventory_entry_id_seq OWNED BY inventory_type_attribute_inventory_entry.id;


--
-- Name: inventory_type_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE inventory_type_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.inventory_type_id_seq OWNER TO swissmon;

--
-- Name: inventory_type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE inventory_type_id_seq OWNED BY inventory_type.id;


--
-- Name: inventory_type_organism; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE inventory_type_organism (
    id bigint NOT NULL,
    organism_id bigint NOT NULL,
    inventory_type_id bigint NOT NULL
);


ALTER TABLE public.inventory_type_organism OWNER TO swissmon;

--
-- Name: inventory_type_organism_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE inventory_type_organism_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.inventory_type_organism_id_seq OWNER TO swissmon;

--
-- Name: inventory_type_organism_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE inventory_type_organism_id_seq OWNED BY inventory_type_organism.id;


--
-- Name: organism; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE organism (
    id bigint NOT NULL,
    organism_type bigint NOT NULL,
    organism_id bigint NOT NULL,
    inventory_type_id bigint
);


ALTER TABLE public.organism OWNER TO swissmon;

--
-- Name: COLUMN organism.organism_type; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN organism.organism_type IS 'Typ des Organismus: 1 = Fauna; 2 = Flora';


--
-- Name: COLUMN organism.organism_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN organism.organism_id IS 'Id in der Tabelle organism';


--
-- Name: COLUMN organism.inventory_type_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN organism.inventory_type_id IS 'Id des Inventartypes';


--
-- Name: organism_habitat; Type: TABLE; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE TABLE organism_habitat (
    id bigint NOT NULL,
    organism_id bigint NOT NULL,
    habitat_id integer NOT NULL,
    dependency_type integer NOT NULL,
    dependency_phase integer,
    dependency_function integer,
    dependency_value character varying(3),
    note character varying(255)
);


ALTER TABLE public.organism_habitat OWNER TO swissmon;

--
-- Name: COLUMN organism_habitat.organism_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN organism_habitat.organism_id IS 'FK to organism.id';


--
-- Name: COLUMN organism_habitat.habitat_id; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN organism_habitat.habitat_id IS 'FK to habitat.id';


--
-- Name: COLUMN organism_habitat.dependency_type; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN organism_habitat.dependency_type IS 'Art der Beziehung';


--
-- Name: COLUMN organism_habitat.dependency_phase; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN organism_habitat.dependency_phase IS 'Lebensphase, in der die Beziehung stattfindet';


--
-- Name: COLUMN organism_habitat.dependency_value; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN organism_habitat.dependency_value IS 'Wert fÃ¼r die Beziehung (z.B. Mass der Bindung an einen Lebensraum)';


--
-- Name: COLUMN organism_habitat.note; Type: COMMENT; Schema: public; Owner: swissmon
--

COMMENT ON COLUMN organism_habitat.note IS 'Bemerkungen';


--
-- Name: organism_habitat_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE organism_habitat_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.organism_habitat_id_seq OWNER TO swissmon;

--
-- Name: organism_habitat_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE organism_habitat_id_seq OWNED BY organism_habitat.id;


--
-- Name: organism_id_seq; Type: SEQUENCE; Schema: public; Owner: swissmon
--

CREATE SEQUENCE organism_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.organism_id_seq OWNER TO swissmon;

--
-- Name: organism_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: swissmon
--

ALTER SEQUENCE organism_id_seq OWNED BY organism.id;



--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE area ALTER COLUMN id SET DEFAULT nextval('area_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE area_habitat ALTER COLUMN id SET DEFAULT nextval('area_habitat_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE area_point ALTER COLUMN id SET DEFAULT nextval('area_point_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE area_type ALTER COLUMN id SET DEFAULT nextval('area_type_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE attribute_format ALTER COLUMN id SET DEFAULT nextval('attribute_format_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE fauna_class ALTER COLUMN id SET DEFAULT nextval('fauna_class_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE fauna_organism ALTER COLUMN id SET DEFAULT nextval('fauna_organism_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE flora_organism ALTER COLUMN id SET DEFAULT nextval('flora_organism_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE flora_red_list ALTER COLUMN id SET DEFAULT nextval('flora_red_list_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE habitat ALTER COLUMN id SET DEFAULT nextval('habitat_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE head_inventory ALTER COLUMN id SET DEFAULT nextval('head_inventory_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE inventory ALTER COLUMN id SET DEFAULT nextval('inventory_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE inventory_entry ALTER COLUMN id SET DEFAULT nextval('inventory_entry_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE inventory_type ALTER COLUMN id SET DEFAULT nextval('inventory_type_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE inventory_type_attribute ALTER COLUMN id SET DEFAULT nextval('inventory_type_attribute_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE inventory_type_attribute_dropdown_value ALTER COLUMN id SET DEFAULT nextval('inventory_type_attribute_dropdown_value_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE inventory_type_attribute_inventory_entry ALTER COLUMN id SET DEFAULT nextval('inventory_type_attribute_inventory_entry_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE inventory_type_organism ALTER COLUMN id SET DEFAULT nextval('inventory_type_organism_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE organism ALTER COLUMN id SET DEFAULT nextval('organism_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: swissmon
--

ALTER TABLE organism_habitat ALTER COLUMN id SET DEFAULT nextval('organism_habitat_id_seq'::regclass);


--
-- Name: area_habitat_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY area_habitat
    ADD CONSTRAINT area_habitat_pkey PRIMARY KEY (id);


--
-- Name: area_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY area
    ADD CONSTRAINT area_pkey PRIMARY KEY (id);


--
-- Name: area_point_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY area_point
    ADD CONSTRAINT area_point_pkey PRIMARY KEY (id);


--
-- Name: area_type_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY area_type
    ADD CONSTRAINT area_type_pkey PRIMARY KEY (id);


--
-- Name: attribute_format_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY attribute_format
    ADD CONSTRAINT attribute_format_pkey PRIMARY KEY (id);


--
-- Name: fauna_class_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY fauna_class
    ADD CONSTRAINT fauna_class_pkey PRIMARY KEY (id);


--
-- Name: fauna_organism_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY fauna_organism
    ADD CONSTRAINT fauna_organism_pkey PRIMARY KEY (id);


--
-- Name: flora_organism_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY flora_organism
    ADD CONSTRAINT flora_organism_pkey PRIMARY KEY (id);


--
-- Name: flora_red_list_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY flora_red_list
    ADD CONSTRAINT flora_red_list_pkey PRIMARY KEY (id);



--
-- Name: habitat_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY habitat
    ADD CONSTRAINT habitat_pkey PRIMARY KEY (id);


--
-- Name: head_inventory_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY head_inventory
    ADD CONSTRAINT head_inventory_pkey PRIMARY KEY (id);


--
-- Name: inventory_entry_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY inventory_entry
    ADD CONSTRAINT inventory_entry_pkey PRIMARY KEY (id);


--
-- Name: inventory_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY inventory
    ADD CONSTRAINT inventory_pkey PRIMARY KEY (id);


--
-- Name: inventory_type_attribute_dropdown_value_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY inventory_type_attribute_dropdown_value
    ADD CONSTRAINT inventory_type_attribute_dropdown_value_pkey PRIMARY KEY (id);


--
-- Name: inventory_type_attribute_inventory_entry_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY inventory_type_attribute_inventory_entry
    ADD CONSTRAINT inventory_type_attribute_inventory_entry_pkey PRIMARY KEY (id);


--
-- Name: inventory_type_attribute_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY inventory_type_attribute
    ADD CONSTRAINT inventory_type_attribute_pkey PRIMARY KEY (id);


--
-- Name: inventory_type_organism_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY inventory_type_organism
    ADD CONSTRAINT inventory_type_organism_pkey PRIMARY KEY (id);


--
-- Name: inventory_type_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY inventory_type
    ADD CONSTRAINT inventory_type_pkey PRIMARY KEY (id);


--
-- Name: organism_habitat_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY organism_habitat
    ADD CONSTRAINT organism_habitat_pkey PRIMARY KEY (id);


--
-- Name: organism_pkey; Type: CONSTRAINT; Schema: public; Owner: swissmon; Tablespace: 
--

ALTER TABLE ONLY organism
    ADD CONSTRAINT organism_pkey PRIMARY KEY (id);


--


--
-- Name: FK_inventory_11; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_11" ON inventory USING btree (inventory_type_id);


--
-- Name: FK_inventory_2; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_2" ON inventory USING btree (head_inventory_id);


--
-- Name: FK_inventory_entry_11; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_entry_11" ON inventory_entry USING btree (organism_id);


--
-- Name: FK_inventory_entry_21; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_entry_21" ON inventory_entry USING btree (inventory_id);


--
-- Name: FK_inventory_type_attribute_11; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_type_attribute_11" ON inventory_type_attribute USING btree (inventory_type_id);


--
-- Name: FK_inventory_type_attribute_21; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_type_attribute_21" ON inventory_type_attribute USING btree (attribute_format_id);


--
-- Name: FK_inventory_type_attribute_dropdown_value_11; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_type_attribute_dropdown_value_11" ON inventory_type_attribute_dropdown_value USING btree (inventory_type_attribute_id);


--
-- Name: FK_inventory_type_attribute_inventory_entry_11; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_type_attribute_inventory_entry_11" ON inventory_type_attribute_inventory_entry USING btree (inventory_type_attribute_id);


--
-- Name: FK_inventory_type_attribute_inventory_entry_21; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_type_attribute_inventory_entry_21" ON inventory_type_attribute_inventory_entry USING btree (inventory_entry_id);


--
-- Name: FK_inventory_type_attribute_inventory_entry_31; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_type_attribute_inventory_entry_31" ON inventory_type_attribute_inventory_entry USING btree (inventory_type_attribute_dropdown_value_id);


--
-- Name: FK_inventory_type_organism_11; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_type_organism_11" ON inventory_type_organism USING btree (organism_id);


--
-- Name: FK_inventory_type_organism_21; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_inventory_type_organism_21" ON inventory_type_organism USING btree (inventory_type_id);


--
-- Name: FK_organism_11; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_organism_11" ON organism USING btree (inventory_type_id);


--
-- Name: FK_organism_2; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX "FK_organism_2" ON organism USING btree (organism_id);


--
-- Name: area_id; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX area_id ON area_habitat USING btree (area_id);


--
-- Name: area_id1; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX area_id1 ON area_point USING btree (area_id, seq);


--
-- Name: area_id2; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX area_id2 ON head_inventory USING btree (area_id);


--
-- Name: area_points_ibfk_1; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX area_points_ibfk_1 ON area_point USING btree (area_id);


--
-- Name: flora_organism_id; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX flora_organism_id ON flora_red_list USING btree (flora_organism_id);


--
-- Name: flora_organism_id_2; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX flora_organism_id_2 ON flora_red_list USING btree (flora_organism_id);


--
-- Name: habitat_id; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX habitat_id ON area_habitat USING btree (habitat_id);


--
-- Name: habitat_id1; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX habitat_id1 ON organism_habitat USING btree (habitat_id);


--
-- Name: official_flora_orfganism_id; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX official_flora_orfganism_id ON flora_organism USING btree (official_flora_orfganism_id);


--
-- Name: organism_habitat_habitat_id_idx; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX organism_habitat_habitat_id_idx ON organism_habitat USING btree (habitat_id);


--
-- Name: organism_habitat_organism_id_idx; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX organism_habitat_organism_id_idx ON organism_habitat USING btree (organism_id);


--
-- Name: organism_id; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX organism_id ON organism_habitat USING btree (organism_id);


--
-- Name: status; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX status ON flora_organism USING btree (status);


--
-- Name: type_id; Type: INDEX; Schema: public; Owner: swissmon; Tablespace: 
--

CREATE INDEX type_id ON area USING btree (type_id);


--
-- Name: FK_inventory_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory
    ADD CONSTRAINT "FK_inventory_1" FOREIGN KEY (inventory_type_id) REFERENCES inventory_type(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: FK_inventory_entry_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory_entry
    ADD CONSTRAINT "FK_inventory_entry_1" FOREIGN KEY (organism_id) REFERENCES organism(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: FK_inventory_entry_2; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory_entry
    ADD CONSTRAINT "FK_inventory_entry_2" FOREIGN KEY (inventory_id) REFERENCES inventory(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: FK_inventory_type_attribute_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory_type_attribute
    ADD CONSTRAINT "FK_inventory_type_attribute_1" FOREIGN KEY (inventory_type_id) REFERENCES inventory_type(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: FK_inventory_type_attribute_2; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory_type_attribute
    ADD CONSTRAINT "FK_inventory_type_attribute_2" FOREIGN KEY (attribute_format_id) REFERENCES attribute_format(id) MATCH FULL;


--
-- Name: FK_inventory_type_attribute_dropdown_value_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory_type_attribute_dropdown_value
    ADD CONSTRAINT "FK_inventory_type_attribute_dropdown_value_1" FOREIGN KEY (inventory_type_attribute_id) REFERENCES inventory_type_attribute(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: FK_inventory_type_attribute_inventory_entry_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory_type_attribute_inventory_entry
    ADD CONSTRAINT "FK_inventory_type_attribute_inventory_entry_1" FOREIGN KEY (inventory_type_attribute_id) REFERENCES inventory_type_attribute(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: FK_inventory_type_attribute_inventory_entry_2; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory_type_attribute_inventory_entry
    ADD CONSTRAINT "FK_inventory_type_attribute_inventory_entry_2" FOREIGN KEY (inventory_entry_id) REFERENCES inventory_entry(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: FK_inventory_type_attribute_inventory_entry_3; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory_type_attribute_inventory_entry
    ADD CONSTRAINT "FK_inventory_type_attribute_inventory_entry_3" FOREIGN KEY (inventory_type_attribute_dropdown_value_id) REFERENCES inventory_type_attribute_dropdown_value(id) MATCH FULL ON DELETE SET NULL;


--
-- Name: FK_inventory_type_organism_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory_type_organism
    ADD CONSTRAINT "FK_inventory_type_organism_1" FOREIGN KEY (organism_id) REFERENCES organism(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: FK_inventory_type_organism_2; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory_type_organism
    ADD CONSTRAINT "FK_inventory_type_organism_2" FOREIGN KEY (inventory_type_id) REFERENCES inventory_type(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: FK_organism_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY organism
    ADD CONSTRAINT "FK_organism_1" FOREIGN KEY (inventory_type_id) REFERENCES inventory_type(id) MATCH FULL;


--
-- Name: FK_organism_habitat_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY organism_habitat
    ADD CONSTRAINT "FK_organism_habitat_1" FOREIGN KEY (habitat_id) REFERENCES habitat(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: FK_organism_habitat_2; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY organism_habitat
    ADD CONSTRAINT "FK_organism_habitat_2" FOREIGN KEY (organism_id) REFERENCES organism(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: area_habitat_ibfk_2; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY area_habitat
    ADD CONSTRAINT area_habitat_ibfk_2 FOREIGN KEY (habitat_id) REFERENCES habitat(id) MATCH FULL;


--
-- Name: area_point_ibfk_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY area_point
    ADD CONSTRAINT area_point_ibfk_1 FOREIGN KEY (area_id) REFERENCES area(id) MATCH FULL ON DELETE CASCADE;


--
-- Name: fk_fauna_organism_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY fauna_organism
    ADD CONSTRAINT fk_fauna_organism_1 FOREIGN KEY (fauna_class_id) REFERENCES fauna_class(id);


--
-- Name: flora_organism_ibfk_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY flora_organism
    ADD CONSTRAINT flora_organism_ibfk_1 FOREIGN KEY (official_flora_orfganism_id) REFERENCES flora_organism(id) MATCH FULL;


--
-- Name: flora_red_list_ibfk_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY flora_red_list
    ADD CONSTRAINT flora_red_list_ibfk_1 FOREIGN KEY (flora_organism_id) REFERENCES flora_organism(id) MATCH FULL;


--
-- Name: head_inventory_ibfk_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY head_inventory
    ADD CONSTRAINT head_inventory_ibfk_1 FOREIGN KEY (area_id) REFERENCES area(id) MATCH FULL;


--
-- Name: inventory_ibfk_1; Type: FK CONSTRAINT; Schema: public; Owner: swissmon
--

ALTER TABLE ONLY inventory
    ADD CONSTRAINT inventory_ibfk_1 FOREIGN KEY (head_inventory_id) REFERENCES head_inventory(id) MATCH FULL;


--
-- Name: public; Type: ACL; Schema: -; Owner: swissmon
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM swissmon;
GRANT ALL ON SCHEMA public TO swissmon;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- Swissmon database dump complete
--

