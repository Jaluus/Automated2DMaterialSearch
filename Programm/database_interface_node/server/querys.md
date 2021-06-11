Getting all flakes which are of the image with a given ID (unused)

        SELECT * FROM image
        JOIN relation_table ON image.id = relation_table.image_id
        JOIN flake on flake.id = relation_table.flake_id
        WHERE image.id = ${id};

getting all images which have a flake with the given id

        SELECT * FROM image
        JOIN relation_table ON image.id = relation_table.image_id
        JOIN flake on flake.id = relation_table.flake_id
        WHERE flake.id = ${id};