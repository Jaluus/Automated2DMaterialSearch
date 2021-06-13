# Creation Query (MySQL) (simple)

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
            chip_id BIGINT UNSIGNED NOT NULL,
            path    TEXT NOT NULL,
            FOREIGN KEY (chip_id) REFERENCES chip(id)
        ) ENGINE=INNODB;

        CREATE TABLE relation_table
        (
            image_id BIGINT UNSIGNED NOT NULL ,
            flake_id BIGINT UNSIGNED NOT NULL ,
            FOREIGN KEY (flake_id) REFERENCES flake(id),
            FOREIGN KEY (image_id) REFERENCES image(id)
        ) ENGINE=INNODB;

# Creation Query (MySQL) (Full)

        create table scan
        (
            id   bigint unsigned auto_increment,
            time bigint                     not null,
            user varchar(50) default 'None' not null,
            constraint id
                unique (id)
        )
            engine = InnoDB;

        alter table scan
            add primary key (id);

        create table chip
        (
            id                 bigint unsigned auto_increment,
            scan_id            bigint unsigned            not null,
            exfoliate_material varchar(50) default 'WSe2' not null,
            chip_thickness     int         default 90     not null,
            constraint id
                unique (id),
            constraint chip_ibfk_1
                foreign key (scan_id) references scan (id)
        )
            engine = InnoDB;

        create index scan_id
            on chip (scan_id);

        alter table chip
            add primary key (id);

        create table flake
        (
            id        bigint unsigned auto_increment,
            chip_id   bigint unsigned      not null,
            size      double               not null,
            thickness int                  not null,
            used      tinyint(1) default 0 not null,
            local_x   double     default 0 not null,
            local_y   double     default 0 not null,
            constraint id
                unique (id),
            constraint flake_ibfk_1
                foreign key (chip_id) references chip (id)
        )
            engine = InnoDB;

        create index chip_id
            on flake (chip_id);

        alter table flake
            add primary key (id);

        create table image
        (
            id              bigint unsigned auto_increment,
            chip_id         bigint unsigned not null,
            path            text            not null,
            aparture        double          not null,
            light_voltage   double          not null,
            magnification   double          not null,
            global_x        double          not null,
            global_y        double          not null,
            white_balance_r int             not null,
            white_balance_g int             not null,
            white_balance_b int             not null,
            gain            double          not null,
            exposure_time   double          not null,
            constraint id
                unique (id),
            constraint image_ibfk_1
                foreign key (chip_id) references chip (id)
        )
            engine = InnoDB;

        create index chip_id
            on image (chip_id);

        alter table image
            add primary key (id);

        create table relation_table
        (
            image_id bigint unsigned not null,
            flake_id bigint unsigned not null,
            constraint relation_table_ibfk_1
                foreign key (flake_id) references flake (id),
            constraint relation_table_ibfk_2
                foreign key (image_id) references image (id)
        )
            engine = InnoDB;

        create index flake_id
            on relation_table (flake_id);

        create index image_id
            on relation_table (image_id);

# Creation Query (MySQL) (Experimental)

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


# Problems

You __*ABSOLUTLY*__ need to specify `ENGINE = INNODB` as the default storage engine is `MyISAM`. That engine does __*NOT*__ support foreign keys, and you will have a bad time debugging the code without this knowledge (I had fun searching for 2 hours). To get the current default engine type `SELECT @@default_storage_engine;`