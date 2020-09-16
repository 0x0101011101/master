// Loading Mongoose Module
const mongoose = require('mongoose');

// Creation of Employee Schema
var employeeSchema = new mongoose.Schema({
	fullName: {
		type: String,
		required: 'This field is mandatory.'
	},
	email: {
		type: String
	},
	mobile: {
		type: String
	},
	city : { 
		type: String
	}
});

// Custom validation for email w/ RegExp
employeeSchema.path('email').validate((val) => {
    emailRegex = /^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/;
    return emailRegex.test(val);
}, 'Invalid e-mail.');

// Register Employee Schema in Mongoose
mongoose.model('Employee',employeeSchema);