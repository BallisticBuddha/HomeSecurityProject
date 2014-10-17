--This will set up the tables for the arduino security system project

--Creates a domain for a custom data type that will inherit the behavior
--from a different data type which one or more contraints can be added to.
--CREATE DOMAIN privilegeLevel AS TEXT
--CONSTRAINT valid_privilegeLevel CHECK (
	--VALUE IN ( 'Admin', 'Basic-User', 'Guest' )
--);

--CREATE DOMAIN eventType AS TEXT
--CONSTRAINT valid_eventType CHECK (
	--VALUE IN ( 'Disarm', 'Alarm', 'Alarming', 'IncorrectPin', 'CorrectPin' )
--);

--Creates an ENUM for both eventTYpe and privilegeLevel
CREATE TYPE privilegeLevel AS ENUM ('Admin', 'BasicUser', 'Guest');
CREATE TYPE eventType AS ENUM ('Disarm', 'Arm', 'Alarming', 'IncorrectPin', 'CorrectPin');


CREATE TABLE users (
	id varchar(10) NOT NULL,
	userName varchar(15) NOT NULL,
	firstName varchar(75) NOT NULL,
	lastName varchar(75) NOT NULL,
	email varchar(75) NOT NULL,
	phoneNumber varchar(15) NOT NULL,
	privilegeLevel privilegeLevel NOT NULL,
	PRIMARY KEY (id)
);

CREATE TABLE events (
	id varchar(10) NOT NULL,
	eventType varchar(20) NOT NULL,
	dateCreated timestamp NOT NULL,
	userID varchar(10),
	sensorTriggered varchar(10),
	picture bytea,
	PRIMARY KEY (id)
);

CREATE TABLE sensors (
	id varchar(10) NOT NULL,
	sensorName varchar(50) NOT NULL,
	description TEXT,
	PRIMARY KEY (id)
);
	
	

