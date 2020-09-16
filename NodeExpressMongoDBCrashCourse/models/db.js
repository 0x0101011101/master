// Loading MongoDB module
const mongoose = require('mongoose');
const connexionString = 'mongodb://localhost:27017/EmployeeDB';

// Connect to database
mongoose.connect(connexionString, {
	useUnifiedTopology: true, 
	useNewUrlParser: true,
	useFindAndModify: false
	}, (err) => {
	if (!err) {
		console.log('MongoDB Connection Succeeded');
	} else {
		console.log('MongoDB Connection Error');
	}
});

// Loading Employee.Model 
require('./employee.model')