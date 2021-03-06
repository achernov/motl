<?php
//=====================

$compVita = array(
	array( "info", null,        "Vita2012" ),
	
	/*-- I отделение --*/
	
	array( "separator", "B",        "1 отделение" ),
	
	array( "separator", "C",        "регистрация 7:30, начало 9:00" ),
	
	array( "couple",    "e._9.st",       "Е класс 6-9, St - м.вальс, танго, квикстеп" ),
	array( "couple",    "e.10_11.st",    "Е класс 10-11, St" ),
	array( "couple",    "e.12_13.st",    "Е класс 12-13, St" ),
	array( "couple",    "ed.14_15.st",   "D+Е класс 14-15, St - м.вальс, танго, м.фокстрот, квикстеп" ),

	array( "separator", "C",        "регистрация 9:00, начало 10:30" ),
	
	array( "couple",    "e._9.la",       "Е класс 6-9, La - ча-ча-ча, румба, джайв" ),
	array( "couple",    "e.10_11.la",    "Е класс 10-11, La" ),
	array( "couple",    "e.12_13.la",    "Е класс 12-13, La" ),
	array( "couple",    "ed.14_15.la",   "D+Е класс 14-15, La - ча-ча-ча, самба, румба, джайв" ),
	
	array( "separator", "C",        "регистрация 10:30, начало 12:00" ),

	array( "couple",    "d.10_11 .st",   "D класс 10-11, St - м.вальс, танго, м.фокстрот, квикстеп" ),
	array( "couple",    "d.12_13.st",    "D класс 12-13, St" ),
	array( "couple",    "c.12_15.st",    "C класс 12-15, La" ),
	array( "couple",    "ba.16_35.la",   "В+А класс 16-35, La" ),
	
	array( "separator", "C",        "регистрация 11:30, начало 13:00" ),
	
	array( "couple",    "d.10_11 .la",   "D класс 10-11, La - ча-ча-ча, самба, румба, джайв" ),
	array( "couple",    "d.12_13.la",    "D класс 12-13, La" ),
	array( "couple",    "c.12_15.la",    "C класс 12-15, St" ),
	array( "couple",    "ba.16_35.st",   "В+А класс 16-35, St" ),

	/*-- II отделение --*/
	
	array( "separator", "B",        "2 отделение" ),
	
	array( "separator", "C",        "регистрация 14:30, начало 16:00" ),
	
	array( "couple",    "juv2.st",       "Ювеналы-2 + Ювеналы-1 (11 и мл.), St - м.вальс, танго, в.вальс, м.фокстрот, квикстеп" ),
	array( "couple",    "jun1.st",       "Юниоры–1 (12-13), St" ),
	array( "couple",    "jun2.st",       "Юниоры–2 (14-15), St" ),
	array( "couple",    "you1.la",       "Молодёжь-1 (16-19), La" ),
	array( "couple",    "you2.la",       "Молодёжь-2 (19-20), La" ),
	array( "couple",    "am.st",         "Взрослые + Молодёжь-1 (19-35), St" ),

	array( "separator", "C",        "регистрация 17:30, начало 19:00" ),
	
	array( "couple",    "juv2.la",       "Ювеналы-2 + Ювеналы-1 (11 и мл.), La - ча-ча-ча, самба, румба, пасодобль, джайв" ),
	array( "couple",    "jun1.la",       "Юниоры–1 (12-13), La" ),
	array( "couple",    "jun2.la",       "Юниоры–2 (14-15), La" ),
	array( "couple",    "you1.st",       "Молодёжь-1 (16-19), St" ),
	array( "couple",    "you2.st",       "Молодёжь-2 (19-20), St" ),
	array( "couple",    "am.la",         "Взрослые + Молодёжь-1 (19-35), La" ),
);
	
//=====================

