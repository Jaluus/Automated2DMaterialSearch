# Creation Query (MySQL) (Simple)

        CREATE TABLE scan
        (
            id   BIGINT UNSIGNED AUTO_INCREMENT UNIQUE NOT NULL PRIMARY KEY ,
            time BIGINT NOT NULL
        ) ENGINE=INNODB;

        CREATE TABLE chip
        (
            id       BIGINT UNSIGNED AUTO_INCREMENT UNIQUE NOT NULL PRIMARY KEY ,
            scan_id  BIGINT UNSIGNED NOT NULL,
            material TEXT NOT NULL ,
            FOREIGN KEY (scan_id) REFERENCES scan(id)
        ) ENGINE=INNODB;

        CREATE TABLE flake
        (
            id        BIGINT UNSIGNED AUTO_INCREMENT UNIQUE NOT NULL PRIMARY KEY ,
            chip_id   BIGINT UNSIGNED NOT NULL,
            size      DOUBLE NOT NULL ,
            thickness INT NOT NULL ,
            used      TINYINT(1) DEFAULT 0 NOT NULL ,
            FOREIGN KEY (chip_id) REFERENCES chip(id)
        ) ENGINE=INNODB;

        CREATE TABLE image
        (
            id      BIGINT UNSIGNED AUTO_INCREMENT UNIQUE NOT NULL PRIMARY KEY ,
            flake_id   BIGINT UNSIGNED NOT NULL,
            path    TEXT NOT NULL,
            FOREIGN KEY (flake_id) REFERENCES flake(id)
        ) ENGINE=INNODB;

# Creation Query (MySQL) (FULL)




# Problems

You __*ABSOLUTLY*__ need to specify `ENGINE = INNODB` as the default storage engine is `MyISAM`. That engine does __*NOT*__ support foreign keys, and you will have a bad time debugging the code without this knowledge (I had fun searching for 2 hours). To get the current default engine type `SELECT @@default_storage_engine;`