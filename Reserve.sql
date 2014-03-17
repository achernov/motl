INSERT INTO db_person(id,ts_created) SELECT max(id)+100,'1980-01-01' FROM db_person;
DELETE FROM db_person WHERE ts_created='1980-01-01';
INSERT INTO db_couples(id,ts_created) SELECT max(id)+100,'1980-01-01' FROM db_couples;
DELETE FROM db_couples WHERE ts_created='1980-01-01';
INSERT INTO db_couple_event(id,ts_created) SELECT max(id)+300,'1980-01-01' FROM db_couple_event;
DELETE FROM db_couple_event WHERE ts_created='1980-01-01';