$compCurrent = array(

	array( "info", null,        "Current template" ),
	
	/*-- I отделение --*/

	array( "separator", "B",        "1 отделение" ),
	array( "separator", "C",        "регистрация 10:00, начало 11:30" ),

	array( "solo",      "sn3._7",         "соло N3 класс 7 и мл. - м.вальс, полька, ча-ча-ча" ),
	array( "solo",      "sn3.8_9",        "соло N3 класс 8-9" ),
	array( "solo",      "sn3.10_11",      "соло N3 класс 10-11" ),
	array( "solo",      "sn3.12_13",      "соло N3 класс 12-13" ),
	array( "solo",      "sn3.14_",      "соло N3 класс 14 и ст." ),

	array( "solo",      "sn4._7",         "соло N4 класс 7 и мл. - м.вальс, квикстеп, ча-ча-ча, джайв" ),
	array( "solo",      "sn4.8_9",        "соло N4 класс 8-9" ),
	array( "solo",      "sn4.10_11",      "соло N4 класс 10-11" ),
	array( "solo",      "sn4.12_13",      "соло N4 класс 12-13" ),
	array( "solo",      "sn4.14_",      "соло N4 класс 14 и ст." ),

	array( "solo",      "sn5._7",         "соло N5 класс 7 и мл. - м.вальс, квикстеп, ча-ча-ча, румба, джайв" ),
	array( "solo",      "sn5.8_9",        "соло N5 класс 8-9" ),
	array( "solo",      "sn5.10_11",      "соло N5 класс 10-11" ),
	array( "solo",      "sn5.12_13",      "соло N5 класс 12-13" ),
	array( "solo",      "sn5.14_",      "соло N5 класс 14 и ст." ),

	array( "separator", "C",        "дорегистрация 11:00, начало 12:30" ),

	array( "solo",      "sc_p._13",      "Открытый кубок среди солистов по отдельным танцам - Полька" ),
	array( "solo",      "sc_w._13",      "Открытый кубок среди солистов по отдельным танцам - Медленный вальс" ),
	array( "solo",      "sc_q._13",      "Открытый кубок среди солистов по отдельным танцам - Квикстеп" ),
	array( "solo",      "sc_c._13",      "Открытый кубок среди солистов по отдельным танцам - Ча-ча-ча" ),
	array( "solo",      "sc_r._13",      "Открытый кубок среди солистов по отдельным танцам - Румба" ),
	array( "solo",      "sc_j._13",      "Открытый кубок среди солистов по отдельным танцам - Джайв" ),
	
	array( "separator", "C",        "дорегистрация 12:30, начало 14:00" ),

	array( "couple",    "n3._7",          "N3 класс 7	 - м.вальс, полька, ча-ча-ча" ),
	array( "couple",    "n3.8_9",         "N3 класс 8-9" ),
	array( "couple",    "n3.10_11",       "N3 класс 10-11" ),
	array( "couple",    "n3.12_13",       "N3 класс 12-13" ),
	array( "couple",    "n3.14_15",       "N3 класс 14-15" ),

	array( "couple",    "n4._7",          "N4 класс 7 и мл. - м.вальс, квикстеп, ча-ча-ча, джайв" ),
	array( "couple",    "n4.8_9",         "N4 класс 8-9" ),
	array( "couple",    "n4.10_11",       "N4 класс 10-11" ),
	array( "couple",    "n4.12_13",       "N4 класс 12-13" ),
	array( "couple",    "n4.14_15",       "N4 класс 14-15" ),

	array( "couple",    "n5._7",          "N5 класс 7 и мл. - м.вальс, квикстеп, ча-ча-ча, румба, джайв" ),
	array( "couple",    "n5.8_9",         "N5 класс 8-9" ),
	array( "couple",    "n5.10_11",       "N5 класс 10-11" ),
	array( "couple",    "n5.12_13",       "N5 класс 12-13" ),
	array( "couple",    "n5.14_15",       "N5 класс 14-15" ),

	array( "separator", "C",        "дорегистрация 13:30, начало 15:00" ),

	array( "couple",    "e._9",           "E класс 9 и мл. - м.вальс, в.вальс, квикстеп, ча-ча-ча, румба, джайв" ),
	array( "couple",    "e.10_11",        "E класс 10-11" ),
	array( "couple",    "e.12_13",        "E класс 12-13" ),
	array( "couple",    "e.14_15",        "E класс 14-15" ),
	array( "couple",    "juv1",         "Ювеналы 1 (9 лет и младше) - 6 танцев: м.вальс, вен. вальс, квикстеп, ча-ча-ча, румба, джайв" ),

	/*-- II отделение --*/

	array( "separator", "B",        "2 отделение" ),

	array( "separator", "C",        "регистрация 15:00, начало 16:30" ),
	
	array( "couple",    "d.10_11",        "D класс 10-11 - м.вальс, танго, в.вальс, квикстеп, ча-ча-ча, самба, румба, джайв" ),
	array( "couple",    "d.12_13",        "D класс 12-13" ),
	array( "couple",    "d.14_15",        "D класс 14-15" ),
	array( "couple",    "n.16_.st",       "N класс 16 и ст., St - м.вальс, квикстеп" ),
	array( "couple",    "e.16_.st",       "E класс 16 и ст., St - м.вальс, в.вальс, квикстеп" ),
	array( "couple",    "d.16_.st",       "D класс 16 и ст., St - м.вальс, танго, в.вальс, квикстеп" ),
	array( "couple",    "c.12_13.st",     "C класс 12-13, St" ),
	array( "couple",    "c.14_15.st",     "C класс 14-15, St" ),
	array( "couple",    "c.16_.st",       "C класс 16 и ст., St" ),
	array( "couple",    "ba.12_13.st",    "B+A класс 12-13, St" ),
	array( "couple",    "ba.14_15.st",    "B+A класс 14-15, St" ),
	array( "couple",    "ba.16_.st",      "B+A класс 16 и ст., St" ),
	array( "couple",    "jun1.st",        "Юниоры 1 (12-13 лет) St" ),
	array( "couple",    "jun2.st",        "Юниоры 2 (14-15 лет) St" ),

	
	array( "separator", "C",        "дорегистрация 16:00, начало 17:30" ),
	
	array( "couple",    "juv2",         "Ювеналы 2 (10-11 лет) - 8 танцев: м.вальс, танго, вен. вальс, квикстеп, ча-ча-ча, самба, румба, джайв" ),
	array( "couple",    "n.16_.la",       "N класс 16 и ст., La - ча-ча-ча, джайв" ),
	array( "couple",    "e.16_.la",       "E класс 16 и ст., La - ча-ча-ча, румба, джайв" ),
	array( "couple",    "d.16_.la",       "D класс 16 и ст., La - ча-ча-ча, самба, румба, джайв" ),
	array( "couple",    "c.12_13.la",     "C класс 12-13, La" ),
	array( "couple",    "c.14_15.la",     "C класс 14-15, La" ),
	array( "couple",    "c.16_.la",       "C класс 16 и ст., La" ),
	array( "couple",    "ba.12_13.la",    "B+A класс 12-13, La" ),
	array( "couple",    "ba.14_15.la",    "B+A класс 14-15, La" ),
	array( "couple",    "ba.16_.la",      "B+A класс 16 и ст., La" ),
	array( "couple",    "jun1.la",        "Юниоры 1 (12-13 лет) La" ),
	array( "couple",    "jun2.la",        "Юниоры 2 (14-15 лет) La" ),
);

