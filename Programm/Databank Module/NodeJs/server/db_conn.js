const Pool = require("pg").Pool;
// THIS IS COMPLETE GARBAGE!
// Use a kerychain nextime
const pool = new Pool({
  user: "postgres",
  host: "localhost",
  database: "testing",
  password: "postgres",
  port: 5432,
});


const get_flake_data = (id) => {

  const query = `
    SELECT * FROM image
    JOIN relation_table ON image.id = relation_table.image_id
    JOIN flake on flake.id = relation_table.flake_id
    WHERE flake.id = ${id};
    `;

  return new Promise(function (resolve, reject) {
    pool.query(query, (error, results) => {
      if (error) {
        reject(error);
      }
      resolve(results.rows);
    });
  });
};


const get_image_data = (id) => {

  const query = `
    SELECT * FROM image
    JOIN relation_table ON image.id = relation_table.image_id
    JOIN flake on flake.id = relation_table.flake_id
    WHERE flake.id = ${id};
    `;

  return new Promise(function (resolve, reject) {
    pool.query(query, (error, results) => {
      if (error) {
        reject(error);
      }
      resolve(results.rows);
    });
  });
};


module.exports = {
  get_image_data,
  get_flake_data,
};
