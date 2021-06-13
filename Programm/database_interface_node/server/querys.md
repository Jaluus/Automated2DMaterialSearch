gets a flake with the given id and all the information for the chip and scan

        SELECT * FROM flake f
        join chip c on c.id = f.chip_id
        join scan s on s.id = c.scan_id
        WHERE f.id = ${id};

 gets all images associated with a given flake id

        SELECT * FROM image i
        WHERE i.flake_id = ${id};