//=====================

$compEkat = array(

	array( "info", null,        "Екатерининский" ),
	
	/*-- I отделение --*/

	array( "separator", "B",        "1 отделение" ),

	array( "separator", "C",        "регистрация 8:00, начало 9:30" ),

	array( "solo",      "sn3._7",         "соло N3 класс 7 и мл. - м.вальс, полька, ча-ча-ча" ),
	array( "solo",      "sn3.8_9",        "соло N3 класс 8-9" ),
	array( "solo",      "sn3.10_11",      "соло N3 класс 10-11" ),
	array( "solo",      "sn3.12_13",      "соло N3 класс 12-13" ),

	array( "solo",      "sn4._7",         "соло N4 класс 7 и мл. - м.вальс, квикстеп, ча-ча-ча, джайв" ),
	array( "solo",      "sn4.8_9",        "соло N4 класс 8-9" ),
	array( "solo",      "sn4.10_11",      "соло N4 класс 10-11" ),
	array( "solo",      "sn4.12_13",      "соло N4 класс 12-13" ),

	array( "solo",      "sn5._7",         "соло N5 класс 7 и мл. - м.вальс, квикстеп, ча-ча-ча, румба, джайв" ),
	array( "solo",      "sn5.8_9",        "соло N5 класс 8-9" ),
	array( "solo",      "sn5.10_11",      "соло N5 класс 10-11" ),
	array( "solo",      "sn5.12_13",      "соло N5 класс 12-13" ),

	array( "separator", "C",        "дорегистрация 10:00, начало 11:30" ),

	array( "solo",      "sc_p._13",      "Открытый кубок среди солистов по отдельным танцам 13 лет и мл. - Полька" ),
	array( "solo",      "sc_w._13",      "Открытый кубок среди солистов по отдельным танцам 13 лет и мл. - Медленный вальс" ),
	array( "solo",      "sc_q._13",      "Открытый кубок среди солистов по отдельным танцам 13 лет и мл. - Квикстеп" ),
	array( "solo",      "sc_c._13",      "Открытый кубок среди солистов по отдельным танцам 13 лет и мл. - Ча-ча-ча" ),
	array( "solo",      "sc_j._13",      "Открытый кубок среди солистов по отдельным танцам 13 лет и мл. - Джайв" ),

	array( "separator", "C",        "дорегистрация 11:00, начало 13:30" ),

	array( "couple",    "n3._7",          "N3 класс 7 и мл. - м.вальс, полька, ча-ча-ча" ),
	array( "couple",    "n3.8_9",         "N3 класс 8-9" ),
	array( "couple",    "n3.10_11",       "N3 класс 10-11" ),
	array( "couple",    "n3.12_13",       "N3 класс 12-13" ),

	array( "couple",    "n4._7",          "N4 класс 7 и мл. - м.вальс, квикстеп, ча-ча-ча, джайв" ),
	array( "couple",    "n4.8_9",         "N4 класс 8-9" ),
	array( "couple",    "n4.10_11",       "N4 класс 10-11" ),
	array( "couple",    "n4.12_13",       "N4 класс 12-13" ),

	array( "couple",    "n5._7",          "N5 класс 7 и мл. - м.вальс, квикстеп, ча-ча-ча, румба, джайв" ),
	array( "couple",    "n5.8_9",         "N5 класс 8-9" ),
	array( "couple",    "n5.10_11",       "N5 класс 10-11" ),
	array( "couple",    "n5.12_13",       "N5 класс 12-13" ),

	array( "separator", "C",        "дорегистрация 13:00, начало 14:30" ),

	array( "couple",    "e._9",           "E класс 9 и мл. - м.вальс, в.вальс, квикстеп, ча-ча-ча, румба, джайв" ),
	array( "couple",    "e.10_11",        "E класс 10-11" ),
	array( "couple",    "e.12_13",        "E класс 12-13" ),
	array( "couple",    "e.14_15",        "E класс 14-15" ),

	/*-- II отделение --*/

	array( "separator", "B",        "2 отделение" ),

	array( "separator", "C",        "регистрация 15:00, начало 16:30" ),

	array( "couple",    "juv1",         "Ювеналы 1 (9 лет и младше) - 6 танцев: м.вальс, вен. вальс, квикстеп, ча-ча-ча, румба, джайв" ),
	
	array( "couple",    "d.10_11",        "D класс 10-11 - м.вальс, танго, в.вальс, квикстеп, ча-ча-ча, самба, румба, джайв" ),
	array( "couple",    "d.12_13",        "D класс 12-13" ),

	array( "couple",    "n.16_.st",       "N класс 16 и ст., St - м.вальс, квикстеп" ),
	array( "couple",    "e.16_.st",       "E класс 16 и ст., St - м.вальс, в.вальс, квикстеп" ),
	array( "couple",    "d.16_.st",       "D класс 16 и ст., St - м.вальс, танго, в.вальс, квикстеп" ),
	array( "couple",    "c.12_13.st",     "C класс 12-13, St" ),
	array( "couple",    "c.14_15.st",     "C класс 14-15, St" ),
	array( "couple",    "c.16_.st",       "C класс 16 и ст., St" ),
	array( "couple",    "ba.12_13.st",    "B+A класс 12-13, St" ),
	array( "couple",    "ba.14_15.st",    "B+A класс 14-15, St" ),
	array( "couple",    "ba.16_.st",      "B+A класс 16 и ст., St" ),

	array( "couple",    "jun1.st",        "Юниоры 1 (12-13 лет) St" ),
	array( "couple",    "jun2.st",        "Юниоры 2 (14-15 лет) St" ),
	
	array( "separator", "C",        "дорегистрация 15:00, начало 16:30 (La)" ),
	
	array( "couple",    "juv2",         "Ювеналы 2 (10-11 лет) - 8 танцев: м.вальс, танго, вен. вальс, квикстеп, ча-ча-ча, самба, румба, джайв" ),

	array( "couple",    "d.14_15",        "D класс 14-15" ),

	array( "couple",    "n.16_.la",       "N класс 16 и ст., La - ча-ча-ча, джайв" ),
	array( "couple",    "e.16_.la",       "E класс 16 и ст., La - ча-ча-ча, румба, джайв" ),
	array( "couple",    "d.16_.la",       "D класс 16 и ст., La - ча-ча-ча, самба, румба, джайв" ),
	array( "couple",    "c.12_13.la",     "C класс 12-13, La" ),
	array( "couple",    "c.14_15.la",     "C класс 14-15, La" ),
	array( "couple",    "c.16_.la",       "C класс 16 и ст., La" ),

	array( "couple",    "ba.12_13.la",    "B+A класс 12-13, La" ),
	array( "couple",    "ba.14_15.la",    "B+A класс 14-15, La" ),
	array( "couple",    "ba.16_.la",      "B+A класс 16 и ст., La" ),

	array( "couple",    "jun1.la",        "Юниоры 1 (12-13 лет) La" ),
	array( "couple",    "jun2.la",        "Юниоры 2 (14-15 лет) La" ),
);

