
/* Drop Tables */

DROP TABLE IF EXISTS CUSTOM_ACL;
DROP TABLE IF EXISTS CUSTOM_ACL_ITEMS;




/* Create Tables */

CREATE TABLE CUSTOM_ACL
(
	USER_ID INT,
	GROUP_ID INT,
	LEVEL SMALLINT,
	ACL_ID INT NOT NULL UNIQUE
) WITHOUT OIDS;


CREATE TABLE CUSTOM_ACL_ITEMS
(
	ID SERIAL NOT NULL UNIQUE,
	USER_ID SERIAL,
	DESCRIPTION TEXT,
	PRIMARY KEY (ID)
) WITHOUT OIDS;



/* Create Foreign Keys */

ALTER TABLE CUSTOM_ACL
	ADD FOREIGN KEY (ACL_ID)
	REFERENCES CUSTOM_ACL_ITEMS (ID)
	ON UPDATE RESTRICT
	ON DELETE RESTRICT
;



