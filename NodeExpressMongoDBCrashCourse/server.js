//console.log('Hello World');

// Runnable in dev mode : npm run dev
// Before running : 
// Install MongoDB - See https://www.youtube.com/watch?v=FwMwO8pXfq0
// Copy & Paste theses folder
// C:\Program Files\MongoDB\Server\4.2\data\
// C:\Program Files\MongoDB\Server\4.2\log\
// After installation, 
// Create c:/data/db folder on your primary HD
// Run mongoDB : mongod.exe in C:\Program Files\MongoDB\Server\4.2\bin>
// Run mongo Shell : mongo
// Type : show dbs
// Database creation : use <dbname>
// Ex Insert DB : db.books.insert({"name":"mongodb book"})
// To show all colections : Show collections;
// Find all document in collection book : db.books.find()
// Or use Mongo Compass. See https://www.mongodb.com/products/compass
// To launch it : use a browser and type URL : localhost:3000/employee

// Loading DB.js
require('./models/db.js');

// Loading Express module
const express = require('express');
const PORT = 3000;

// Loading Path module
const path = require('path');
const bodyparser = require('body-parser');
const exphbs = require('express-handlebars');

// Create the Express Server
var app = express();

// Configure Express MiddleWare
app.use(bodyparser.urlencoded({
	extended: true
}));

// View Engine Setup w/ Handlebars
app.engine('hbs', exphbs({
	extname: 'hbs',
	defaultLayout: 'mainLayout',
	layoutsDir: __dirname + '/views/layouts/'
}));

// Configure View - __dirname is base file directory path
app.set('views', path.join(__dirname,'views'));

// Setting View Engine as HandleBar
app.set('view engine', 'hbs');

// Import EmployeeController
const employeeController = require('./controllers/employeeController');

// Adding route for EmployeeController
app.use('/employee', employeeController);

// Server is listening
app.listen(PORT, () => {
	console.log('Server started on Port : ' + PORT);
});