//=====================

$compImperia = array(
	
	array( "info", null,        "Imperia" ),
	
	/*-- I отделение --*/

	array( "separator", "A",        "17 ноября" ), //-------------------
	
	array( "separator", "C",        "регистрация 14:00, начало 16:30" ),
	
	array( "couple",    "juv2.st",   "Ювеналы 2 St" ),
	array( "couple",    "jun1.st",   "Юниоры 1 St" ),
	array( "couple",    "jun2.st",   "Юниоры 2 St" ),

	array( "separator", "C",        "регистрация 17:00, начало 19:30" ),
	
	array( "couple",    "you1.la",   "Молодежь 1 La" ),
	array( "couple",    "you2.la",   "Молодежь 2 La" ),
	array( "couple",    "am.st",     "Взрослые St" ),
	
	array( "separator", "A",        "18 ноября" ), //-------------------

	array( "separator", "C",        "регистрация 14:00, начало 16:30" ),

	array( "couple",    "juv2.la",   "Ювеналы 2 La" ),
	array( "couple",    "jun1.la",   "Юниоры 1 La" ),
	array( "couple",    "jun2.la",   "Юниоры 2 La" ),
	
	array( "separator", "C",        "регистрация 17:00, начало 19:30" ),

	array( "couple",    "you1.st",   "Молодежь 1 St" ),
	array( "couple",    "you2.st",   "Молодежь 2 St" ),
	array( "couple",    "am.la",     "Взрослые La" ),

	//array( "couple",    "juv1.st",   "Ювеналы 1 St" ),
	//array( "couple",    "juv1.la",   "Ювеналы 1 La" ),
);

//=====================
?>
