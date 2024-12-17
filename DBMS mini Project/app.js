const express = require("express");
const app = express();
const mysql = require("mysql2");
const path = require("path");
const methodOverride = require("method-override");
const ejsMate = require("ejs-mate");
const { v4: uuidv4 } = require("uuid");
uuidv4(); 

app.set("view engine", "ejs");
app.set("views", path.join(__dirname, "/views"));
app.use(express.urlencoded({extended : true}));
app.use(express.json());
app.use(methodOverride('_method'));
app.engine('ejs', ejsMate);
app.use(express.static(path.join(__dirname, "/public")));

const connection = mysql.createConnection({
    host: 'localhost',
    user: 'root',
    database: 'hotel',
    password: 'root',
  });

app.get("/", (req, res) => {
    res.render("listings/home.ejs");
})

app.get("/", (req, res) => {
    let q = `SELECT COUNT(*) FROM data`;
    try {
        connection.query(q, (err, result) => {
          if(err) throw err;
          let count = result[0]["COUNT(*)"];
          res.render("home.ejs", {count});
        })
      } catch(err) {
        console.log(err);
        res.send("Some error occured in database");
      }
})

// Back button redirecting
app.get("/listing", (req, res) => {
    res.redirect("/listings");
})

// index Route
app.get("/listings", (req, res) => {
    let q = `select * from data`;

    try {
        connection.query(q, (err, result) => {
            if(err) throw err;
            let allListings = result;
            res.render("listings/index.ejs", {allListings});
        });
    } catch(err) {
        res.send("some error occured");
    }
})

// New Route
app.get("/listings/new", (req, res) => {
    res.render("listings/new.ejs");
})

// Create Route
app.post("/listings", (req, res) => {
    let id = uuidv4();
    let {title, description, image, price, location, country} = req.body;

    if (!image) {
        image = 'https://plus.unsplash.com/premium_photo-1681922761648-d5e2c3972982?q=80&w=2070&auto=format&fit=crop&ixlib=rb-4.0.3&ixid=M3wxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8fA%3D%3D';
    }

    let q = `insert into data(id, title, description, image, price, location, country)values('${id}', '${title}', '${description}', '${image}', ${price}, '${location}', '${country}')`;

    try {
        connection.query(q, (err, result) => {
            if(err) throw err;
            res.redirect("/listings");
        })
    } catch(err) {
        res.send("some error occured");
    }
})

// Show Route
app.get("/listings/:id", async (req, res) => {
    let {id} = req.params;
    let q = `select * from data where id='${id}'`

    try {
        connection.query(q, (err, result) => {
            if(err) throw err;
            let listing = result[0];
            res.render("listings/show.ejs", {listing});
        })
    } catch(err) {
        res.send("some error occcured");
    }
})

// Edit Route
app.get("/listings/:id/edit", (req, res) => {
    let {id} = req.params;
    let q = `select * from data where id='${id}'`;

    try {
        connection.query(q, (err, result) => {
            if(err) throw err;
            let listing = result[0];
            res.render("listings/edit.ejs", {listing});
        }) 
    } catch(err) {
        res.send("some error has occured");
    }
})

// Update Route
app.put("/listings/:id", async (req, res) => {
    let {id} = req.params;
    let{title: formTitle, description: formDescription, image: formImage, price: formPrice, location: formLocation, country: formCountry} = req.body;
    
    let q = `select * from data where id='${id}'`;

    try {
        connection.query(q, (err, result) => {
            if(err) throw err;
            let listing = result[0];
            
            let q2 = `update data set title = '${formTitle}', description = '${formDescription}', image = '${formImage}', price = ${formPrice}, location = '${formLocation}', country = '${formCountry}' where id='${id}'`;
            connection.query(q2, (err, result) => {
                if(err) throw err;
                res.redirect(`/listings/${id}`);
            })
        })
    } catch(err) {
        res.send("some error occured");
    }
})

// Delete Route
app.delete("/listings/:id", async (req, res) => {
    let {id} = req.params;
    let q = `select * from data where id='${id}'`;

    try {
        connection.query(q, (err, result) => {
            if(err) throw err;
            let listing = result[0];

            let q2 = `delete from data where id='${id}'`;
            connection.query(q2, (err, result) => {
                if(err) throw err;
                // console.log("deleted");
                res.redirect("/listings");
            })
        })
    }catch(err) {
        res.send("error in database");
    }
})

app.listen(5050, () => {
    console.log("Server is listening to port 5050");
})