const mysql = require('mysql')

const credentials = require("./credentials.json")

var conn = mysql.createConnection(credentials);

const get_flake_data = (id) => {

  const query = `
    ​SELECT * FROM flake f
    ​join chip c on c.id = f.chip_id
    ​join scan s on s.id = c.scan_id
    ​WHERE f.id = ${id};
    `;

  return new Promise(function (resolve, reject) {
    conn.query(query, (error, results) => {
      if (error) {
        reject(error);
      }
      resolve(results.rows);
    });
  });
};


const get_image_data = (id) => {

  const query = `
    SELECT * FROM image i
    WHERE i.id = ${id};
    `;

  return new Promise(function (resolve, reject) {
    conn.query(query, (error, results) => {
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
