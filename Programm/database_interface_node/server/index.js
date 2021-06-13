const express = require("express");
const cors = require('cors')
const db = require("./db_conn");

const port = 3002;
const app = express();

app.use(cors())
app.use(express.json());
// app.use(function (req, res, next) {
//   res.setHeader("Access-Control-Allow-Origin", "*");
//   res.setHeader("Access-Control-Allow-Methods", "GET,POST,PUT,DELETE,OPTIONS");
//   res.setHeader(
//     "Access-Control-Allow-Headers",
//     "Content-Type, Access-Control-Allow-Headers"
//   );
//   next();
// });

app.get("/image/", (req, res) => {

  console.log("Get Received");

  const id = req.query.id;

  console.log(id)

  db
    .get_image_data(id)
    .then((data) => {
      res.status(200).send(data);
    })
    .catch((error) => {
      res.status(500).send(error);
    });
});

app.get("/flake/", (req, res) => {

  console.log("Get Received");

  const id = req.query.id;

  db
    .get_flake_data(id)
    .then((data) => {
      res.status(200).send(data);
    })
    .catch((error) => {
      res.status(500).send(error);
    });
});

app.listen(port, () => {
  console.log(`App running on port ${port}.`);
});
